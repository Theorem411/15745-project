#ifndef PEEL_TOY_PASS_H
#define PEEL_TOY_PASS_H

#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Analysis/LoopInfo.h"
// #include "llvm/Analysis/LoopIterator.h"
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Analysis/TapirTaskInfo.h"
#include "llvm/Analysis/MemorySSA.h"
#include "llvm/Analysis/MemorySSAUpdater.h"
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

#include "llvm/Transforms/Utils/UnrollLoop.h"
#include "peeling.h"

using namespace llvm;

class PeelingToyPassImpl {
public: 
    PeelingToyPassImpl(Function &F, DominatorTree &DT, LoopInfo &LI, TaskInfo &TI,
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
};

///////////////////////////////
class PeelToyPass : public FunctionPass {
public: 
    static char ID;
    PeelToyPass() : FunctionPass(ID) {}

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

        return PeelingToyPassImpl(F, DT, LI, TI, SE, AC, TTI, ORE, MSSA, MSSAU).run(); 
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
char PeelToyPass::ID = 0; 
RegisterPass<PeelToyPass> X("peeling-toy-pass", "toy pass for experiments");


#endif