//===- UnrollLoopPeel.cpp - Loop peeling utilities ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements some loop unrolling utilities for peeling loops
// with dynamically inferred (from PGO) trip counts. See LoopUnroll.cpp for
// unrolling loops with compile-time constant trip counts.
//
//===----------------------------------------------------------------------===//

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
#include "llvm/Transforms/Utils/UnrollLoop.h"
#include "llvm/Transforms/Utils/ValueMapper.h"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>

using namespace llvm;

#define DEBUG_TYPE "peeling"

STATISTIC(NumPeeled, "Number of loops peeled");

// static cl::opt<unsigned> UnrollPeelMaxCount(
    // "unroll-peel-max-count", cl::init(7), cl::Hidden,
    // cl::desc("Max average trip count which will cause loop peeling."));

// static cl::opt<unsigned> UnrollForcePeelCount(
    // "unroll-force-peel-count", cl::init(0), cl::Hidden,
    // cl::desc("Force a peel count regardless of profiling information."));

// Check whether we are capable of peeling this loop.
bool canPeel(Loop *L) {
  // Make sure the loop is in simplified form
  if (!L->isLoopSimplifyForm())
    return false;

  // Only peel loops that contain a single exit
  if (!L->getExitingBlock() || !L->getUniqueExitBlock())
    return false;

  // Don't try to peel loops where the latch is not the exiting block.
  // This can be an indication of two different things:
  // 1) The loop is not rotated.
  // 2) The loop contains irreducible control flow that involves the latch.
  if (L->getLoopLatch() != L->getExitingBlock())
    return false;

  return true;
}

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
/// \param IterNumber The serial number of the iteration that was just
/// peeled off.
/// \param AvgIters The average number of iterations we expect the loop to have.
/// \param[in,out] PeeledHeaderWeight The total number of dynamic loop
/// iterations that are unaccounted for. As an input, it represents the number
/// of times we expect to enter the header of the iteration currently being
/// peeled off. The output is the number of times we expect to enter the
/// header of the next iteration.
static void updateBranchWeights(BasicBlock *Header, BranchInst *LatchBR,
                                unsigned IterNumber, unsigned AvgIters,
                                uint64_t &PeeledHeaderWeight) {
  // FIXME: Pick a more realistic distribution.
  // Currently the proportion of weight we assign to the fall-through
  // side of the branch drops linearly with the iteration number, and we use
  // a 0.9 fudge factor to make the drop-off less sharp...
  if (PeeledHeaderWeight) {
    uint64_t FallThruWeight =
        PeeledHeaderWeight * ((float)(AvgIters - IterNumber) / AvgIters * 0.9);
    uint64_t ExitWeight = PeeledHeaderWeight - FallThruWeight;
    PeeledHeaderWeight -= ExitWeight;

    unsigned HeaderIdx = (LatchBR->getSuccessor(0) == Header ? 0 : 1);
    MDBuilder MDB(LatchBR->getContext());
    MDNode *WeightNode =
        HeaderIdx ? MDB.createBranchWeights(ExitWeight, FallThruWeight)
                  : MDB.createBranchWeights(FallThruWeight, ExitWeight);
    LatchBR->setMetadata(LLVMContext::MD_prof, WeightNode);
  }
}

/// \brief Clones the body of the loop L, putting it between \p InsertTop and \p
/// InsertBot.
/// \param IterNumber The serial number of the iteration currently being
/// peeled off.
/// \param Exit The exit block of the original loop.
/// \param[out] NewBlocks A list of the the blocks in the newly created clone
/// \param[out] VMap The value map between the loop and the new clone.
/// \param LoopBlocks A helper for DFS-traversal of the loop.
/// \param LVMap A value-map that maps instructions from the original loop to
/// instructions in the last peeled-off iteration.
static void cloneLoopBlocks(Loop *L, unsigned IterNumber, BasicBlock *InsertTop,
                            BasicBlock *InsertBot, BasicBlock *Exit,
                            SmallVectorImpl<BasicBlock *> &NewBlocks,
                            LoopBlocksDFS &LoopBlocks, ValueToValueMapTy &VMap,
                            ValueToValueMapTy &LVMap, DominatorTree *DT,
                            LoopInfo *LI) {
  BasicBlock *Header = L->getHeader();
  BasicBlock *Latch = L->getLoopLatch();
  BasicBlock *PreHeader = L->getLoopPreheader();

  Function *F = Header->getParent();
  LoopBlocksDFS::RPOIterator BlockBegin = LoopBlocks.beginRPO();
  LoopBlocksDFS::RPOIterator BlockEnd = LoopBlocks.endRPO();
  Loop *ParentLoop = L->getParentLoop();

  // For each block in the original loop, create a new copy,
  // and update the value map with the newly created values.
  for (LoopBlocksDFS::RPOIterator BB = BlockBegin; BB != BlockEnd; ++BB) {
    BasicBlock *NewBB = CloneBasicBlock(*BB, VMap, ".peel", F);
    NewBlocks.push_back(NewBB);

    if (ParentLoop)
      ParentLoop->addBasicBlockToLoop(NewBB, *LI);

    VMap[*BB] = NewBB;

    // If dominator tree is available, insert nodes to represent cloned blocks.
    if (DT) {
      if (Header == *BB)
        DT->addNewBlock(NewBB, InsertTop);
      else {
        DomTreeNode *IDom = DT->getNode(*BB)->getIDom();
        // VMap must contain entry for IDom, as the iteration order is RPO.
        DT->addNewBlock(NewBB, cast<BasicBlock>(VMap[IDom->getBlock()]));
      }
    }
  }

  // Hook-up the control flow for the newly inserted blocks.
  // The new header is hooked up directly to the "top", which is either
  // the original loop preheader (for the first iteration) or the previous
  // iteration's exiting block (for every other iteration)
  InsertTop->getTerminator()->setSuccessor(0, cast<BasicBlock>(VMap[Header]));

  // Similarly, for the latch:
  // The original exiting edge is still hooked up to the loop exit.
  // The backedge now goes to the "bottom", which is either the loop's real
  // header (for the last peeled iteration) or the copied header of the next
  // iteration (for every other iteration)
  BasicBlock *NewLatch = cast<BasicBlock>(VMap[Latch]);
  BranchInst *LatchBR = cast<BranchInst>(NewLatch->getTerminator());
  unsigned HeaderIdx = (LatchBR->getSuccessor(0) == Header ? 0 : 1);
  LatchBR->setSuccessor(HeaderIdx, InsertBot);
  LatchBR->setSuccessor(1 - HeaderIdx, Exit);
  if (DT)
    DT->changeImmediateDominator(InsertBot, NewLatch);

  // The new copy of the loop body starts with a bunch of PHI nodes
  // that pick an incoming value from either the preheader, or the previous
  // loop iteration. Since this copy is no longer part of the loop, we
  // resolve this statically:
  // For the first iteration, we use the value from the preheader directly.
  // For any other iteration, we replace the phi with the value generated by
  // the immediately preceding clone of the loop body (which represents
  // the previous iteration).
  for (BasicBlock::iterator I = Header->begin(); isa<PHINode>(I); ++I) {
    PHINode *NewPHI = cast<PHINode>(VMap[&*I]);
    if (IterNumber == 0) {
      VMap[&*I] = NewPHI->getIncomingValueForBlock(PreHeader);
    } else {
      Value *LatchVal = NewPHI->getIncomingValueForBlock(Latch);
      Instruction *LatchInst = dyn_cast<Instruction>(LatchVal);
      if (LatchInst && L->contains(LatchInst))
        VMap[&*I] = LVMap[LatchInst];
      else
        VMap[&*I] = LatchVal;
    }
    cast<BasicBlock>(VMap[Header])->getInstList().erase(NewPHI);
  }

  // Fix up the outgoing values - we need to add a value for the iteration
  // we've just created. Note that this must happen *after* the incoming
  // values are adjusted, since the value going out of the latch may also be
  // a value coming into the header.
  for (BasicBlock::iterator I = Exit->begin(); isa<PHINode>(I); ++I) {
    PHINode *PHI = cast<PHINode>(I);
    Value *LatchVal = PHI->getIncomingValueForBlock(Latch);
    Instruction *LatchInst = dyn_cast<Instruction>(LatchVal);
    if (LatchInst && L->contains(LatchInst))
      LatchVal = VMap[LatchVal];
    PHI->addIncoming(LatchVal, cast<BasicBlock>(VMap[Latch]));
  }

  // LastValueMap is updated with the values for the current loop
  // which are used the next time this function is called.
  for (const auto &KV : VMap)
    LVMap[KV.first] = KV.second;
}

/// \brief Peel off the first \p PeelCount iterations of loop \p L.
///
/// Note that this does not peel them off as a single straight-line block.
/// Rather, each iteration is peeled off separately, and needs to check the
/// exit condition.
/// For loops that dynamically execute \p PeelCount iterations or less
/// this provides a benefit, since the peeled off iterations, which account
/// for the bulk of dynamic execution, can be further simplified by scalar
/// optimizations.
BasicBlock * peelLoop(Loop *L, unsigned PeelCount, LoopInfo *LI,
                    ScalarEvolution *SE, DominatorTree *DT,
                    AssumptionCache *AC, llvm::MemorySSAUpdater* MSSAU,
                    bool PreserveLCSSA) {
    if (!canPeel(L))
    return nullptr;

    LoopBlocksDFS LoopBlocks(L);
    LoopBlocks.perform(LI);

    BasicBlock *Header = L->getHeader();
    BasicBlock *PreHeader = L->getLoopPreheader();
    BasicBlock *Latch = L->getLoopLatch();
    BasicBlock *Exit = L->getUniqueExitBlock();

    Function *F = Header->getParent();

    errs() << F->getName() << ": <Header>" << Header->getName() << ": <Preheader>" 
    << PreHeader->getName() << ": <Latch>" << Latch->getName() 
    << ": <Exit>" << Exit->getName() << "\n";

    // Set up all the necessary basic blocks. It is convenient to split the
    // preheader into 3 parts - two blocks to anchor the peeled copy of the loop
    // body, and a new preheader for the "real" loop.

    // Peeling the first iteration transforms.
    //
    // PreHeader:
    // ...
    // Header:
    //   LoopBody
    //   If (cond) goto Header
    // Exit:
    //
    // into
    //
    // InsertTop:
    //   LoopBody
    //   If (!cond) goto Exit
    // InsertBot:
    // NewPreHeader:
    // ...
    // Header:
    //  LoopBody
    //  If (cond) goto Header
    // Exit:
    //
    // Each following iteration will split the current bottom anchor in two,
    // and put the new copy of the loop body between these two blocks. That is,
    // after peeling another iteration from the example above, we'll split 
    // InsertBot, and get:
    //
    // InsertTop:
    //   LoopBody
    //   If (!cond) goto Exit
    // InsertBot:
    //   LoopBody
    //   If (!cond) goto Exit
    // InsertBot.next:
    // NewPreHeader:
    // ...
    // Header:
    //  LoopBody
    //  If (cond) goto Header
    // Exit:

    BasicBlock *InsertTop = SplitEdge(PreHeader, Header, DT, LI);

    BasicBlock *InsertBot =
        SplitBlock(InsertTop, InsertTop->getTerminator(), DT, LI);
    BasicBlock *NewPreHeader =
        SplitBlock(InsertBot, InsertBot->getTerminator(), DT, LI);

    InsertTop->setName(Header->getName() + ".peel.begin");
    InsertBot->setName(Header->getName() + ".peel.next");
    NewPreHeader->setName(PreHeader->getName() + ".peel.newph");

    errs() << "<InsertTop>: " << InsertTop->getName() << "<InsertBot>: " << InsertBot->getName() 
    << "NewPreheader" << NewPreHeader->getName() << "\n\n";

    ValueToValueMapTy LVMap;

    // If we have branch weight information, we'll want to update it for the
    // newly created branches.
    BranchInst *LatchBR =
        cast<BranchInst>(cast<BasicBlock>(Latch)->getTerminator());
    unsigned HeaderIdx = (LatchBR->getSuccessor(0) == Header ? 0 : 1);

    uint64_t TrueWeight, FalseWeight;
    uint64_t ExitWeight = 0, CurHeaderWeight = 0;
    if (LatchBR->extractProfMetadata(TrueWeight, FalseWeight)) {
    ExitWeight = HeaderIdx ? TrueWeight : FalseWeight;
    // The # of times the loop body executes is the sum of the exit block
    // weight and the # of times the backedges are taken.
    CurHeaderWeight = TrueWeight + FalseWeight;
    }

    // For each peeled-off iteration, make a copy of the loop.
    SmallVector<BasicBlock *, 8> NewBlocks;
    ValueToValueMapTy VMap;

    // Subtract the exit weight from the current header weight -- the exit
    // weight is exactly the weight of the previous iteration's header.
    // FIXME: due to the way the distribution is constructed, we need a
    // guard here to make sure we don't end up with non-positive weights.
    if (ExitWeight < CurHeaderWeight)
        CurHeaderWeight -= ExitWeight;
    else
        CurHeaderWeight = 1;

    cloneLoopBlocks(L, 0, InsertTop, InsertBot, Exit,
                    NewBlocks, LoopBlocks, VMap, LVMap, DT, LI);

    // Remap to use values from the current iteration instead of the
    // previous one.
    remapInstructionsInBlocks(NewBlocks, VMap);

    if (DT) {
        // Latches of the cloned loops dominate over the loop exit, so idom of the
        // latter is the first cloned loop body, as original PreHeader dominates
        // the original loop body.
    //   if (Iter == 0)
        DT->changeImmediateDominator(Exit, cast<BasicBlock>(LVMap[Latch]));
    }

    updateBranchWeights(InsertBot, cast<BranchInst>(VMap[LatchBR]), 0, // Iter
                        PeelCount, ExitWeight);

    // InsertTop = InsertBot;
    // InsertBot = SplitBlock(InsertBot, InsertBot->getTerminator(), DT, LI);
    // InsertBot->setName(Header->getName() + ".peel.next");

    F->getBasicBlockList().splice(InsertBot->getIterator(),
                                    F->getBasicBlockList(),
                                    NewBlocks[0]->getIterator(), F->end());

    // Now adjust the phi nodes in the loop header to get their initial values
    // from the last peeled-off iteration instead of the preheader.
    for (BasicBlock::iterator I = Header->begin(); isa<PHINode>(I); ++I) {
        PHINode *PHI = cast<PHINode>(I);
        Value *NewVal = PHI->getIncomingValueForBlock(Latch);
        Instruction *LatchInst = dyn_cast<Instruction>(NewVal);
        if (LatchInst && L->contains(LatchInst))
            NewVal = LVMap[LatchInst];

        PHI->setIncomingValue(PHI->getBasicBlockIndex(NewPreHeader), NewVal);
    }

    // Adjust the branch weights on the loop exit.
    if (ExitWeight) {
        // The backedge count is the difference of current header weight and
        // current loop exit weight. If the current header weight is smaller than
        // the current loop exit weight, we mark the loop backedge weight as 1.
        uint64_t BackEdgeWeight = 0;
        if (ExitWeight < CurHeaderWeight)
            BackEdgeWeight = CurHeaderWeight - ExitWeight;
        else
            BackEdgeWeight = 1;
        MDBuilder MDB(LatchBR->getContext());
        MDNode *WeightNode =
            HeaderIdx ? MDB.createBranchWeights(ExitWeight, BackEdgeWeight)
                        : MDB.createBranchWeights(BackEdgeWeight, ExitWeight);
        LatchBR->setMetadata(LLVMContext::MD_prof, WeightNode);
    }

    // If the loop is nested, we changed the parent loop, update SE.
    if (Loop *ParentLoop = L->getParentLoop()) {
        SE->forgetLoop(ParentLoop);

        // // FIXME: Incrementally update loop-simplify
        simplifyLoop(ParentLoop, DT, LI, SE, AC, MSSAU, PreserveLCSSA);
    } else {
        // FIXME: Incrementally update loop-simplify
        simplifyLoop(L, DT, LI, SE, AC, MSSAU, PreserveLCSSA);
    }

    NumPeeled++;

    return InsertTop;
}