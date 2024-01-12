#include "cache-locality.h"

/// PUBLIC: ///////////////////////////
bool Locality::isEmpty() {
    return !isSpatial() && !isTemporal();
}

vector<GroupAccess *> Locality::getSpatialLocality() {

}

bool Locality::isSpatial() {

}

vector<GroupAccess *> Locality::getTemporalLocality() {

}

bool Locality::isTemporal() {

}

/// PRIVATE: /////////////////////////
// Copied from LoopInterchange.cpp
static PHINode *getInductionVariable(Loop *L) {
    PHINode *InnerIndexVar = L->getCanonicalInductionVariable();
    if (InnerIndexVar)
      return InnerIndexVar;
    if (L->getLoopLatch() == nullptr || L->getLoopPredecessor() == nullptr)
      return nullptr;
    for (BasicBlock::iterator I = L->getHeader()->begin(); isa<PHINode>(I); ++I) {
        PHINode *PhiVar = cast<PHINode>(I);
        Type *PhiTy = PhiVar->getType();
        if (!PhiTy->isIntegerTy() && !PhiTy->isFloatingPointTy() &&
            !PhiTy->isPointerTy()) {
            outs() << "Type not interger, float, or pointer\n";
            return nullptr;
        }

        auto scevExpr = SE.getSCEV(PhiVar);
        assert(scevExpr);
        //scevExpr->dump();
        const SCEVAddRecExpr *AddRec =  dyn_cast<SCEVAddRecExpr>(scevExpr);
        if (!AddRec || !AddRec->isAffine()) {
            if(!AddRec)  {
                outs() << "AddRec == null\n";
                continue;
            }

            if(!AddRec->isAffine()) {
                outs() << "AddRec not affine\n";
                continue;
            }
        }
        const SCEV *Step = AddRec->getStepRecurrence(SE);
        if (!isa<SCEVConstant>(Step)) {
            outs() << "Not constant\n";
            continue;
        }

        // PhiVar must be used in latchcmpinst
        ICmpInst *LatchCmp = L->getLatchCmpInst();
        if (PhiVar != LatchCmp->getOperand(0) && PhiVar != LatchCmp->getOperand(1)) {
            outs() << "Not used in loop guard";
            continue;
        }
        // Found the induction variable.
        // FIXME: Handle loops with more than one induction variable. Note that,
        // currently, legality makes sure we have only one induction variable.
        return PhiVar;
    }
    return nullptr;
}

// find all  
void Locality::initGroupAccesses() {
    for (auto *B : L->blocks()) {
        for (auto &I : *B) {
            Value *PtrValue;
            Instruction *MemI;

            if (LoadInst *LMemI = dyn_cast<LoadInst>(&I)) {
                MemI = LMemI;
                PtrValue = LMemI->getPointerOperand();
            } else if (StoreInst *SMemI = dyn_cast<StoreInst>(&I)) {
                MemI = SMemI;
                PtrValue = SMemI->getPointerOperand();
            } else continue;

            AccessPattern *AP = parseAccessPattern(MemI, PtrValue);
            if (!AP) continue;
            
            bool newMember = true;
            for (auto *GA : GroupAccesses) {
                if (GA->addMember(MemI, PtrValue)) {
                    newMember = false;
                    break;
                }
            }
            if (newMember) {
                GroupAccess *GA = new GroupAccess(MemI, PtrValue, L, LoopTrace, LoopIndex, LoopToIndvar, IndvarToLoop); 
                GroupAccesses.push_back(GA);
            }
        }
    }
}

// return true if all loop in the trace has primary induction variables
bool Locality::initLoopRecursive(Loop *L) {
    if (!L) return true;

    PHINode *indvar = getInductionVariable(L);
    if (!indvar) {
        return false;
    }

    if (isLocalizedLoop(L)) {
        LocalizedLoops.push_back(L);
    }

    // add to LoopTrace & LoopIndex
    LoopIndex.insert({L, LoopTrace.size()});
    LoopTrace.push_back(L);

    // add to LoopToIndvar
    LoopToIndvar.insert({L, indvar});
    IndvarToLoop.insert({indvar, L});

    Loop *LNext = L->getParentLoop();
    return initLoopRecursive(LNext);
}

void Locality::InitLoop() {
    if (!L) {
        valid = false;
        return;
    }
    bool initOk = initLoopRecursive(L);
    valid &= initOk;
}

bool Locality::isLocalizedLoop(Loop *TargetL) {
    for (Loop *L : post_order(TargetL)) {
        /// TODO:
    }
}


// void CacheLocality::LoopTraceInitRecursive(Loop *L, std::vector<Loop *> &Trace) {
//     if (!L) {
//         throw std::runtime_error("encounter null Loop pointer when doing depth-first traversal using LoopInfo!");
//     }
//     // put current loop on the stack
//     Trace.push_back(L);

//     std::vector<Loop *> SubLoops = L->getSubLoops();
//     // reach inner most loop; make a fresh record of Trace and store in LoopTrace
//     if (SubLoops.size() == 0) {
//         std::vector<Loop *> TraceRecord(Trace); // unrelated copy
//         if (LoopTrace.find(L) != LoopTrace.end()) {
//             throw std::runtime_error("encounter the same loop twice when doing loop tree traversal!");
//         }
//         LoopTrace[L] = TraceRecord;
//         return;
//     }
//     // depth-first search down the loop tree
//     for (Loop *SubL : SubLoops) {
//         LoopTraceInitRecursive(SubL, Trace);
//     }
// }

// void CacheLocality::LoopTraceInit() {
//     for (Loop *Outermost : LI) {
//         std::vector<Loop *> Trace;
//         LoopTraceInitRecursive(Outermost, Trace);
//     }
// }

// // Indvar and loop mapping init 
// void CacheLocality::IndvarLoopInitRecursive(Loop *L) {
//     if (PHINode *indvar = getInductionVariableFromOptimizableLoop(L)) {
//         ////////// DEBUG: //////////////
//         LLVM_DEBUG(dbgs() << "Loop " << L->getHeader()->getName() << " has induction variable " << getShortValueName(indvar) << "!\n");
//         ////////////////////////////////
//         if (Indvar2Loop.find(indvar) != Indvar2Loop.end()) {
//             std::ostringstream oss;
//             oss << getShortValueName(indvar) << " qualifies as induction variable for different loops!";
//             throw std::runtime_error(oss.str());
//         }
//         Indvar2Loop.insert({indvar, L});
//         Loop2Indvar.insert({L, indvar});

//     } else {
//         LLVM_DEBUG(dbgs() << "Loop " << L->getHeader()->getName() << " doesn't have an induction variable or is not optimizable!\n");
//     }

//     for (Loop *SubL : LoopNests[L]) {
//         IndvarLoopInitRecursive(SubL);
//     }
// }

// void CacheLocality::IndvarLoopInit() {
//     for (Loop *Outermost : LI) {
//         IndvarLoopInitRecursive(Outermost);
//     }
// }

// void CacheLocality::ArrayAccessSetInit() {
//     /* For each load/store instruction:
//     *    init ArrayAccess(Ty, I, ParentLoop, SE)
//     *    run checkLoopNestOptimizable()
//     *   run parseSubscriptExpression()
//     *   if successfully parsed, add to ArrayAccessSet
//     */

//     for (auto I = inst_begin(F); I != inst_end(F); ++I) {
//         ArrayAccessTy AATy;
//         Value *ptrOperand;
//         if (auto *loadI = dyn_cast<LoadInst>(&*I)) {
//             ptrOperand = loadI->getPointerOperand();
//             if (!ptrOperand || !isa<GetElementPtrInst>(ptrOperand)) {
//                 continue;
//             }
//             AATy = ALoad;
//         } else if (auto *storeI = dyn_cast<StoreInst>(&*I)) {
//             ptrOperand = storeI->getPointerOperand();
//             if (!ptrOperand || !isa<GetElementPtrInst>(ptrOperand)) {
//                 continue;
//             }
//             AATy = AStore;
//         } else {
//             continue;
//         }
//         Loop *parentLoop = LI.getLoopFor((&*I)->getParent());
//         if (!parentLoop) {
//             // Load/Store instruction must be inside a loop
//             continue;
//         }
        
//         //// DEBUG: //////////////////////////
//         LLVM_DEBUG(dbgs() << "* attempt ArrayAccess analysis in loop " << parentLoop->getHeader()->getName() << " on instruction : \n");
//         I->dump();
//         //////////////////////////////////////
//         auto AA = std::make_unique<ArrayAccess>(AATy, &*I, SE, DL, parentLoop);
//         if (!AA->ArrayAccessInit()) {
//             continue;
//         }

//         // store into ArrayAccessSet 
//         /** TODO: make this into a union-find insert */
//         ArrayAccessSet.insert(std::move(AA));
//         LLVM_DEBUG(dbgs() << "\n");
//         // AA->printAccessLinearAlgebra();
//     }
// }