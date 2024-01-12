#ifndef TOY_PASS_H
#define TOY_PASS_H

#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopIterator.h"
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Analysis/TapirTaskInfo.h"
#include "llvm/Analysis/MemorySSA.h"
#include "llvm/Analysis/MemorySSAUpdater.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/LoopSimplify.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/TapirUtils.h"
#include "llvm/Transforms/Tapir/TapirLoopInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/Utils/UnrollLoop.h"

#include "Transform/PrefetchLoopTransform.h"

using namespace llvm;

class ToyPassImpl {
public: 
    ToyPassImpl(Function &F, DominatorTree &DT, LoopInfo &LI, TaskInfo &TI,
                   ScalarEvolution &SE, AssumptionCache &AC,
                   TargetTransformInfo &TTI, OptimizationRemarkEmitter &ORE,
                   MemorySSA& MSSA, MemorySSAUpdater& MSSAU) 
    : F(F), DT(DT), LI(LI), TI(TI), SE(SE), AC(AC), TTI(TTI), ORE(ORE), MSSA(MSSA), MSSAU(MSSAU) {}
    bool run();

private: 
    bool runOnLoop(Loop *L);

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

    std::vector<TapirLoopInfo *> TapirLoops;
    DenseMap<Task *, TapirLoopInfo *> TaskToTapirLoop;
    DenseMap<Loop *, TapirLoopInfo *> LoopToTapirLoop;
};

///////////////////////////////
class ToyPass : public FunctionPass {
public: 
    static char ID;
    ToyPass() : FunctionPass(ID) {}

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
        errs() << "run toy peeling pass on function " << F.getName() << "\n";

        return ToyPassImpl(F, DT, LI, TI, SE, AC, TTI, ORE, MSSA, MSSAU).run(); 
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

/////////////////------------------------------------/////////////////////
char ToyPass::ID = 0; 
RegisterPass<ToyPass> X("toy-pass", "toy pass for experiments");


#endif