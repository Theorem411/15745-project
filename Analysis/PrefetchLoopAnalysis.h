#ifndef LOOP_ANALYSIS_ANALYSIS_H
#define LOOP_ANALYSIS_ANALYSIS_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopIterator.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Analysis/MemorySSA.h"
#include "llvm/Analysis/MemorySSAUpdater.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/MDBuilder.h"
#include "llvm/IR/Metadata.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/LoopSimplify.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/UnrollLoop.h"  // Important file!
#include "llvm/Transforms/Utils/ValueMapper.h"

#include "ParserUtils.h"
#include "utils.h"

using namespace llvm;
using namespace std;

using AE_t = unique_ptr<AffineExpr>;
using AS_t = AffineExprSimple;
using SubExp_t = vector<AE_t>;
using SubExpS_t = vector<AS_t>;
using LT_t = vector<Loop *>;
using IV_t = vector<PHINode *>;
using L2Idx_t = unordered_map<Loop *, unsigned>;
using L2Idv_t = unordered_map<Loop *, PHINode *>;
using Idv2L_t = unordered_map<PHINode *, Loop *>;

struct LoopNestInfo {
    static Optional<LoopNestInfo> getLoopNestInfo(Loop *L, LoopInfo *LI, ScalarEvolution *SE) {
        LT_t LT;
        L2Idx_t LID;
        IV_t IVs;
        L2Idv_t L2IV;
        Idv2L_t IV2L;
        if (!trace(L, LI, SE, LT, LID, IVs, L2IV, IV2L)) {
            return None;
        }
        // errs() << "trace succeed!\n";
        return LoopNestInfo(L, LT, LID, IVs, L2IV, IV2L);
    }
private:
    static bool trace(Loop *L, LoopInfo *LI, ScalarEvolution *SE, LT_t &LT, 
                        L2Idx_t &LID, IV_t &IVs, L2Idv_t &L2IV, Idv2L_t &IV2L) {
        if (!L) {
            // errs() << "trace fail case 1\n";
            return false;
        }

        PHINode *IV = L->getInductionVariable(*SE);
        if (!IV) {
            // errs() << "trace fail case 2\n";
            return false;
        }
        IVs.push_back(IV);
        L2IV.insert({L, IV});
        IV2L.insert({IV, L});

        LID.insert({L, LT.size()});
        LT.push_back(L);

        Loop *PL = L->getParentLoop();
        if (!PL) {
            return true;
        }
        return trace(PL, LI, SE, LT, LID, IVs, L2IV, IV2L);
    }
    LoopNestInfo(Loop *L, LT_t LT, L2Idx_t LID, IV_t IVs, L2Idv_t L2IV, Idv2L_t IV2L) 
        : L(L), LT(LT), LID(LID), IVs(IVs), L2IV(L2IV), IV2L(IV2L) {}
public:
    LoopNestInfo() = default;
    Loop *L; 
    LT_t LT;
    IV_t IVs;
    L2Idx_t LID; 
    L2Idv_t L2IV;
    Idv2L_t IV2L;
};

using LToLNIF_t = DenseMap<const Loop *, LoopNestInfo>;
using LToAP_t = DenseMap<const Loop *, SmallVector<AccessPattern *>>;
using LToCost_t = DenseMap<const Loop *, unsigned>;
class PrefetchLoopAnalysis {
public: 
    PrefetchLoopAnalysis(LoopInfo &LI, ScalarEvolution &SE, DominatorTree &DT,
                    AssumptionCache &AC, TaskInfo &TI, TargetTransformInfo &TTI, 
                    OptimizationRemarkEmitter& ORE, MemorySSAUpdater &MSSAU) 
        : LI(LI), SE(SE), DT(DT), AC(AC), TI(TI), TTI(TTI),ORE(ORE), MSSAU(MSSAU) {}
    
    ~PrefetchLoopAnalysis() {
        // Iterate over each entry in the LToAP map
        for (auto &MapEntry : LToAP) {
            SmallVector<AccessPattern*> &APVector = MapEntry.second;
            for (AccessPattern *AP : APVector) {
                delete AP;
            }
            APVector.clear();
        }
    }

    bool run();
    
    unsigned computePrefetchItersAhead(Loop *L);
    bool isValidLoop(Loop *L) { return LToLNIF.find(L) != LToLNIF.end(); }
    
    bool isSpatial(Loop *L);
    bool isTemporal(Loop *L);

    SmallVector<AccessPattern *> getAccessPattern(Loop *L);
    SmallVector<AccessPattern *> getSpatialAccess(Loop *L);
    SmallVector<AccessPattern *> getTemporalAccess(Loop *L);
    
    const LoopNestInfo &getLNIF(Loop *L);
    vector<PHINode *> getIVs(Loop *L) {
        assert(isValidLoop(L) && "getIVs called on invalid loop");
        LoopNestInfo LNIF = LToLNIF[L];
        return LNIF.IVs;
    }

    unsigned getLoopMemUse(Loop *L);
    unsigned getLoopLength(Loop *L);
private:
    bool runOnLoop1(Loop *L);
    bool runOnLoop2(Loop *L);
    unsigned getL1CacheSize() { return 32000; }
    
private:
    // collect during runOnLoop1 & finalize during runOnLoop2
    LToAP_t LToAP;
    LToCost_t LToMemUse;
    LToCost_t LToLength;
    LToLNIF_t LToLNIF;
    unordered_set<Loop *> LocalizedLoops;
    // collect during runOnLoop2
    LToAP_t LToTLoc;
    LToAP_t LToSLoc;

    LoopInfo &LI;
    ScalarEvolution &SE; 
    DominatorTree &DT;
    AssumptionCache &AC; 
    TaskInfo &TI; 
    TargetTransformInfo &TTI; 
    OptimizationRemarkEmitter& ORE; 
    MemorySSAUpdater &MSSAU;
};


#endif 