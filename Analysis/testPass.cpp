#include "testPass.h"
/// HELPER: Loop Transformation helper function
Instruction *TestPassImpl::computeSplitPoint(Loop *L, 
    unsigned ItersAhead, unsigned UnrollTimes, Instruction *InsertBefore) const {
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

    // create IVFinalVal - ItersAhead * (StepValue * UnrollTimes)
    Value *ConstUR = ConstantInt::get(StepValue->getType(), UnrollTimes);
    Value *DeltaValue = BinaryOperator::Create(Instruction::Mul, StepValue, ConstUR, "", InsertBefore);

    Value *ConstIA = ConstantInt::get(StepValue->getType(), ItersAhead);
    Value *Mul = BinaryOperator::Create(Instruction::Mul, ConstIA, DeltaValue, "", InsertBefore);
    return BinaryOperator::Create(Instruction::Sub, &IVFinalVal, Mul, "", InsertBefore);
}

// LoopUnrollResult PrefetchLoopTransform::unrollLoop(Loop *L, unsigned UnrollTimes, 
//         Loop ** RemainderLoop) {
//     bool PreserveLCSSA = true;
//     UnrollLoopOptions ULO; 
//     ULO.Count = UnrollTimes;  
//     ULO.Force = false;
//     ULO.Runtime = false;
//     ULO.AllowExpensiveTripCount = true;
//     ULO.UnrollRemainder = false;
//     ULO.ForgetAllSCEV = false;
//     LoopUnrollResult res = UnrollLoop(L, ULO, &LI, &SE, &DT, &AC, &TI, &TTI, 
//         &ORE, PreserveLCSSA, RemainderLoop);
    
//     bool SimplifyIVs = false;
//     simplifyLoopAfterUnroll(L, SimplifyIVs, &LI, &SE, &DT, &AC, &TTI); 
    
    
//     return res;
// }

Loop *TestPassImpl::splitLoop(Loop *L, unsigned ItersAhead, unsigned UnrollTimes) {
    // if (!canTransform(L)) {
    //     errs() << "can't split loop \n";
    //     return nullptr;
    // }
    // errs() << "  pass canTransform\n";
    
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
    Instruction *Split1 = computeSplitPoint(L, ItersAhead, UnrollTimes,
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
    Instruction *Split2 = computeSplitPoint(EpilogL, ItersAhead, UnrollTimes, 
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

BasicBlock *TestPassImpl::peelFirstIter(Loop *L, BasicBlock *Exit) {
    // if (!canTransform(L)) {
    //     errs() << "can't peel loop \n";
    //     return nullptr;
    // }

    // LoopBlocksDFS LoopBlocks(L);
    // LoopBlocks.perform(&LI);

    BasicBlock *Header = L->getHeader();
    BasicBlock *PreHeader = L->getLoopPreheader();
    BasicBlock *Latch = L->getLoopLatch();
    // BasicBlock *Exit = L->getUniqueExitBlock();

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

    errs() << "-- peelFirstIter begin: \nInserTop:";
    InsertTop->print(errs());
    errs() << "NewPreHeader:\n";
    NewPreHeader->print(errs());
    errs() << "InserBot: \n";
    InsertBot->print(errs());
    // errs() << "<InsertTop>: " << InsertTop->getName() << "<InsertBot>: " << InsertBot->getName() 
    // << "NewPreHeader" << NewPreHeader->getName() << "\n\n";

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

    cloneLoopBody(L, InsertTop, InsertBot, Exit, NewBlocks, VMap, LVMap, &DT, &LI);
    // Remap to use values from the current iteration instead of the
    // previous one.

    errs() << "-- peelFirstIter after clone loop body: \nInserTop:";
    InsertTop->print(errs());
    errs() << "NewPreHeader:\n";
    NewPreHeader->print(errs());
    errs() << "InserBot: \n";
    InsertBot->print(errs());

    remapInstructionsInBlocks(NewBlocks, VMap);

    errs() << "-- peelFirstIter after remapInstructions: \nInserTop:";
    InsertTop->print(errs());
    errs() << "NewPreHeader:\n";
    NewPreHeader->print(errs());
    errs() << "InserBot: \n";
    InsertBot->print(errs());

    // Latches of the cloned loops dominate over the loop exit, so idom of the
    // latter is the first cloned loop body, as original PreHeader dominates
    // the original loop body.
    // DT.changeImmediateDominator(Exit, cast<BasicBlock>(LVMap[Latch]));

    updateBranchWeights(InsertBot, cast<BranchInst>(VMap[LatchBR]), ExitWeight);

    errs() << "-- peelFirstIter after updatebranchweights: \nInserTop:";
    InsertTop->print(errs());
    errs() << "NewPreHeader:\n";
    NewPreHeader->print(errs());
    errs() << "InserBot: \n";
    InsertBot->print(errs());
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

    errs() << "-- peelFirstIter after splice and phinode adjustment: \nInserTop: ";
    InsertTop->print(errs());
    errs() << "NewPreHeader:\n";
    NewPreHeader->print(errs());
    errs() << "InserBot: \n";
    InsertBot->print(errs());

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
        // simplifyLoop(ParentLoop, &DT, &LI, &SE, &AC, &MSSAU, true);
    } else {
        // FIXME: Incrementally update loop-simplify
        // simplifyLoop(L, &DT, &LI, &SE, &AC, &MSSAU, true);
    }

    return InsertTop;
}

Loop *TestPassImpl::insertProlog(Loop *L, unsigned ItersAhead, PHINode **NewIndVar) {
    
    // if (!canTransform(L)) {
    //     errs() << "can't insert prolog loop \n";
    //     return nullptr;
    // }

    errs() << "  at least can transform Loop <" << L->getHeader()->getName() << "\n";

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
    errs() << "  at least Loop has preheader, header, and latch\n";
    
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
    // assert(DT.getNode(OldPH) && "Old preheader has null node !\n");
    // assert(DT.getNode(Header) && "Old preheader has null node !\n");
    BasicBlock *InsertTop = SplitEdge(OldPH, Header);
    DT.addNewBlock(InsertTop, OldPH);
    assert(DT.getNode(InsertTop) && "insertTop is null node pointers after addNewBlock!");
    DT.changeImmediateDominator(DT.getNode(Header), DT.getNode(InsertTop));
    BasicBlock *InsertBot = SplitBlock(InsertTop, InsertTop->getTerminator(), &DT, &LI);
    assert(DT.getNode(InsertBot) && "insertBot is null node pointers before prependloop!");

    errs() << "before prepend: InsertTop = " << InsertTop->getName() << ", InsertBot = " << InsertBot->getName() << "\n";
    errs() << "InsertTop terminator = ";
    InsertTop->getTerminator()->print(errs());
    errs() << ", InsertBot terminator = ";
    InsertBot->getTerminator()->print(errs());
    errs() << "\n";
    Loop *PrologL = prependLoop(L, InsertTop, InsertBot, &IVInitialVal, &IVFinalVal, 
                                StepValue, StepOpcode, LatchCmpInst, &LI, &SE, &DT,
                                &AC, &MSSAU, NewIndVar);
    // errs() << "  at least prependLoop is a success\n";

    return PrologL; 
}

/// HELPER: Prefetch insert helper
bool TestPassImpl::insertPrefetch(BasicBlock *B, unsigned ItersAhead, 
                                SmallVector<AccessPattern *> AccessPatterns) {
    // preliminary information
    Module *M = B->getModule();
    LLVMContext &context = M->getContext();

    IntegerType* I32 = Type::getInt32Ty(context);
    Type *I8Ptr = Type::getInt8PtrTy(context, 0/*PtrAddrSpace*/);

    BasicBlock::iterator InsertPt = B->begin();
    while (isa<PHINode>(*InsertPt)) {
        ++InsertPt;
    }
    // 
    SCEVExpander SCEVE(SE, M->getDataLayout(), "prefaddr");
    // for each access pattern, conduct insertion
    unsigned NumPrefetches = 0;
    for (auto *AP : AccessPatterns) {
        const SCEVAddRecExpr *PtrAR = AP->getPtrAR();
        const SCEV *NextLSCEV = SE.getAddExpr(PtrAR, 
                                SE.getMulExpr(
                                    SE.getConstant(PtrAR->getType(), ItersAhead),
                                    PtrAR->getStepRecurrence(SE)));
        if (!isSafeToExpand(NextLSCEV, SE)) {
            errs() << "not safe to Expand!\n";
            continue;
        }

        Value *PrefPtrValue = SCEVE.expandCodeFor(NextLSCEV, I8Ptr, &*InsertPt);

        IRBuilder<> Builder(&*InsertPt);
        Function *PrefetchFunc = Intrinsic::getDeclaration(M, Intrinsic::prefetch, 
                                                        PrefPtrValue->getType());
        Builder.CreateCall(
            PrefetchFunc, { 
                PrefPtrValue, 
                ConstantInt::get(I32, AP->isWrite()),
                ConstantInt::get(I32, 1),  // L3 cache
                ConstantInt::get(I32, 1)   // overlap with second argument
            }
        );

        errs() << "-- insert spatial prefetch at: \n";
        PrefPtrValue->print(errs());

        NumPrefetches++;
    }

    // if (Loop *L = LI.getLoopFor(B)) {
    //     errs() << "in loop " << L->getHeader()->getName() << ", ";
    //     SmallVector<WeakTrackingVH, 16> DeadInsts;
    //     bool Changed = SCEVE.replaceCongruentIVs(L, &DT, DeadInsts);
    //     if (Changed) {
    //         errs() << "cleaned up introduced indvar after scevExpander";
    //     }
    //     // errs() << "\n";
    //     // Remove dead instructions
    //     for (auto &VH : DeadInsts) {
    //         if (Instruction *Inst = dyn_cast_or_null<Instruction>(VH)) {
    //             Inst->eraseFromParent();
    //         }
    //     }
    // }
    ///////////////////////////////
    errs() << "insertPrefetch: " << NumPrefetches << "\n";
    return NumPrefetches > 0;
}

bool TestPassImpl::insertPrefetch(BasicBlock *B, unsigned ItersAhead, vector<PHINode *> &IVs,
                                 PHINode *oldIV, Value *newValue, SmallVector<AccessPattern *> AccessPatterns) {
    errs() << "  prolog prefetch insert is called\n";
    // preliminary information
    // BasicBlock *H = L->getHeader();
    Module *M = B->getModule();
    LLVMContext &context = M->getContext();

    IntegerType* I32 = Type::getInt32Ty(context);
    Type *I8Ptr = Type::getInt8PtrTy(context, 0/*PtrAddrSpace*/);

    BasicBlock::iterator InsertPt = B->begin();
    while (isa<PHINode>(*InsertPt)) {
        ++InsertPt;
    }

    errs() << "  prolog prefetch insert successfully computed insert point\n";
    // for each access pattern, conduct insertion
    unsigned NumPrefetches = 0;
    for (auto *AP : AccessPatterns) {
        Value *PrefPtrValue = AP->expandCodeFor(IVs, ItersAhead, oldIV, newValue, &LI, &SE, &*InsertPt);

        IRBuilder<> Builder(&*InsertPt);
        Function *PrefetchFunc = Intrinsic::getDeclaration(M, Intrinsic::prefetch, 
                                                        PrefPtrValue->getType());
        Builder.CreateCall(
            PrefetchFunc, { 
                PrefPtrValue, 
                ConstantInt::get(I32, AP->isWrite()),
                ConstantInt::get(I32, 1),  // L3 cache
                ConstantInt::get(I32, 1)   // overlap with second argument
            }
        );

        NumPrefetches++;
    }

    ///////////////////////////////
    errs() << "insertPrefetch: " << NumPrefetches << "\n";
    return NumPrefetches > 0;
}

//// RUN: main implementation
bool TestPassImpl::run() {
    PrefetchLoopAnalysis Analysis(LI, SE, DT, AC, TI, TTI, ORE, MSSAU);
    Analysis.run();

    /// DEBUG:  //////////////////////////
    if (debug) {
        errs() << "------------ PrefetchLoopAnalysis results: -------------------\n";
        for (Loop *Outermost : LI) {
            for (Loop *L : post_order(Outermost)) {
                if (!Analysis.isValidLoop(L)) {
                    continue;
                }
                errs() << "-- Loop " << L->getHeader()->getName() << ": \n";
                errs() << "Use memory (in bytes): " << Analysis.getLoopMemUse(L) << "\n";
                errs() << "Has body of length: " << Analysis.getLoopLength(L) << "\n";

                errs() << "discovered access patterns: \n";
                for (auto *AP : Analysis.getAccessPattern(L)) {
                    AP->printSubscriptExpr();
                    errs() << "\n";
                }
                errs() << "\n\n";
                // errs() << "Has the following indvars in order: ";
                // LoopNestInfo LNIF = Analysis.getLNIF(L);
                // for (PHINode *iv : LNIF.IVs) {
                //     errs() << getShortValueName(iv) << ", ";
                // }
                // errs() << "\n";

                // errs() << "Has the Loops\n";
                // for (auto it : LNIF.LID) {
                //     Loop *L = it.first;
                //     unsigned idx = it.second;
                //     errs() << L->getHeader()->getName() << ": " << idx << "\n";
                // }
                // errs() << "\n";

                errs() << "recommend to prefetch " << Analysis.computePrefetchItersAhead(L) << " iters ahead\n"; 

                errs() << "The following access patterns are considered spatial: \n";
                
                for (auto *AP : Analysis.getSpatialAccess(L)) {
                    AP->printSubscriptExpr();
                    errs() << "\n";
                }
                errs() << "The following access patterns are considered temporal: \n";
                for (auto *AP : Analysis.getTemporalAccess(L)) {
                    AP->printSubscriptExpr();
                    errs() << "\n";
                }
            }
        }
        errs() << "---------------------- : -------------------\n";
    }

    //// main loop to running loop transformation: remember to create copies of loops as LI will be updated 
    std::vector<Loop *> OriginalLoops;
    unordered_map<Loop *, PHINode *> OriginalIVs;
    unordered_map<Loop *, BasicBlock *> OrigExits;
    errs() << "Original Loops: \n";
    for (Loop *Outermost : LI) {
        for (Loop *L : post_order(Outermost)) {
            errs() << "- preheader: " << L->getLoopPreheader()->getName() << "\n";
            L->print(errs());
            OriginalLoops.push_back(L);
            if (PHINode *IV = L->getInductionVariable(SE)) 
                OriginalIVs.insert(make_pair(L, IV));
            if (BasicBlock *Exit = L->getUniqueExitBlock()) 
                OrigExits.insert(make_pair(L, Exit));
        }
    }
    errs() << "\n";


    bool MadeChange = false;
    for (Loop *L : OriginalLoops) {
        errs() << ">> runOnLoop(" << L->getHeader()->getName() << "): \n";
        MadeChange |= runOnLoop(L, Analysis, OriginalIVs, OrigExits); // OriginalIVs
    }

    return MadeChange;
}   

bool TestPassImpl::runOnLoop(Loop *L, PrefetchLoopAnalysis &Analysis, 
                            unordered_map<Loop *, PHINode *>& OrigIVs,
                            unordered_map<Loop *, BasicBlock *>& OrigExits) {// std::unordered_map<Loop *, PHINode *>& OriginalIVs
    if (!Analysis.isValidLoop(L)) {
        return false;
    }
    // prepare original induction variable
    PHINode *OrigIV = OrigIVs[L];
    assert(OrigIV && "L doesn't have induction variable");
    /// DEBUG: 

    vector<PHINode *> IVs = Analysis.getIVs(L);
    // prepare prefetch iters ahead
    unsigned ItersAhead = Analysis.computePrefetchItersAhead(L);
    // for each spatial & temporal locality in the Loop perform transformation and insert prefetch
    bool MadeChange = false;
    if (Analysis.isTemporal(L)) {
        // temporal accesses
        L->print(errs());
        errs() << "\n";

        errs() << "there is Temporal locality!\n";
        SmallVector<AccessPattern *> TAP = Analysis.getTemporalAccess(L);
        // temporal loop transformation: peel off the first iteration 
        // BasicBlock *InsertTop = peelFirstIter(L, OrigExits[L]);
        // assert(InsertTop && "Loop Peeling failed");
        // errs() << "peel off starting at InsertTop: <" << InsertTop->getName() << ">\n";

        // Optional<Loop::LoopBounds> LB = L->getBounds(SE);
        // assert(LB.hasValue() && "LB should have value in temporal prefetch case");
        // Value &InitIV = LB->getInitialIVValue();
        // // insert at insertTop
        // MadeChange |= insertPrefetch(L->getLoopPreheader(), 0, IVs, OrigIV, &InitIV, TAP);
    } else {
        if (Analysis.isSpatial(L)) {

            errs() << "there is spatial locality!\n";
            // spatial accesses 
            SmallVector<AccessPattern *> SAP = Analysis.getSpatialAccess(L);

            // spatial loop transformation 
            // errs() << "proceed to split loop\n";
            // Loop *Epilog = splitLoop(L, ItersAhead, 1); // no unroll
            // assert(Epilog && "cannot split loop!");

            // insert prefetch at original loop
            errs() << "proceed to insert Prefetch at original loop: <" << L->getHeader()->getName() << ">\n";
            MadeChange |= insertPrefetch(L->getHeader(), ItersAhead, SAP);
            // MadeChange |= insertPrefetch(L->getHeader(), ItersAhead, IVs, OrigIV, OrigIV, SAP);

            // errs() << "proceed to insert loop prolog \n";
            // PHINode *PrologIV = nullptr;
            // Loop *Prolog = insertProlog(L, ItersAhead, &PrologIV);
            // assert(Prolog && "cannot insert empty prolog loop for spatial locality transformation!");
            // assert(PrologIV && "cannot find indvar for prolog loop!");
            // PHINode *PrologIVTrue = Prolog->getInductionVariable(SE);
            // if (!PrologIVTrue) {
            //     errs() << "still can't find prolog indvar\n";
            // }
            // // insert at each newly generated loops
            // errs() << "proceed to insert Prefetch at prolog:\n";
            // MadeChange |= insertPrefetch(Prolog, ItersAhead, IVs, OrigIV, PrologIV, SAP);
            // errs() << "Prolog: ";
            // Prolog->print(errs());
            // errs() << "L: ";
            // L->print(errs());
        }
    }
    errs() << "runOnLoop successly completed !\n\n";
    return MadeChange;
}