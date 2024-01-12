#ifndef PREFETCH_H
#define PREFETCH_H

// llvm function, pass, const, iterators
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Analysis/LoopInfo.h"
// llvm scalar evolution
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Analysis/TapirTaskInfo.h"
#include "llvm/Analysis/MemorySSA.h"
#include "llvm/Analysis/MemorySSAUpdater.h"
#include "llvm/Transforms/Utils/LoopSimplify.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/TapirUtils.h"
// Tapir include 
#include "llvm/Transforms/Tapir/LoweringUtils.h"
#include "llvm/Transforms/Tapir/TapirLoopInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
// std lib 
#include "llvm/Support/raw_ostream.h"
#include <vector>
#include <unordered_map>
// cache locality analysis
#include "cache-locality.h"
#include "peeling.h"

using namespace std;
using namespace llvm;

STATISTIC(NumPrefetches, "Number of prefetches inserted");

class PrefetchPassImpl {
public: 
    PrefetchPassImpl(Function &F, DominatorTree &DT, LoopInfo &LI, TaskInfo &TI,
                   ScalarEvolution &SE, AssumptionCache &AC,
                   TargetTransformInfo &TTI, OptimizationRemarkEmitter &ORE,
                   MemorySSA& MSSA, MemorySSAUpdater& MSSAU) 
    : F(F), DT(DT), LI(LI), TI(TI), SE(SE), AC(AC), TTI(TTI), ORE(ORE), MSSA(MSSA), MSSAU(MSSAU) {}

    bool run();

private: 
    bool runOnLoop(Loop *L);
    Task *getTaskIfTapirLoop(const Loop *L);
    TapirLoopInfo *createTapirLoop(Loop *L, Task *T);
    Locality *getCacheLocalityLoop(const Loop *L);
    unsigned computePrefetchIterAhead(Loop *L);

private: 
    Function &F;

    DominatorTree &DT;
    LoopInfo &LI;
    TaskInfo &TI;
    ScalarEvolution &SE;
    AssumptionCache &AC;
    TargetTransformInfo &TTI;
    OptimizationRemarkEmitter &ORE;
    MemorySSA &MSSA;
    MemorySSAUpdater &MSSAU;

    // Tapir Loop 
    std::vector<TapirLoopInfo *> TapirLoops;
    DenseMap<Task *, TapirLoopInfo *> TaskToTapirLoop;
    DenseMap<Loop *, TapirLoopInfo *> LoopToTapirLoop;

    // Loop to cachelocality 
    std::vector<Loop *, Locality *> LoopToLocality;
}

class PrefetchPass : public FunctionPass {
public: 
    static char ID; 
    PrefetchPass() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
        if (skipFunction(F))
            return false;

        auto &DT = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
        auto &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
        auto &TI = getAnalysis<TaskInfoWrapperPass>().getTaskInfo();
        auto &SE = getAnalysis<ScalarEvolutionWrapperPass>().getSE();
        auto &AC = getAnalysis<AssumptionCacheTracker>().getAssumptionCache(F);
        auto &TTI = getAnalysis<TargetTransformInfoWrapperPass>().getTTI(F);
        auto &ORE = getAnalysis<OptimizationRemarkEmitterWrapperPass>().getORE();
        auto &MSSA = getAnalysis<llvm::MemorySSAWrapperPass>().getMSSA();
        MemorySSAUpdater MSSAU(&MSSA);
        errs() << "run prefetch main pass on function " << F.getName() << "\n";

        return PrefetchPassImpl(F, DT, LI, TI, SE, AC, TTI, ORE, MSSA, MSSAU).run();
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
        AU.addRequired<AssumptionCacheTracker>();
        AU.addRequired<DominatorTreeWrapperPass>();
        // AU.addPreserved<DominatorTreeWrapperPass>();
        AU.addRequired<LoopInfoWrapperPass>();
        // AU.addPreserved<LoopInfoWrapperPass>();
        AU.addRequired<OptimizationRemarkEmitterWrapperPass>();
        AU.addRequired<ScalarEvolutionWrapperPass>();
        // AU.addPreserved<ScalarEvolutionWrapperPass>();
        AU.addRequired<TargetTransformInfoWrapperPass>();
        AU.addRequired<TaskInfoWrapperPass>();
        AU.addRequired<OptimizationRemarkEmitterWrapperPass>();
        AU.addRequired<MemorySSAWrapperPass>();
    }
};

char PrefetchPass::ID = 0; 
RegisterPass<PrefetchPass> X("insert-prefetch", "insert prefetching code based on cache locality analysis");

#endif