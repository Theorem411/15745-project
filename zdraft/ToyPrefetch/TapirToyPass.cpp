#include "TapirToyPass.h"

bool TapirToyPassImpl::run() {
    if (TI.isSerial()) {
        (errs() << "TI is serial!\n");
        return false;
    }

    // Discover all Tapir loops and record them.
    for (Loop *TopLevelLoop : LI) {
        // TopLevelLoop->dump();
        for (Loop *L : post_order(TopLevelLoop)) {
            if (Task *T = getTaskIfTapirLoop(L)) {
                T->print(outs());
                createTapirLoop(L, T);
            }
        }
    }
        
    if (TapirLoops.empty())
        return false;
    
    return true;
}

Task *TapirToyPassImpl::getTaskIfTapirLoop(const Loop *L) {
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

///////////////////////////////
bool TapirToyPass::runOnFunction(Function &F) {
    if (skipFunction(F))
    return false;

    /// Testing TTI 
    auto &TTI = getAnalysis<TargetTransformInfoWrapperPass>().getTTI(F);
    errs() << "what is the cache line size: " << TTI.getCacheLineSize() << "\n\n";

    /// Testing TapirLoopInfo TapirTaskInfo

    auto &DT = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    auto &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
    auto &TI = getAnalysis<TaskInfoWrapperPass>().getTaskInfo();
    auto &SE = getAnalysis<ScalarEvolutionWrapperPass>().getSE();
    auto &AC = getAnalysis<AssumptionCacheTracker>().getAssumptionCache(F);
    auto &ORE = getAnalysis<OptimizationRemarkEmitterWrapperPass>().getORE();

    errs() << "run toy pass on function " << F.getName() << "\n";

    return TapirToyPassImpl(F, DT, LI, TI, SE, AC, TTI, ORE).run(); 
}



