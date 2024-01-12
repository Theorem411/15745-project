#include "PrefetchLoopAnalysis.h"

// LoopNestInfo functions

// PrefetchLoopAnalysis functions
bool PrefetchLoopAnalysis::run() {
    bool MadeChange = false;
    // preliminary run to collect loop-specific info
    for (Loop *Outermost : LI) {
        for (Loop *L : post_order(Outermost)) {
            MadeChange |= runOnLoop1(L);
        }
    }
/// DEBUG: //////////////////////////
    // errs() << "------- runOnLoop1 results: --------\n";
    // for (Loop *Outermost : LI) {
    //     for (Loop *L : post_order(Outermost)) {
    //         BasicBlock *Header = L->getHeader();
    //         if (!isValidLoop(L)) {
    //             errs() << "Loop " << Header->getName() << " is not valid\n\n";
    //             continue;
    //         }

    //         LoopNestInfo LNIF = LToLNIF.lookup(L);
            
    //         errs() << "IN Loop " << Header->getName() << ", found access patterns: \n";
    //         SmallVector<AccessPattern *> APs = getAccessPattern(L);
    //         for (auto *AP : APs) {
    //             AP->printSubscriptExpr();
    //             errs() << "\n";
    //             AP->printAccessMatrix();
    //             AP->printReuseMatrix();
    //             errs() << "\n";
    //         }
    //         errs() << "Use memory (in bytes): " << LToMemUse[L] << "\n";
    //         errs() << "Has body of length: " << LToLength[L] << "\n";

    //         errs() << "Has the following indvars in order: ";
    //         for (PHINode *iv : LNIF.IVs) {
    //             errs() << getShortValueName(iv) << ", ";
    //         }
    //         errs() << "\n";

    //         errs() << "Has the Loops\n";
    //         for (auto it : LNIF.LID) {
    //             Loop *L = it.first;
    //             unsigned idx = it.second;
    //             errs() << L->getHeader()->getName() << ": " << idx << "\n";
    //         }
            
    //         errs() << "\n\n";
    //     }
    // }
////////////////////////////////////

    // second run to get more global information
    for (Loop *Outermost : LI) {
        for (Loop *L : post_order(Outermost)) {
            runOnLoop2(L);
        }
    }

/// DEBUG:  //////////////////////////
    // errs() << "------- runOnLoop2 results: --------\n";
    // for (Loop *Outermost : LI) {
    //     for (Loop *L : post_order(Outermost)) {
    //         if (!isValidLoop(L)) {
    //             continue;
    //         }
    //         errs() << "Loop " << L->getHeader()->getName() << ": \n";
    //         errs() << "recommend to prefetch " << computePrefetchItersAhead(L) << " iters ahead\n"; 

    //         errs() << "The following access patterns are considered temporal: \n";
            
    //         for (auto *AP : LToTLoc[L]) {
    //             AP->printSubscriptExpr();
    //             errs() << "\n";
    //         }
    //         errs() << "The following access patterns are considered spatial: \n";
    //         for (auto *AP : LToSLoc[L]) {
    //             AP->printSubscriptExpr();
    //             errs() << "\n";
    //         }
    //     }
    // }
////////////////////////////////////
    return MadeChange;
}

bool PrefetchLoopAnalysis::runOnLoop1(Loop *L) {
    Optional<LoopNestInfo> LNIF = LoopNestInfo::getLoopNestInfo(L, &LI, &SE);
    if (!LNIF.hasValue()) {
        return false;
    }
    LToLNIF.insert(make_pair(L, LNIF.getValue()));
    // collect access patterns by traversing L
    SmallVector<AccessPattern *, 8> AccessPatterns;
    for (const auto BB : L->blocks()) {
        if (LI.getLoopFor(BB) != L) {
            // only care about accesses in the bottom of the loop nests
            continue; 
        }
        for (auto &I : *BB) {
            Value *PtrValue;
            Instruction *MemI;

            if (LoadInst *LMemI = dyn_cast<LoadInst>(&I)) {
                MemI = LMemI;
                PtrValue = LMemI->getPointerOperand();
            } else if (StoreInst *SMemI = dyn_cast<StoreInst>(&I)) {
                MemI = SMemI;
                PtrValue = SMemI->getPointerOperand();
            } else continue;

            if (!isa<GetElementPtrInst>(PtrValue)) {
                continue;
            }
            if (L->isLoopInvariant(PtrValue))
                continue;
            
            GetElementPtrInst *PtrGEP = dyn_cast<GetElementPtrInst>(PtrValue);
            IV_t IVs = LNIF->IVs;
            AccessPattern * NewAP = AccessPattern::getAccessPattern(MemI, PtrGEP, &SE, &LI, IVs);
            if (!NewAP) {
                continue;
            }
            bool GroupAccess = false;
            for (auto *AP : AccessPatterns) {
                if (AP->update(NewAP, SE)) {
                    GroupAccess = true;
                    break;
                }
            }
            if (!GroupAccess) {
                AccessPatterns.push_back(NewAP);
            }
        }
    }
    // record AccessPattern
    LToAP.insert(make_pair(L, AccessPatterns));

/// DEBUG: //////
    // for (auto *AP : AccessPatterns) {
    //     errs() << "discovered group access pattern: "
    //     AP->printSubscriptExpr();
    //     errs() << "\n";
    // }
/////////////////

    // update memory usage 
    unsigned memUse = 0;
    for (auto *AP : AccessPatterns) {
        memUse += AP->getBasePtrSize();
    }
    for (Loop *SubL : L->getSubLoops()) {
        unsigned tripCount = SE.getSmallConstantTripCount(L);
        if (!tripCount) {
            tripCount = 1;
        }
        memUse += LToMemUse[L] * tripCount;
    }
    LToMemUse[L] = memUse;

    // determine if L is localized
    if (memUse <= getL1CacheSize()) {
        LocalizedLoops.insert(L);
    }
    // compute access matrix & reuse 
    for (auto *AP: AccessPatterns) {
        AP->computeAccessMatrix(LNIF->LT, LNIF->LID, LNIF->IV2L);
    }

    // compute loop body to compute prefetch iters ahead
    auto CostBook = getBlockCost(L, &AC, &TTI);
    unsigned length = loopBodyCost(L, CostBook);
    LToLength.insert({L, length});

    // init LToTLoc & LToSLoc
    LToTLoc.insert(make_pair(L, SmallVector<AccessPattern *, 8>()));
    LToSLoc.insert(make_pair(L, SmallVector<AccessPattern *, 8>()));
    
    return true;
}

bool PrefetchLoopAnalysis::runOnLoop2(Loop *L) {
    // errs() << "  runOnLoop2(" << L->getHeader()->getName() << "): \n";
    if (LToLNIF.find(L) == LToLNIF.end()) {
        return false;
    }
    LoopNestInfo LNIF = LToLNIF[L];
    assert(LNIF.L == L && "LoopNestInfo is constructed incorrectly!");
    // for each Loop Trace, determine the inner loops that is localized
    vector<Loop *> LocLoops;
    for (Loop *L : LNIF.LT) {
        if (LocalizedLoops.find(L) != LocalizedLoops.end()) {
            // errs() << "localize loop " << L->getHeader()->getName() << "\n";
            LocLoops.push_back(L);
        }
    }
    // errs() << "-- finish finding localized loop trace\n";
    // compute AP's locality info
    assert(LToAP.find(L) != LToAP.end() && "L should have been recored in LToAP during first run!");
    SmallVector<AccessPattern *> AccessPatterns = LToAP[L];
    
    for (auto *AP : AccessPatterns) {
        AP->computeLocality(LocLoops, LNIF.LT, LNIF.LID);
        for (Loop *LocLoop : LocLoops) {
            // 
            if (AP->isSpatial(LocLoop, LNIF.LT, LNIF.LID)) {
                // errs() << "spatial: "; 
                // AP->printSubscriptExpr();
                // errs() << "\n";
                LToSLoc[LocLoop].push_back(AP);
            }
            if (AP->isTemporal(LocLoop, LNIF.LT, LNIF.LID)) {
                // errs() << "temporal: "; 
                // AP->printSubscriptExpr();
                // errs() << "\n";

                LToTLoc[LocLoop].push_back(AP);
            }
        }
    }
    // errs() << "SAP after: \n";
    // for (auto *AP : SAP) {
    //     AP->printSubscriptExpr();
    //     errs() << "\n";
    // }
    // errs() << "TAP after: \n";
    // for (auto *AP : TAP) {
    //     AP->printSubscriptExpr();
    //     errs() << "\n";
    // }

    return true;
}

bool PrefetchLoopAnalysis::isSpatial(Loop *L) {
    if (LToSLoc.find(L) == LToSLoc.end()) 
        return false;
    auto SLoc = LToSLoc[L];
    return SLoc.size() != 0;
}   

bool PrefetchLoopAnalysis::isTemporal(Loop *L) {
    if (LToTLoc.find(L) == LToTLoc.end()) 
        return false;
    auto TLoc = LToTLoc[L];
    return TLoc.size() != 0;
}   

unsigned PrefetchLoopAnalysis::computePrefetchItersAhead(Loop *L) {
    assert(LToLength.find(L) != LToLength.end() && "can't find record of L when computing prefetch iter");
    unsigned len = LToLength[L];
    if (len == 0)
        len = 1;
    return std::min(120/len, (unsigned)5);
}

SmallVector<AccessPattern *> PrefetchLoopAnalysis::getAccessPattern(Loop *L) {
    assert(isValidLoop(L) && "getAccessPattern cannot be called on invalid loop!");
    return LToAP.lookup(L);
}

SmallVector<AccessPattern *> PrefetchLoopAnalysis::getSpatialAccess(Loop *L) {
    assert(isValidLoop(L) && "getSpatialAccess cannot be called on invalid loop!");
    return LToSLoc.lookup(L);
}

SmallVector<AccessPattern *> PrefetchLoopAnalysis::getTemporalAccess(Loop *L) {
    assert(isValidLoop(L) && "getTemporalAccess cannot be called on invalid loop!");
    return LToTLoc.lookup(L);
}

const LoopNestInfo &PrefetchLoopAnalysis::getLNIF(Loop *L) {
    assert(isValidLoop(L) && "getLNIF called on invalid loops");
    const LoopNestInfo& LNIF = LToLNIF.lookup(L);
    return LNIF;
}

unsigned PrefetchLoopAnalysis::getLoopMemUse(Loop *L) {
    assert(isValidLoop(L) && "getLoopMemUse cannot be called on invalid loops\n");
    return LToMemUse.lookup(L);
}

unsigned PrefetchLoopAnalysis::getLoopLength(Loop *L) {
    assert(isValidLoop(L) && "getLoopLength cannot be called on invalid loops\n");
    return LToLength.lookup(L);
}