#include "PeelToyPass.h"

bool PeelingToyPassImpl::run() {
    bool MadeChange = false;
    /// DEBUG: Solved! runOnLoop will change LI, so take out original loops before iteration
    std::vector<Loop *> OriginalLoops;
    for (Loop *Outermost : LI) {
        OriginalLoops.push_back(Outermost);
    }

    for (Loop *Outermost : OriginalLoops) {
        for (Loop *L : post_order(Outermost)) {
            MadeChange |= runOnLoop(L);
        }
    }
    return MadeChange;
}   

bool PeelingToyPassImpl::runOnLoop(Loop *L) {
    // if (auto *Idv = L->getInductionVariable(SE)) {
    //     errs() << "Loop " << L->getLoopPreheader()->getName() << " has induction variable : ";
    //     Idv->print(errs());
    //     errs() << "\n";
    // }
    // return false;
    BasicBlock *OrigPreheader = L->getLoopPreheader();
    PHINode *OrigIndvar = L->getInductionVariable(SE);
    errs() << "runOnLoop(" << OrigPreheader->getName() << "): \n";

    bool PreserveLCSSA = true;
    UnrollLoopOptions ULO; 
    ULO.Count = 2;  
    ULO.Force = false;
    ULO.Runtime = true;
    ULO.AllowExpensiveTripCount = false;
    ULO.UnrollRemainder = false;
    ULO.ForgetAllSCEV = false;
    Loop *RemainderLoop = nullptr;
    LoopUnrollResult res = UnrollLoop(L, ULO, &LI, &SE, &DT, &AC, &TI, &TTI, &ORE, PreserveLCSSA, &RemainderLoop); // you should also interpret the results

    switch (res) {
        case LoopUnrollResult::Unmodified: {
            errs() << "Loop " << OrigPreheader->getName() << " is unmodified\n";
            return false;
        }
        case LoopUnrollResult::PartiallyUnrolled: {
            errs() << "Loop " << OrigPreheader->getName() << " is PartiallyUnrolled\n";
            break;
        }
        case LoopUnrollResult::FullyUnrolled: {
            errs() << "Loop " << OrigPreheader->getName() << " is fully unrolled\n";
            break;
        }
    }
    simplifyLoopAfterUnroll(L, true, &LI, &SE, &DT, &AC, &TTI); 

    errs() << "dumping the new loop: \n";
    L->dump();

    PHINode *NewIndvar = L->getInductionVariable(SE);
    errs() << "Original INDVAR: ";
    OrigIndvar->print(errs());
    errs() << "\nNew INDVAR:     ";
    NewIndvar->print(errs());
    errs() << "\n";
    if (RemainderLoop) {
        // when !RemainderLoop, accessing anything about it will result in segfault
        errs() << "with remainder loop: " << RemainderLoop->getHeader()->getName() << "\n";
        RemainderLoop->print(errs());
    }

    // BasicBlock *PfInsertPt = peelLoop(L, 1, &LI, &SE, &DT, &AC, &MSSAU, true);
    // if (!PfInsertPt) {
    //     errs() << "Peeling after unrolling failed!... \n";
    //     return false;
    // }
    // errs() << "------------------------\n";
    errs() << "\n";
    return true;
}