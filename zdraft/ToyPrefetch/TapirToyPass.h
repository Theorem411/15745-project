#ifndef TAPIR_TOY_PASS_H
#define TAPIR_TOY_PASS_H

#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopIterator.h" // LoopBlocksDFS
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/TapirTaskInfo.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/LoopSimplify.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h" // SplitEdge
/*
BasicBlock * 	llvm::SplitEdge (BasicBlock *From, BasicBlock *To, DominatorTree *DT=nullptr, LoopInfo *LI=nullptr, MemorySSAUpdater *MSSAU=nullptr, const Twine &BBName="")
 	Split the edge connecting the specified blocks, and return the newly created basic block between From and To.

BasicBlock * 	llvm::SplitBlock (BasicBlock *Old, BasicBlock::iterator SplitPt, DominatorTree *DT, LoopInfo *LI=nullptr, MemorySSAUpdater *MSSAU=nullptr, const Twine &BBName="", bool Before=false)
 	Split the specified block at the specified instruction.
*/
#include "llvm/Transforms/Utils/TapirUtils.h"
// #include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Transforms/Tapir/TapirLoopInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

///////////////////////////////
class TapirToyPass : public FunctionPass {
public: 
    static char ID;
    TapirToyPass() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override;

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
    }

};

class TapirToyPassImpl {
public: 
    TapirToyPassImpl(Function &F, DominatorTree &DT, LoopInfo &LI, TaskInfo &TI,
                   ScalarEvolution &SE, AssumptionCache &AC,
                   TargetTransformInfo &TTI, OptimizationRemarkEmitter &ORE) 
                   : F(F), DT(DT), LI(LI), TI(TI), SE(SE), AC(AC), TTI(TTI), ORE(ORE) {};
    bool run();

    TapirLoopInfo *getTapirLoop(Loop *L) {
        if (!LoopToTapirLoop.count(L))
        return nullptr;
        return LoopToTapirLoop[L];
    }
    TapirLoopInfo *getTapirLoop(Task *T) {
        if (!TaskToTapirLoop.count(T))
        return nullptr;
        return TaskToTapirLoop[T];
    }
    TapirLoopInfo *getTapirLoop(const BasicBlock *B) {
        return getTapirLoop(LI.getLoopFor(B));
    }
private: 
    // Task *getTaskIfTapirLoopOfficial(const Loop *L, TaskInfo *TI);

    Task *getTaskIfTapirLoop(const Loop *L);

    TapirLoopInfo *createTapirLoop(Loop *L, Task *T) {
        TapirLoops.push_back(new TapirLoopInfo(L, T, ORE));
        TaskToTapirLoop[T] = TapirLoops.back();
        LoopToTapirLoop[L] = TapirLoops.back();
        return TapirLoops.back();
    }
private:
    Function &F;

    DominatorTree &DT;
    LoopInfo &LI;
    TaskInfo &TI;
    ScalarEvolution &SE;
    AssumptionCache &AC;
    TargetTransformInfo &TTI;
    OptimizationRemarkEmitter &ORE;

    std::vector<TapirLoopInfo *> TapirLoops;
    DenseMap<Task *, TapirLoopInfo *> TaskToTapirLoop;
    DenseMap<Loop *, TapirLoopInfo *> LoopToTapirLoop;
};

/////////////////------------------------------------/////////////////////
char TapirToyPass::ID = 0; 
RegisterPass<TapirToyPass> X("tapir-toy-pass", "toy pass for experiments");


#endif