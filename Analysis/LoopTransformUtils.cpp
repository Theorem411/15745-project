#include "LoopTransformUtils.h"

// Check whether we are capable of peeling this loop.
bool canTransform(Loop *L) {
  // Make sure the loop is in simplified form
  if (!L->isLoopSimplifyForm()) {
    errs() << "Loop is not simplified\n";
    return false;
  }

  // Only peel loops that contain a single exit
//   if (!L->getExitingBlock() || !L->getUniqueExitBlock()) {
//     errs() << "Loop has no unique exiting block!\n";
//     return false;
//   }

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

    assert(DT->getNode(InsertBot) && DT->getNode(InsertTop) && "insertTop or InsertBot is already null node pointers!");

    Function *F = L->getHeader()->getParent();
    LLVMContext &Context = InsertTop->getContext();
    IRBuilder<> Builder(Context);

    BasicBlock *NewPH = SplitEdge(InsertTop, InsertBot);

    DT->addNewBlock(NewPH, InsertTop);
    // assert(DT->getNode(NewPH) && "NewPH is null node pointers after addNewBlock!");
    // DT->changeImmediateDominator(DT->getNode(InsertBot), DT->getNode(NewPH));

    BasicBlock *NewBody = SplitEdge(NewPH, InsertBot);
    DT->addNewBlock(NewBody, NewPH);
    // assert(DT->getNode(NewBody) && "NewBody is null node pointers after addNewBlock!");
    // DT->changeImmediateDominator(DT->getNode(InsertBot), DT->getNode(NewBody));

    BasicBlock *NewExit = SplitEdge(NewBody, InsertBot);
    
    DT->addNewBlock(NewExit, NewBody);
    // assert(DT->getNode(NewExit) && "NewExit is null node pointers after addNewBlock!");
    // DT->changeImmediateDominator(DT->getNode(InsertBot), DT->getNode(NewExit));
    // assert(DT->getNode(InsertBot) && DT->getNode(InsertTop) && "insertTop or InsertBot is null node pointers after creating 3 blocks!");
    
    Instruction *OldTerminator = NewBody->getTerminator();
    Builder.SetInsertPoint(OldTerminator);
    PHINode *IndVar = Builder.CreatePHI(InitValue->getType(), 2, "pl.idv");
    Value *UpdateIndVar = Builder.CreateBinOp(StepOpcode, IndVar, StepValue, "");
    Value *LatchCond = Builder.CreateICmp(LatchCmpInst->getPredicate(), UpdateIndVar, FinalValue, "");
    Builder.CreateCondBr(LatchCond, NewBody, NewExit); 
    IndVar->addIncoming(InitValue, NewPH);
    IndVar->addIncoming(UpdateIndVar, NewBody);
    OldTerminator->eraseFromParent();

    // update cfg on InsertTop
    Instruction *OldTerminatorTop = InsertTop->getTerminator();
    Builder.SetInsertPoint(OldTerminatorTop);
    Value *PrecheckCond = Builder.CreateICmp(LatchCmpInst->getPredicate(), InitValue, FinalValue, "");
    Builder.CreateCondBr(PrecheckCond, NewPH, InsertBot);
    OldTerminatorTop->eraseFromParent();

    assert(DT->getNode(InsertTop) && "insertTop is null node pointers after changing cfg!");
    assert(DT->getNode(InsertBot) && "insertBot is null node pointers after changing cfg!");
    /// DEBUG: 
    // if (!InsertBot || !InsertTop) {
    //     errs() << "what the fuck is goin on?\n";
    // }
    // assert(DT->getNode(InsertBot) && DT->getNode(InsertTop) && "insertTop or InsertBot is null node pointers!");
    // DT->changeImmediateDominator(DT->getNode(InsertBot), DT->getNode(InsertTop));
    // errs() << "    first changeImmediateDominator succeeded!\n";
    // header's idom now should be InsertBot

    errs() << "    successfully updated cfg and change DT!\n";



    // Value *PrecheckCond = Builder.CreateICmp(LatchCmpInst->getPredicate(), InitValue, FinalValue, "");
    // Builder.CreateCondBr(PrecheckCond, NewPH, InsertBot);
    // OldTerminator->eraseFromParent();


    // // Create the PrologLoopBody and PrologLatch basic blocks
    // BasicBlock *NewPH = BasicBlock::Create(Context, InsertBot->getName() + ".pl.precheck", F, InsertBot);
    // BasicBlock *NewBody = BasicBlock::Create(Context, InsertBot->getName() + ".pl.ph", F, InsertBot);
    // BasicBlock *NewExit = BasicBlock::Create(Context, InsertBot->getName() + ".pl.body", F, InsertBot);

    // // inserttop
    // Instruction *OldTerminator = InsertTop->getTerminator();
    // Builder.SetInsertPoint(OldTerminator);
    // //// new preheader
    // Builder.SetInsertPoint(NewPH);
    // Builder.CreateBr(NewBody);
    // //// new body 
    // Builder.SetInsertPoint(NewBody);

    // // new loop exit
    // Builder.SetInsertPoint(NewExit);
    // Builder.CreateBr(InsertBot);
    
    // create new loop 
    Loop *Prolog = LI->AllocateLoop();
    Loop *ParentLoop = L->getParentLoop();
    if (ParentLoop) {
        ParentLoop->addChildLoop(Prolog);
        ParentLoop->addBasicBlockToLoop(NewPH, *LI);
    } else {
        LI->addTopLevelLoop(Prolog);
    }
    errs() << "    successfully registered prolog loop to LI\n"; 

    assert(L->getLoopPreheader() == InsertBot);

    // update LI
    Prolog->addBlockEntry(NewBody);

    // verify new loop
    Prolog->verifyLoop();
    errs() << "     Prolog loop passed verifyLoop\n";
    
    // simplify loop
    simplifyLoop(Prolog, DT, LI, SE, AC, MSSAU, true);
    errs() << "---- loop after all the simplify prolog: ----\n";
    errs() << "InsertTop: ";
    InsertTop->print(errs());
    errs() << "\n"; 
    errs() << "NewPH: ";
    NewPH->print(errs());
    errs() << "\n"; 
    errs() << "NewBody: ";
    NewBody->print(errs()); 
    errs() << "\n";
    errs() << "NewExit: ";
    NewExit->print(errs()); 
    errs() << "\n";
    errs() << "New InsertBot: ";
    InsertBot->print(errs()); 
    errs() << "\n";
    
    SE->forgetLoop(Prolog);
    // if (ParentLoop) {
    //     SE->forgetLoop(ParentLoop);
    //     simplifyLoop(ParentLoop, DT, LI, SE, AC, MSSAU, false);
    // } 
    simplifyLoop(L, DT, LI, SE, AC, MSSAU, true);
    
    errs() << "     prolog loop successfully simplified with preserved LCSSA\n";
    errs() << "     found prolog loop with header <" << Prolog->getHeader()->getName() << ">\n";
    errs() << "---- loop after simplify L: ----\n";
    errs() << "InsertTop: ";
    InsertTop->print(errs());
    errs() << "\n"; 
    errs() << "NewPH: ";
    NewPH->print(errs());
    errs() << "\n"; 
    errs() << "NewBody: ";
    NewBody->print(errs()); 
    errs() << "\n";
    errs() << "NewExit: ";
    NewExit->print(errs()); 
    errs() << "\n";
    errs() << "New InsertBot: ";
    InsertBot->print(errs()); 
    errs() << "\n";
    *NewIndVar = IndVar;
    return Prolog;
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

std::vector<BasicBlock *> LoopRPO(Loop *L) {
    BasicBlock *Header = L->getHeader();
    std::unordered_set<BasicBlock *> visited;
    std::stack<BasicBlock *> worklist;
    std::vector<BasicBlock *> postOrder;

    // Start with the header
    worklist.push(Header);
    visited.insert(Header);

    while (!worklist.empty()) {
        BasicBlock *BB = worklist.top();
        worklist.pop();

        postOrder.push_back(BB);

        // Add successors to the worklist if they haven't been visited
        for (BasicBlock *Succ : successors(BB)) {
            if (L->contains(Succ)) {
                if (visited.find(Succ) == visited.end()) {
                    visited.insert(Succ);
                    worklist.push(Succ);
                }
            }
        }
    }

    // Reverse to get reverse post-order
    std::reverse(postOrder.begin(), postOrder.end());
    return postOrder;
}

void cloneLoopBody(Loop *L, BasicBlock *InsertTop, BasicBlock *InsertBot, BasicBlock *Exit,
                    SmallVectorImpl<BasicBlock *> &NewBlocks, // LoopBlocksDFS &LoopBlocks, 
                    ValueToValueMapTy &VMap, ValueToValueMapTy &LVMap, DominatorTree *DT,
                    LoopInfo *LI) {
    BasicBlock *Header = L->getHeader();
    BasicBlock *Latch = L->getLoopLatch();
    BasicBlock *PreHeader = L->getLoopPreheader();

    Function *F = Header->getParent();
    // LoopBlocksDFS::RPOIterator BlockBegin = LoopBlocks.beginRPO();
    // LoopBlocksDFS::RPOIterator BlockEnd = LoopBlocks.endRPO();
    Loop *ParentLoop = L->getParentLoop();

    // For each block in the original loop, create a new copy,
    // and update the value map with the newly created values.
    // for (LoopBlocksDFS::RPOIterator BB = BlockBegin; BB != BlockEnd; ++BB) {
    for (auto *BB : LoopRPO(L)) {
        // errs() << "- cloned: " << (&*BB)->getName() << "\n";
        BasicBlock *NewBB = CloneBasicBlock(BB, VMap, ".peel", F);
        NewBlocks.push_back(NewBB);

        // if (ParentLoop)
        // ParentLoop->addBasicBlockToLoop(NewBB, *LI);

        VMap[BB] = NewBB;

        // If dominastor tree is available, insert nodes to represent cloned blocks.
        // if (DT) {
        //     if (Header == *BB)
        //         DT->addNewBlock(NewBB, InsertTop);
        //     else {
        //         DomTreeNode *IDom = DT->getNode(*BB)->getIDom();
        //         // VMap must contain entry for IDom, as the iteration order is RPO.
        //         DT->addNewBlock(NewBB, cast<BasicBlock>(VMap[IDom->getBlock()]));
        //     }
        // }
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
    // if (DT)
    //     DT->changeImmediateDominator(InsertBot, NewLatch);

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