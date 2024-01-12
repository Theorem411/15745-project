#include "LoopTransformUtils.h"

// Check whether we are capable of peeling this loop.
bool canTransform(Loop *L) {
  // Make sure the loop is in simplified form
  if (!L->isLoopSimplifyForm()) {
    errs() << "Loop is not simplified\n";
    return false;
  }

  // Only peel loops that contain a single exit
  if (!L->getExitingBlock() || !L->getUniqueExitBlock()) {
    errs() << "Loop has no unique exiting block!\n";
    return false;
  }

  // Don't try to peel loops where the latch is not the exiting block.
  // This can be an indication of two different things:
  // 1) The loop is not rotated.
  // 2) The loop contains irreducible control flow that involves the latch.
  if (L->getLoopLatch() != L->getExitingBlock()) {
    errs() << "Loop has no latch!\n";
    return false;
  }

  if (!L->isSafeToClone()) {
    errs() << "Loop isn't safe to clone\n";
    return false;
  }

  if (L->getHeader()->hasAddressTaken()) {
    // The loop-rotate pass can be helpful to avoid this in many cases.
    errs() << "Loop header has address taken\n";
    return false;
  }

  return true;
}

void updateBranchWeights(BasicBlock *Header, BranchInst *LatchBR, uint64_t &PeeledHeaderWeight) {
  // FIXME: Pick a more realistic distribution.
  // Currently the proportion of weight we assign to the fall-through
  // side of the branch drops linearly with the iteration number, and we use
  // a 0.9 fudge factor to make the drop-off less sharp...
  unsigned AvgIters = 1; 
  unsigned IterNumber = 0; // default the first iter

  if (PeeledHeaderWeight) {
    uint64_t FallThruWeight =
        PeeledHeaderWeight * ((float)(AvgIters - IterNumber) / AvgIters * 0.9);
    uint64_t ExitWeight = PeeledHeaderWeight - FallThruWeight;
    PeeledHeaderWeight -= ExitWeight;

    unsigned HeaderIdx = (LatchBR->getSuccessor(0) == Header ? 0 : 1);
    MDBuilder MDB(LatchBR->getContext());
    MDNode *WeightNode =
        HeaderIdx ? MDB.createBranchWeights(ExitWeight, FallThruWeight)
                  : MDB.createBranchWeights(FallThruWeight, ExitWeight);
    LatchBR->setMetadata(LLVMContext::MD_prof, WeightNode);
  }
}

Loop *prependLoop(Loop *L, BasicBlock *InsertTop, BasicBlock *InsertBot,
  Value *InitValue, Value *FinalValue, Value *StepValue, Instruction::BinaryOps StepOpcode, 
  ICmpInst *LatchCmpInst, LoopInfo *LI, ScalarEvolution *SE, DominatorTree *DT, 
  AssumptionCache *AC, MemorySSAUpdater* MSSAU, PHINode** NewIndVar) {
    Function *F = L->getHeader()->getParent();

    LLVMContext &Context = InsertTop->getContext();
    IRBuilder<> Builder(Context);

    // Create the PrologLoopBody and PrologLatch basic blocks
    BasicBlock *Precheck = BasicBlock::Create(Context, InsertBot->getName() + ".pl.precheck", F, InsertBot);
    BasicBlock *Preheader = BasicBlock::Create(Context, InsertBot->getName() + ".pl.ph", F, InsertBot);
    BasicBlock *LoopBody = BasicBlock::Create(Context, InsertBot->getName() + ".pl.body", F, InsertBot);

    //// inserttop
    Instruction *OldTerminator = InsertTop->getTerminator();
    Builder.SetInsertPoint(OldTerminator);
    Builder.CreateBr(Precheck);
    OldTerminator->eraseFromParent();
    //// precheck 
    Builder.SetInsertPoint(Precheck);
    Value *PrecheckCond = Builder.CreateICmp(LatchCmpInst->getPredicate(), 
        InitValue, FinalValue, "");
    Builder.CreateCondBr(PrecheckCond, Preheader, InsertBot);
    //// preheader
    Builder.SetInsertPoint(Preheader);
    Builder.CreateBr(LoopBody);
    //// loop body
    // create indvar
    Builder.SetInsertPoint(LoopBody);
    PHINode *IndVar = Builder.CreatePHI(InitValue->getType(), 2, "pl.idv");
    IndVar->addIncoming(InitValue, Preheader);
    // connect loop body 
    Value *UpdateIndVar = Builder.CreateBinOp(StepOpcode, IndVar, StepValue, "");
    IndVar->addIncoming(UpdateIndVar, LoopBody);
    Value *LatchCond = Builder.CreateICmp(LatchCmpInst->getPredicate(), 
        UpdateIndVar, FinalValue, "");
    Builder.CreateCondBr(LatchCond, LoopBody, InsertBot); 
    
    // create new loop 
    Loop *PrologL = LI->AllocateLoop();
    Loop *ParentLoop = L->getParentLoop();
    if (ParentLoop) {
        ParentLoop->addChildLoop(PrologL);
        ParentLoop->addBasicBlockToLoop(Preheader, *LI);
    } else {
        LI->addTopLevelLoop(PrologL);
    }

    // update DT
    DT->addNewBlock(Precheck, InsertTop);
    DT->addNewBlock(Preheader, Precheck);
    DT->addNewBlock(LoopBody, Preheader);

    // update LI
    PrologL->addBlockEntry(LoopBody);

    // verify new loop
    PrologL->verifyLoop();
    
    // simplify loop
    simplifyLoop(PrologL, DT, LI, SE, AC, MSSAU, true);
    SE->forgetLoop(PrologL);
    if (ParentLoop) {
        SE->forgetLoop(ParentLoop);
        simplifyLoop(ParentLoop, DT, LI, SE, AC, MSSAU, true);
    } else {
        simplifyLoop(L, DT, LI, SE, AC, MSSAU, true);
    }

    *NewIndVar = IndVar;
    return PrologL;
}
// original cloneLoopWithPreheader can't work on outer loops
static Loop *myCloneLoopWithPreheader(BasicBlock *Before, BasicBlock *LoopDomBB,
                                   Loop *L, ValueToValueMapTy &VMap,
                                   const Twine &NameSuffix, LoopInfo *LI,
                                   DominatorTree *DT,
                                   SmallVectorImpl<BasicBlock *> &Blocks) {
    Function *F = L->getHeader()->getParent();
    Loop *ParentLoop = L->getParentLoop();

    Loop *NewLoop = LI->AllocateLoop();
    if (ParentLoop)
        ParentLoop->addChildLoop(NewLoop);
    else
        LI->addTopLevelLoop(NewLoop);

    BasicBlock *OrigPH = L->getLoopPreheader();
    assert(OrigPH && "No preheader");
    BasicBlock *NewPH = CloneBasicBlock(OrigPH, VMap, NameSuffix, F);
    // To rename the loop PHIs.
    VMap[OrigPH] = NewPH;
    Blocks.push_back(NewPH);

    // Update LoopInfo.
    if (ParentLoop)
        ParentLoop->addBasicBlockToLoop(NewPH, *LI);

    // Update DominatorTree.
    DT->addNewBlock(NewPH, LoopDomBB);

    for (BasicBlock *BB : L->getBlocks()) {
        BasicBlock *NewBB = CloneBasicBlock(BB, VMap, NameSuffix, F);
        VMap[BB] = NewBB;

        // Update LoopInfo.
        NewLoop->addBasicBlockToLoop(NewBB, *LI);

        // Add DominatorTree node. After seeing all blocks, update to correct IDom.
        DT->addNewBlock(NewBB, NewPH);

        Blocks.push_back(NewBB);
    }

    for (BasicBlock *BB : L->getBlocks()) {
        // Update DominatorTree.
        BasicBlock *IDomBB = DT->getNode(BB)->getIDom()->getBlock();
        DT->changeImmediateDominator(cast<BasicBlock>(VMap[BB]),
                                    cast<BasicBlock>(VMap[IDomBB]));
    }

    // Move them physically from the end of the block list.
    F->getBasicBlockList().splice(Before->getIterator(), F->getBasicBlockList(),
                                    NewPH);
    F->getBasicBlockList().splice(Before->getIterator(), F->getBasicBlockList(),
                                    NewLoop->getHeader()->getIterator(), F->end());

    return NewLoop;
}

void cloneLoopBody(Loop *L, BasicBlock *InsertTop, BasicBlock *InsertBot, BasicBlock *Exit,
                    SmallVectorImpl<BasicBlock *> &NewBlocks, LoopBlocksDFS &LoopBlocks, 
                    ValueToValueMapTy &VMap, ValueToValueMapTy &LVMap, DominatorTree *DT,
                    LoopInfo *LI) {
    BasicBlock *Header = L->getHeader();
    BasicBlock *Latch = L->getLoopLatch();
    BasicBlock *PreHeader = L->getLoopPreheader();

    Function *F = Header->getParent();
    LoopBlocksDFS::RPOIterator BlockBegin = LoopBlocks.beginRPO();
    LoopBlocksDFS::RPOIterator BlockEnd = LoopBlocks.endRPO();
    Loop *ParentLoop = L->getParentLoop();

    // For each block in the original loop, create a new copy,
    // and update the value map with the newly created values.
    for (LoopBlocksDFS::RPOIterator BB = BlockBegin; BB != BlockEnd; ++BB) {
        BasicBlock *NewBB = CloneBasicBlock(*BB, VMap, ".peel", F);
        NewBlocks.push_back(NewBB);

        if (ParentLoop)
        ParentLoop->addBasicBlockToLoop(NewBB, *LI);

        VMap[*BB] = NewBB;

        // If dominator tree is available, insert nodes to represent cloned blocks.
        if (DT) {
            if (Header == *BB)
                DT->addNewBlock(NewBB, InsertTop);
            else {
                DomTreeNode *IDom = DT->getNode(*BB)->getIDom();
                // VMap must contain entry for IDom, as the iteration order is RPO.
                DT->addNewBlock(NewBB, cast<BasicBlock>(VMap[IDom->getBlock()]));
            }
        }
    }

    // Hook-up the control flow for the newly inserted blocks.
    // The new header is hooked up directly to the "top", which is either
    // the original loop preheader (for the first iteration) or the previous
    // iteration's exiting block (for every other iteration)
    InsertTop->getTerminator()->setSuccessor(0, cast<BasicBlock>(VMap[Header]));

    // Similarly, for the latch:
    // The original exiting edge is still hooked up to the loop exit.
    // The backedge now goes to the "bottom", which is either the loop's real
    // header (for the last peeled iteration) or the copied header of the next
    // iteration (for every other iteration)
    BasicBlock *NewLatch = cast<BasicBlock>(VMap[Latch]);
    BranchInst *LatchBR = cast<BranchInst>(NewLatch->getTerminator());
    unsigned HeaderIdx = (LatchBR->getSuccessor(0) == Header ? 0 : 1);
    LatchBR->setSuccessor(HeaderIdx, InsertBot);
    LatchBR->setSuccessor(1 - HeaderIdx, Exit);
    if (DT)
        DT->changeImmediateDominator(InsertBot, NewLatch);

    // The new copy of the loop body starts with a bunch of PHI nodes
    // that pick an incoming value from either the preheader, or the previous
    // loop iteration. Since this copy is no longer part of the loop, we
    // resolve this statically:
    // For the first iteration, we use the value from the preheader directly.
    // For any other iteration, we replace the phi with the value generated by
    // the immediately preceding clone of the loop body (which represents
    // the previous iteration).
    for (BasicBlock::iterator I = Header->begin(); isa<PHINode>(I); ++I) {
        PHINode *NewPHI = cast<PHINode>(VMap[&*I]);
        VMap[&*I] = NewPHI->getIncomingValueForBlock(PreHeader);
        cast<BasicBlock>(VMap[Header])->getInstList().erase(NewPHI);
    }

    // Fix up the outgoing values - we need to add a value for the iteration
    // we've just created. Note that this must happen *after* the incoming
    // values are adjusted, since the value going out of the latch may also be
    // a value coming into the header.
    for (BasicBlock::iterator I = Exit->begin(); isa<PHINode>(I); ++I) {
        PHINode *PHI = cast<PHINode>(I);
        Value *LatchVal = PHI->getIncomingValueForBlock(Latch);
        Instruction *LatchInst = dyn_cast<Instruction>(LatchVal);
        if (LatchInst && L->contains(LatchInst))
        LatchVal = VMap[LatchVal];
        PHI->addIncoming(LatchVal, cast<BasicBlock>(VMap[Latch]));
    }

    // LastValueMap is updated with the values for the current loop
    // which are used the next time this function is called.
    for (const auto &KV : VMap)
        LVMap[KV.first] = KV.second;
}

Loop *cloneLoop(Loop *L, BasicBlock *InsertBefore, BasicBlock *LoopDomBB, 
            SmallVectorImpl<BasicBlock *> &NewBlocks,
            ValueToValueMapTy &VMap, LoopInfo *LI, ScalarEvolution *SE, 
            DominatorTree *DT, AssumptionCache *AC, MemorySSAUpdater* MSSAU) {
    // Clone the original loop, insert the clone before the "InsertBefore" BB.
    Function *F = L->getHeader()->getParent(); 

    // clone loop and update DT & LI
    Loop *NewLoop = myCloneLoopWithPreheader(InsertBefore, LoopDomBB, L, VMap, "", LI, DT, NewBlocks);
    assert(NewLoop && "cloneLoop run out of memory");

    // Update instructions referencing the original loop basic blocks to
    // reference the corresponding block in the cloned loop.
    VMap[L->getExitBlock()] = InsertBefore;
    remapInstructionsInBlocks(NewBlocks, VMap);

    // Make the predecessor of original loop jump to the cloned loop.
    LoopDomBB->getTerminator()->replaceUsesOfWith(InsertBefore, NewLoop->getLoopPreheader());

    // // For each block in the original loop, create a new copy,
    // // and update the value map with the newly created values.
    // LoopBlocksDFS::RPOIterator BlockBegin = LoopBlocks.beginRPO();
    // LoopBlocksDFS::RPOIterator BlockEnd = LoopBlocks.endRPO();
    // Loop *ParentLoop = L->getParentLoop();
    // for (LoopBlocksDFS::RPOIterator BB = BlockBegin; BB != BlockEnd; ++BB) {
    //     BasicBlock *NewBB = CloneBasicBlock(*BB, VMap, ".peel", F);
    //     NewBlocks.push_back(NewBB);

    //     if (ParentLoop)
    //     ParentLoop->addBasicBlockToLoop(NewBB, *LI);

    //     VMap[*BB] = NewBB;

    //     // If dominator tree is available, insert nodes to represent cloned blocks.
    //     if (DT) {
    //         if (Header == *BB)
    //             DT->addNewBlock(NewBB, InsertTop);
    //         else {
    //             DomTreeNode *IDom = DT->getNode(*BB)->getIDom();
    //             // VMap must contain entry for IDom, as the iteration order is RPO.
    //             DT->addNewBlock(NewBB, cast<BasicBlock>(VMap[IDom->getBlock()]));
    //         }
    //     }
    // }

    // Simplify
    Loop *ParentLoop = L->getParentLoop();
    if (ParentLoop) {
        SE->forgetLoop(ParentLoop);
        simplifyLoop(ParentLoop, DT, LI, SE, AC, MSSAU, true);
    } else {
        simplifyLoop(L, DT, LI, SE, AC, MSSAU, true);
    }
    
    return NewLoop;
}   

Optional<Loop::LoopBounds> getBounds(Loop *L, ScalarEvolution &SE) {
  if (PHINode *IndVar = L->getInductionVariable(SE))
    return Loop::LoopBounds::getBounds(*L, *IndVar, SE);

  return None;
}