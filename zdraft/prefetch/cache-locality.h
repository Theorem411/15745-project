#ifndef CACHE_LOCALITY_H
#define CACHE_LOCALITY_H

#include "utils.h"
#include "array-access.h"
// llvm function, pass, const, iterators
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Support/Debug.h"
// llvm scalar evolution
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
// Tapir include 
#include "llvm/Analysis/TapirTaskInfo.h"
#include "llvm/Transforms/Tapir/TapirLoopInfo.h"
// std lib 
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace llvm;
using namespace std;
// #define DEBUG_TYPE "cache-locality"

/*= LLVM context =================*/
// llvm::LLVMContext context;

/*= CacheLocality Analysis main class ==================*/
// class CacheLocality {
//     // extrinsic info
//     const Module *M;
//     Function &F;
//     LoopInfo &LI;
//     ScalarEvolution &SE; 
//     const DataLayout &DL;
//     const TargetTransformInfo *TTI;
//     // Loop nest info
//     using LoopNest_t = std::unordered_map<Loop *, std::vector<Loop *>>;
//     LoopNest_t LoopNests;
//     // Loop trace info
//     using LoopTrace_t = std::unordered_map<Loop *, std::vector<Loop *>>;
//     LoopTrace_t LoopTrace; // each innermost loop map to a ordered traceback of loop parentage
//     // Loop to induction variable mapping
//     using Indvar2Loop_t = std::unordered_map<PHINode *, Loop *>;
//     using LP2Indvar_t = std::unordered_map<Loop *, PHINode *>;
//     Indvar2Loop_t Indvar2Loop;
//     LP2Indvar_t Loop2Indvar;
//     // cache locality info /////////////////////////////////////////
//     using ArrayAccessSet_t = std::unordered_set<std::unique_ptr<ArrayAccess>>;
//     ArrayAccessSet_t ArrayAccessSet; /** TODO: make this into a union-find datastructure representing group locality */

//     // LoopNest helper functions ///////////////////////////////////
//     void LoopNestsInitRecursive(Loop *OutermostLoop);
//     void LoopNestsInit();
//     // LoopTrace helper functions /////////////////////////////////
//     void LoopTraceInitRecursive(Loop *L, std::vector<Loop *> &Trace);
//     void LoopTraceInit();
//     // Loop to induction variable helper ///////////////////////////
//     void IndvarLoopInitRecursive(Loop *L);
//     void IndvarLoopInit();
//     // ArrayAccessSet help ///////////////////////////
//     void ArrayAccessSetInit();

// public: 
//     /** CacheLocality: 
//         - Function F
//         - LoopInfo produced by LoopWrapperPass on F
//         - ScalarEvolution produced by ScalarEvolutionWrapperPass on F
//     */
//     CacheLocality(
//         Function &F, 
//         const Module *M, 
//         const DataLayout &DL, 
//         LoopInfo &LI, 
//         ScalarEvolution &SE, 
//         const TargetTransformInfo *TTI) 
//         : F(F), M(M), DL(DL), LI(LI), SE(SE), TTI(TTI) {
//         LLVM_DEBUG(dbgs() << "== Constructing CacheLocality ====================\n");
//         // build loop tree graph representation
//         LLVM_DEBUG(dbgs() << "-- begin LoopNestsInit --------------------------\n");
//         LoopNestsInit();
//         pp_LoopNests();
//         LLVM_DEBUG(dbgs() << "\n");

//         // Loop trace init 
//         LLVM_DEBUG(dbgs() << "-- begin LoopTraceInit ---------------------------\n");
//         LoopTraceInit();
//         pp_LoopTrace();
//         LLVM_DEBUG(dbgs() << "\n");
        
//         // build mapping between induction variables and their optimizable loops
//         LLVM_DEBUG(dbgs() << "-- begin IndvarLoopInit --------------------------\n");
//         IndvarLoopInit();
//         LLVM_DEBUG(dbgs() << "\n");
//         // build array access sets
//         LLVM_DEBUG(dbgs() << "-- begin ArrayAccessSetInit ----------------------\n");
//         ArrayAccessSetInit();
//         LLVM_DEBUG(dbgs() << "\n");
//     }
// };

class Locality {
public: 
    Locality(Loop *L, DominatorTree &DT, LoopInfo &LI, TaskInfo &TI,
        ScalarEvolution &SE, AssumptionCache &AC, TargetTransformInfo &TTI, 
        OptimizationRemarkEmitter &ORE) 
    : L(L), DT(DT), LI(LI), TI(TI), SE(SE), AC(AC), TTI(TTI), ORE(ORE), valid(true) {
        initLoop();
        initGroupAccesses();
    }

    bool isValid() const {
        // a Locality class is valid iff properly initialized
        // - loop trace must all be for-loop with primary induction variable
        return valid;
    }

    bool isTemporal() const;
    vector<GroupAccess *> getTemporalLocality();

    bool isSpatial() const;
    vector<GroupAccess *> getSpatialLocality();

    bool isEmpty();
private:
    bool initLoopRecursive(Loop *L, vector<Loop *>& trace);
    void initLoop();
    /// TODO: compute loops in loopTrace that access less memory than L1 cache size
    bool isLocalizedLoop(Loop *L);

    void initGroupAccesses();

    PHINode *getInductionVariable(Loop *L);
private: 
    DominatorTree &DT;
    LoopInfo &LI;
    TaskInfo &TI;
    ScalarEvolution &SE;
    AssumptionCache &AC;
    TargetTransformInfo &TTI;
    OptimizationRemarkEmitter &ORE;

    bool valid;

    Loop *L;
    vector<Loop *> LoopTrace;
    unordered_map<Loop *, unsigned> LoopIndex;
    unordered_map<Loop *, PHINode *> LoopToIndvar;
    unordered_map<PHINode *, Loop *> IndvarToLoop;
    unordered_set<Loop *> LocalizedLoops; // subset of LoopTrace that realize loop-carried locality

    vector<GroupAccess *> GroupAccesses;
}

#endif