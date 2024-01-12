#ifndef PREFETCH_LOOP_TRANFORM_H
#define PREFETCH_LOOP_TRANFORM_H

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
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>

#include "LoopTransformUtils.h"

using namespace llvm;
using namespace std;

class PrefetchLoopTransform {
    public:
    PrefetchLoopTransform(LoopInfo &LI, ScalarEvolution &SE, DominatorTree &DT,
                    AssumptionCache &AC, TaskInfo &TI, TargetTransformInfo &TTI, 
                    OptimizationRemarkEmitter& ORE, MemorySSAUpdater &MSSAU) 
        : LI(LI), SE(SE), DT(DT), AC(AC), TI(TI), TTI(TTI),ORE(ORE), MSSAU(MSSAU) {}

    bool run(Loop *L, bool temporal, bool spatial, unsigned PrefetchItersAhead, unsigned UnrollTimes);

private: 
    Loop *splitLoop(Loop *L, unsigned PrefetchItersAhead, unsigned UnrollTimes);
    Instruction *computeSplitPoint(Loop *L, unsigned PrefetchItersAhead, unsigned UnrollTimes, Instruction *InsertBefore) const;
    // split L into <Loop> <Epilog>
    LoopUnrollResult unrollLoop(Loop *L, unsigned UnrollTimes, Loop ** RemainderLoop);
    BasicBlock *peelFirstIter(Loop *L);
    Loop *insertProlog(Loop *L, unsigned PrefetchItersAhead, PHINode **NewIndVar);
private:
    LoopInfo &LI;
    ScalarEvolution &SE; 
    DominatorTree &DT;
    AssumptionCache &AC; TaskInfo &TI; 
    TargetTransformInfo &TTI; 
    OptimizationRemarkEmitter& ORE; 
    MemorySSAUpdater &MSSAU;
};

#endif