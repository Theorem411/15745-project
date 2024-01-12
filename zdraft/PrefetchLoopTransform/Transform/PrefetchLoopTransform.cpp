#include "PrefetchLoopTransform.h"

Instruction *PrefetchLoopTransform::computeSplitPoint(Loop *L, 
    unsigned PrefetchItersAhead, unsigned UnrollTimes, Instruction *InsertBefore) const {
    Optional<Loop::LoopBounds> Bounds = getBounds(L, SE);
    if (!Bounds.hasValue()) {
        errs() << "Unable to retrieve the loop bounds\n";
        return nullptr;
    }
    // PHINode *IndVar = L->getInductionVariable(SE);
    // if (!IndVar) {
    //     errs() << "Unable to compute induction variable\n";
    //     return nullptr;
    // }
    Value &IVFinalVal = Bounds->getFinalIVValue();
    Instruction &StepInst = Bounds->getStepInst();
    BinaryOperator *StepInstBop = dyn_cast<BinaryOperator>(&StepInst);

    Value *StepValue;
    if (L->isLoopInvariant(StepInstBop->getOperand(0))) {
        StepValue = StepInstBop->getOperand(0);
    } else if (L->isLoopInvariant(StepInstBop->getOperand(1))) {
        StepValue = StepInstBop->getOperand(1);
    } else {
        errs() << "Unable to compute Step Value \n";
        return nullptr;
    }

    // create IVFinalVal - PrefetchItersAhead * (StepValue * UnrollTimes)
    Value *ConstUR = ConstantInt::get(StepValue->getType(), UnrollTimes);
    Value *DeltaValue = BinaryOperator::Create(Instruction::Mul, StepValue, ConstUR, "", InsertBefore);

    Value *ConstIA = ConstantInt::get(StepValue->getType(), PrefetchItersAhead);
    Value *Mul = BinaryOperator::Create(Instruction::Mul, ConstIA, DeltaValue, "", InsertBefore);
    return BinaryOperator::Create(Instruction::Sub, &IVFinalVal, Mul, "", InsertBefore);
}

LoopUnrollResult PrefetchLoopTransform::unrollLoop(Loop *L, unsigned UnrollTimes, 
        Loop ** RemainderLoop) {
    bool PreserveLCSSA = true;
    UnrollLoopOptions ULO; 
    ULO.Count = UnrollTimes;  
    ULO.Force = false;
    ULO.Runtime = false;
    ULO.AllowExpensiveTripCount = true;
    ULO.UnrollRemainder = false;
    ULO.ForgetAllSCEV = false;
    LoopUnrollResult res = UnrollLoop(L, ULO, &LI, &SE, &DT, &AC, &TI, &TTI, 
        &ORE, PreserveLCSSA, RemainderLoop);
    
    bool SimplifyIVs = false;
    simplifyLoopAfterUnroll(L, SimplifyIVs, &LI, &SE, &DT, &AC, &TTI); 
    
    
    return res;
}

Loop *PrefetchLoopTransform::splitLoop(Loop *L, unsigned PrefetchItersAhead, unsigned UnrollTimes) {
    if (!canTransform(L)) {
        errs() << "can't split loop \n";
        return nullptr;
    }
    
    Optional<Loop::LoopBounds> Bounds = L->getBounds(SE);
    if (!Bounds.hasValue()) {
        errs() << "Unable to retrieve the loop bounds\n";
        return nullptr;
    }

    // Split the loop exit to create an insertion point for the cloned loop.
    BasicBlock *UniqueExit = L->getUniqueExitBlock();
    BasicBlock *LoopDomBB = UniqueExit;
    
    BasicBlock *InsertBefore =
        SplitBlock(UniqueExit, UniqueExit->getTerminator(), &DT, &LI);
    // Clone the original l oop, and insert the clone after the original loop.
    SmallVector<BasicBlock *, 4> NewBlocks;
    ValueToValueMapTy VMap;
    Loop *EpilogL = cloneLoop(L, InsertBefore, LoopDomBB, NewBlocks, VMap, &LI, &SE, &DT, &AC, &MSSAU);
    if (!EpilogL) {
        errs() << "Unable to clone loop to a epilog loop!\n";
        return nullptr;
    }
    // compute split point & insert 
    Instruction *Split1 = computeSplitPoint(L, PrefetchItersAhead, UnrollTimes,
                            L->getLoopPreheader()->getTerminator());
    if (!Split1) {
        errs() << "Unable to compute split point\n";
        return nullptr;
    }
    // Modify the upper bound of the main loop.
    ICmpInst *LatchCmpInst = L->getLatchCmpInst();
    if (!LatchCmpInst) {
        errs() << "Unable to find the latch comparison instruction\n";
        return nullptr;
    }
    LatchCmpInst->setOperand(1, Split1);

    // Modify the lower bound of the epilog loop.
    Instruction *Split2 = computeSplitPoint(EpilogL, PrefetchItersAhead, UnrollTimes, 
                            EpilogL->getLoopPreheader()->getTerminator());
    if (!Split2) {
        errs() << "Unable to compute split point\n";
        return nullptr;
    }
    PHINode *IndVar = EpilogL->getInductionVariable(SE);
    if (!IndVar) {
        errs() << "Unable to find the induction variable PHI node\n";
        return nullptr;
    }
    IndVar->setIncomingValueForBlock(EpilogL->getLoopPreheader(), Split2);

    return EpilogL;
}

Loop *PrefetchLoopTransform::insertProlog(Loop *L, unsigned PrefetchItersAhead, PHINode **NewIndVar) {
    if (!canTransform(L)) {
        errs() << "can't insert prolog loop \n";
        return nullptr;
    }
    Optional<Loop::LoopBounds> Bounds = L->getBounds(SE);
    if (!Bounds.hasValue()) {
        errs() << "Unable to retrieve the loop bounds\n";
        return nullptr;
    }
    // prepare necessary boundary blocks
    BasicBlock *OldPH = L->getLoopPreheader();
    BasicBlock *Header = L->getHeader();
    ICmpInst *LatchCmpInst = L->getLatchCmpInst();
    if (!LatchCmpInst) {
        errs() << "Unable to retrieve the loop latch\n";
        return nullptr;
    }
    // compute indvar bounds
    Value &IVInitialVal = Bounds->getInitialIVValue();
    Value &IVFinalVal = Bounds->getFinalIVValue();
    Value &StepInst = Bounds->getStepInst();
    Value *StepValue = Bounds->getStepValue();

    BinaryOperator *StepInstBinOp = dyn_cast<BinaryOperator>(&StepInst);
    if (!StepInstBinOp) {
        errs() << "Step instruction is not a binary operator; cannot reconstruct latch block from this\n";
        return nullptr;
    } 
    Instruction::BinaryOps StepOpcode = StepInstBinOp->getOpcode();
    if (!StepValue) {
        errs() << "Unable to compute loop bound info\n";
        return nullptr;
    }

    BasicBlock *NewPH = SplitBlock(OldPH, OldPH->getTerminator(), &DT, &LI);

    Loop *PrologL = prependLoop(L, OldPH, NewPH, &IVInitialVal, &IVFinalVal, 
                                StepValue, StepOpcode, LatchCmpInst, &LI, &SE, &DT,
                                &AC, &MSSAU, NewIndVar);
    return PrologL; 
}

BasicBlock *PrefetchLoopTransform::peelFirstIter(Loop *L) {
    if (!canTransform(L)) {
        errs() << "can't peel loop \n";
        return nullptr;
    }

    LoopBlocksDFS LoopBlocks(L);
    LoopBlocks.perform(&LI);

    BasicBlock *Header = L->getHeader();
    BasicBlock *PreHeader = L->getLoopPreheader();
    BasicBlock *Latch = L->getLoopLatch();
    BasicBlock *Exit = L->getUniqueExitBlock();

    Function *F = Header->getParent();

    // Set up all the necessary basic blocks. It is convenient to split the
    // preheader into 3 parts - two blocks to anchor the peeled copy of the loop
    // body, and a new preheader for the "real" loop.
    BasicBlock *InsertTop = SplitEdge(PreHeader, Header, &DT, &LI);

    BasicBlock *InsertBot =
        SplitBlock(InsertTop, InsertTop->getTerminator(), &DT, &LI);
    BasicBlock *NewPreHeader =
        SplitBlock(InsertBot, InsertBot->getTerminator(), &DT, &LI);

    InsertTop->setName(Header->getName() + ".peel.begin");
    InsertBot->setName(Header->getName() + ".peel.next");
    NewPreHeader->setName(PreHeader->getName() + ".peel.newph");

    errs() << "<InsertTop>: " << InsertTop->getName() << "<InsertBot>: " << InsertBot->getName() 
    << "NewPreheader" << NewPreHeader->getName() << "\n\n";

    // Peeling the first iteration transforms.
    //
    // PreHeader:
    // ...
    // Header:
    //   LoopBody
    //   If (cond) goto Header
    // Exit:
    //
    // into
    //
    // InsertTop:
    //   LoopBody
    //   If (!cond) goto Exit
    // InsertBot:
    // NewPreHeader:
    // ...
    // Header:
    //  LoopBody
    //  If (cond) goto Header
    // Exit:
    //

    ValueToValueMapTy LVMap;

    // If we have branch weight information, we'll want to update it for the
    // newly created branches.
    BranchInst *LatchBR =
        cast<BranchInst>(cast<BasicBlock>(Latch)->getTerminator());
    unsigned HeaderIdx = (LatchBR->getSuccessor(0) == Header ? 0 : 1);

    uint64_t TrueWeight, FalseWeight;
    uint64_t ExitWeight = 0, CurHeaderWeight = 0;
    if (LatchBR->extractProfMetadata(TrueWeight, FalseWeight)) {
        ExitWeight = HeaderIdx ? TrueWeight : FalseWeight;
        // The # of times the loop body executes is the sum of the exit block
        // weight and the # of times the backedges are taken.
        CurHeaderWeight = TrueWeight + FalseWeight;
    }

    // For each peeled-off iteration, make a copy of the loop.
    SmallVector<BasicBlock *, 8> NewBlocks;
    ValueToValueMapTy VMap;

    // Subtract the exit weight from the current header weight -- the exit
    // weight is exactly the weight of the previous iteration's header.
    // FIXME: due to the way the distribution is constructed, we need a
    // guard here to make sure we don't end up with non-positive weights.
    if (ExitWeight < CurHeaderWeight)
        CurHeaderWeight -= ExitWeight;
    else
        CurHeaderWeight = 1;

    cloneLoopBody(L, InsertTop, InsertBot, Exit, NewBlocks, LoopBlocks, VMap, LVMap, &DT, &LI);
    // Remap to use values from the current iteration instead of the
    // previous one.
    remapInstructionsInBlocks(NewBlocks, VMap);

    // Latches of the cloned loops dominate over the loop exit, so idom of the
    // latter is the first cloned loop body, as original PreHeader dominates
    // the original loop body.
    DT.changeImmediateDominator(Exit, cast<BasicBlock>(LVMap[Latch]));

    updateBranchWeights(InsertBot, cast<BranchInst>(VMap[LatchBR]), ExitWeight);

    // InsertTop = InsertBot;
    // InsertBot = SplitBlock(InsertBot, InsertBot->getTerminator(), &DT, &LI);
    // InsertBot->setName(Header->getName() + ".peel.next");

    F->getBasicBlockList().splice(InsertBot->getIterator(),
                                    F->getBasicBlockList(),
                                    NewBlocks[0]->getIterator(), F->end());

    // Now adjust the phi nodes in the loop header to get their initial values
    // from the last peeled-off iteration instead of the preheader.
    for (BasicBlock::iterator I = Header->begin(); isa<PHINode>(I); ++I) {
        PHINode *PHI = cast<PHINode>(I);
        Value *NewVal = PHI->getIncomingValueForBlock(Latch);
        Instruction *LatchInst = dyn_cast<Instruction>(NewVal);
        if (LatchInst && L->contains(LatchInst))
            NewVal = LVMap[LatchInst];

        PHI->setIncomingValue(PHI->getBasicBlockIndex(NewPreHeader), NewVal);
    }

    // Adjust the branch weights on the loop exit.
    if (ExitWeight) {
        // The backedge count is the difference of current header weight and
        // current loop exit weight. If the current header weight is smaller than
        // the current loop exit weight, we mark the loop backedge weight as 1.
        uint64_t BackEdgeWeight = 0;
        if (ExitWeight < CurHeaderWeight)
            BackEdgeWeight = CurHeaderWeight - ExitWeight;
        else
            BackEdgeWeight = 1;
        MDBuilder MDB(LatchBR->getContext());
        MDNode *WeightNode =
            HeaderIdx ? MDB.createBranchWeights(ExitWeight, BackEdgeWeight)
                        : MDB.createBranchWeights(BackEdgeWeight, ExitWeight);
        LatchBR->setMetadata(LLVMContext::MD_prof, WeightNode);
    }

    // If the loop is nested, we changed the parent loop, update SE.
    if (Loop *ParentLoop = L->getParentLoop()) {
        SE.forgetLoop(ParentLoop);
        // // FIXME: Incrementally update loop-simplify
        simplifyLoop(ParentLoop, &DT, &LI, &SE, &AC, &MSSAU, true);
    } else {
        // FIXME: Incrementally update loop-simplify
        simplifyLoop(L, &DT, &LI, &SE, &AC, &MSSAU, true);
    }
    return InsertTop;
}

bool PrefetchLoopTransform::run(Loop *L, bool temporal, bool spatial, 
    unsigned PrefetchItersAhead, unsigned UnrollTimes) {
    if (!temporal && !spatial) {
        errs() << "No transformation\n";
        return false;
    }
    if (!canTransform(L)) {
        errs() << "No transformation because Loop is invalid\n";
        return false;
    }
    PHINode *IndVarOrig = L->getInductionVariable(SE);
    if (!IndVarOrig) {
        errs() << "No transformation because loop has no induction variable\n";
        return false;
    }

    if (spatial) {
    // step 1: unroll L by UnrollTimes
        // errs() << "-- begin unrolling --------------------\n";
        // Loop *RemainderLoop = nullptr;
        // switch (unrollLoop(L, UnrollTimes, &RemainderLoop)) {
        //     case LoopUnrollResult::Unmodified: {
        //         errs() << "Loop " << L->getLoopPreheader()->getName() << " is unmodified\n";
        //         break;
        //     }
        //     case LoopUnrollResult::PartiallyUnrolled: {
        //         errs() << "Loop " << L->getLoopPreheader()->getName() << " is PartiallyUnrolled\n";
        //         break;
        //     }
        //     case LoopUnrollResult::FullyUnrolled: {
        //         errs() << "Loop " << L->getLoopPreheader()->getName() << " is fully unrolled\n";
        //         assert(false && "Shouldn't have Full Unroll result!");
        //     }
        // }
        // PHINode *IndvarUnroll = L->getInductionVariable(SE);
        // if (!IndvarUnroll) {
        //     errs() << "cannot find induction variable after unroll\n";
        //     return false;
        // }
        // IndvarUnroll->print(errs());
        // errs() << "\n";

    // step 2: split epilog off L
        errs() << "-- begin loop splitting ---------------\n";
        Loop *LEpil = splitLoop(L, PrefetchItersAhead, UnrollTimes);
        if (!LEpil) {
            errs() << "cannot split loop for spatial locality transformation!\n";
            return true;
        }
        PHINode *IndVarEpi = LEpil->getInductionVariable(SE);
        errs() << "Loop LEpil has new induction variable ";
        IndVarEpi->print(errs());
        errs() << "\n";

    // split 3: insert empty prolog 
        errs() << "-- begin prolog inserting --------------\n";
        PHINode *LProlIndVar = nullptr;
        Loop *LProl = insertProlog(L, PrefetchItersAhead, &LProlIndVar);
        if (!LProl) {
            errs() << "cannot insert empty prolog loop for spatial locality transformation!\n";
            return true;
        }
        if (!LProlIndVar) {
            errs() << "cannot find indvar for prolog loop!\n";
            return true;
        }
        errs() << "Loop prolog has induction variable: ";
        LProlIndVar->print(errs());
        errs() << "\n";
    }

    if (temporal) {
    // perform peeling on the first iteration 
        BasicBlock *InsertTop = peelFirstIter(L);
        if (!InsertTop) {
            errs() << "Loop Peeling failed\n";
            return false;
        }
        errs() << "Peel off insertion point at " << InsertTop->getName() << "\n";
    }
    errs() << "\n";
    return true;
}

