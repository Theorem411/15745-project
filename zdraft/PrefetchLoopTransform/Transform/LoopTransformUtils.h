#ifndef LOOP_TRANSFORM_UTIL_H
#define LOOP_TRANSFORM_UTIL_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopIterator.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
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
#include "llvm/IR/IRBuilder.h"
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
#include "llvm/Transforms/Utils/UnrollLoop.h"
#include "llvm/Transforms/Utils/ValueMapper.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <optional>
#include <limits>

using namespace llvm;
// Check whether we are capable of peeling this loop.
bool canTransform(Loop *L);

/// \brief Update the branch weights of the latch of a peeled-off loop
/// iteration.
/// This sets the branch weights for the latch of the recently peeled off loop
/// iteration correctly. 
/// Our goal is to make sure that:
/// a) The total weight of all the copies of the loop body is preserved.
/// b) The total weight of the loop exit is preserved.
/// c) The body weight is reasonably distributed between the peeled iterations.
///
/// \param Header The copy of the header block that belongs to next iteration.
/// \param LatchBR The copy of the latch branch that belongs to this iteration.
/// \param AvgIters The average number of iterations we expect the loop to have.
/// \param[in,out] PeeledHeaderWeight The total number of dynamic loop
/// iterations that are unaccounted for. As an input, it represents the number
/// of times we expect to enter the header of the iteration currently being
/// peeled off. The output is the number of times we expect to enter the
/// header of the next iteration.
void updateBranchWeights(BasicBlock *Header, BranchInst *LatchBR, uint64_t &PeeledHeaderWeight);

/// \brief Clones the body of the loop L, putting it between \p InsertTop and \p
/// InsertBot.
/// \param Exit The exit block of the original loop.
/// \param[out] NewBlocks A list of the the blocks in the newly created clone
/// \param[out] VMap The value map between the loop and the new clone.
/// \param LoopBlocks A helper for DFS-traversal of the loop.
/// \param LVMap A value-map that maps instructions from the original loop to
/// instructions in the last peeled-off iteration.
void cloneLoopBody(Loop *L, BasicBlock *InsertTop, BasicBlock *InsertBot, BasicBlock *Exit,
                    SmallVectorImpl<BasicBlock *> &NewBlocks, LoopBlocksDFS &LoopBlocks, 
                    ValueToValueMapTy &VMap, ValueToValueMapTy &LVMap, DominatorTree *DT,
                    LoopInfo *LI); 

// Clone the original loop, insert the clone before the "InsertBefore" BB.
Loop *cloneLoop(Loop *L, BasicBlock *InsertBefore, BasicBlock *LoopDomBB, 
                    SmallVectorImpl<BasicBlock *> &NewBlocks, 
                    ValueToValueMapTy &VMap, LoopInfo *LI, ScalarEvolution *SE, 
                    DominatorTree *DT, AssumptionCache *AC, 
                    MemorySSAUpdater* MSSAU);

Loop *prependLoop(Loop *L, BasicBlock *Preheader, BasicBlock *Exit,
                    Value *InitValue, Value *FinalValue, Value *StepValue, 
                    Instruction::BinaryOps StepOpcode, ICmpInst *LatchCmpInst, 
                    LoopInfo *LI, ScalarEvolution *SE, DominatorTree *DT, 
                    AssumptionCache *AC, MemorySSAUpdater* MSSAU, 
                    PHINode **NewIndVar);

// static Value *findFinalIVValue(const Loop *L, const PHINode *Indvar, const Instruction *StepInst);
Optional<Loop::LoopBounds> getBounds(Loop *L, ScalarEvolution &SE);

#endif