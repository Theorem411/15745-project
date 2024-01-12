#include "prefetch.h"
/// PUBLIC: private functionalities of PrefetchPassImpl pass /////
bool PrefetchPassImpl::run() {
    bool MadeChange = false;

    if (TI.isSerial()) {
        errs() << "TI is serial!\n";
        return false;
    }

    for (Loop *Outermost : LI) {
        for (Loop *L : post_order(Outermost)) {
            MadeChange |= runOnLoop(L);
        }
    }
    return MadeChange;
}

bool PrefetchPassImpl::runOnLoop(Loop *L) {
    /** collect tapir loop *****/
    if (Task *T = getTaskIfTapirLoop(L)) {
        // T->print(outs());
        createTapirLoop(L, T);
    }

    /** create locality info about this loop */
    Locality *CL = getCacheLocalityLoop(L);
    if (!CL->isValid()) {
        errs() << "Loop " << L->getPreheader()->getName() << " doesn't have array locality... \n";
        return false;
    }

    /** Perform loop unrolling if L contain spatial locality */
    unsigned PrefetchIterAhead = computePrefetchIterAhead(L);
    
    if (vector<GroupAccess *> PfInsertInsts = CL->getSpatialLocality()) {
        /// TODO: perfom unrolling

    }

    /** Perform loop peeling if L contain temporal locality */
    if (vector<GroupAccess *> PfInsertInsts = CL->getTemporalLocality()) {    
        // perfom loop peeling and insert Insertion top block 
        BasicBlock *PfInsertBB = peelLoop(L, 1, &LI, &SE, &DT, &AC, &MSSAU, true);
        if (!PfInsertBB) {
            return false;
        }
        // insert pf intrinsic 
        for (GroupAccess *Pf : PfInsertInsts) {
            IRBuilder<> Builder(PfInsertBB->getTerminator());
            Module *M = F.getParent();
            Function *PrefetchFunc = Intrinsic::getDeclaration(M, Intrinsic::prefetch, Pf->getType());
            Type *I32 = Type::getInt32Ty(BB->getContext());
            /** prefetch intrinsic call parameters: 
                - 
                - read/write prefetch mode
                - prefetch to cache level
                - has call
            */
            Builder.CreateCall(
                PrefetchFunc, 
                {
                    Pf->leadAccess(), 
                    ConstantInt::get(I32, Pf->isWrite()), 
                    ConstantInt::get(I32, 3), 
                    ConstantInt::get(I32, 1)
                }
            ); 
            
            // Take record of prefetch instances
            ++NumPrefetches;

            errs() << "Prefetched memory access " << Pf->leadAccess() << "\n";
        }
    }

    /*************************/
    return true;
}

TapirLoopInfo *PrefetchPassImpl::getTapirLoop(Loop *L) {
    if (!LoopToTapirLoop.count(L))
    return nullptr;
    return LoopToTapirLoop[L];
}
TapirLoopInfo *PrefetchPassImpl::getTapirLoop(Task *T) {
    if (!TaskToTapirLoop.count(T))
    return nullptr;
    return TaskToTapirLoop[T];
}
TapirLoopInfo *PrefetchPassImpl::getTapirLoop(const BasicBlock *B) {
    return getTapirLoop(LI.getLoopFor(B));
}

unsigned PrefetchPassImpl::computePrefetchIterAhead(Loop *L) {
    /// TODO: compute depth-first-search of loop body 
    return 1;
}

/// PRIVATE: PrefetchPassImpl private helper function //////////////
TapirLoopInfo *PrefetchPassImpl::createTapirLoop(Loop *L, Task *T) {
    TapirLoops.push_back(new TapirLoopInfo(L, T, ORE));
    TaskToTapirLoop[T] = TapirLoops.back();
    LoopToTapirLoop[L] = TapirLoops.back();
    return TapirLoops.back();
};

Task *PrefetchPassImpl::getTaskIfTapirLoop(const Loop *L) {
    errs() << "Analyzing loop: " << *L;
    // Loop must have a preheader.  LoopSimplify should guarantee that the loop
    // preheader is not terminated by a sync.
    const BasicBlock *Preheader = L->getLoopPreheader();
    if (!Preheader) {
        errs() << "Loop lacks a preheader.\n";
        return nullptr;
    } else if (!isa<BranchInst>(Preheader->getTerminator())) {
        errs() << "Loop preheader is not terminated by a branch.\n";
        return nullptr;
    }

    Task *T = llvm::getTaskIfTapirLoop(L, &TI);
    if (!T) {
        errs() << "Loop " << Preheader->getName() << " does not match structure of Tapir loop.\n";
        return nullptr;
    }
    
    errs() << "Loop " << Preheader->getName() << " match structure of Tapir loop!\n";
    return T;
}

Locality *PrefetchPassImpl::getCacheLocalityLoop(const Loop *L) {
    /// TODO: call Locality init
    Locality *CL = new Locality(L, DT, LI, TI, SE, AC, TTI, ORE);
    /** What does Locality do?
        for L, first compute the following things 
        - Trace back of L -> used for matrix dimension & indicing
        - Maintain a set of GroupAccess *
        - search through instruction in L
            - check for each existing GroupAccess, if no group, add new one
    */

    // record new CL to map LoopToLocality
    LoopToLocality[L] = CL;
    return CL;
}
