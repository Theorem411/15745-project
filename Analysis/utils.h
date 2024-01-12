#ifndef UTILS_H
#define UTILS_H

#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/CodeMetrics.h"
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

#include <stack>
#include <utility>
#include <unordered_map>

using namespace llvm;
using namespace std;

/*= LLVM context & ConstantInt helper =================*/
ConstantInt * getConstantInt(LLVMContext &context, int64_t n);

/*= print function for llvm::Value ======================*/
std::string getShortValueName(Value *v);

/*= codemetrics code */
using BlockCost_t = const DenseMap<const BasicBlock *, unsigned>;
BlockCost_t getBlockCost(Loop *L, AssumptionCache *AC, TargetTransformInfo *TTI);
unsigned loopBodyCost(Loop *L, BlockCost_t refBook);

#endif