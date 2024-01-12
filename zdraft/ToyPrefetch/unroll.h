//===-- UnrollLoop.cpp - Loop unrolling utilities -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements some loop unrolling utilities. It does not define any
// actual pass or policy, but provides a single function to perform loop
// unrolling.
//
// The process of unrolling can produce extraneous basic blocks linked with
// unconditional branches.  This will be corrected in the future.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/InstructionSimplify.h"
#include "llvm/Analysis/LoopIterator.h"
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/TapirTaskInfo.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Transforms/Utils/LoopSimplify.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/SimplifyIndVar.h"
#include "llvm/Transforms/Utils/TapirUtils.h"
using namespace llvm;

/// Represents the result of a \c UnrollLoop invocation.
enum Result {
  /// The loop was not modified.
  Unmodified,
 
  /// The loop was partially unrolled -- we still have a loop, but with a
  /// smaller trip count.  We may also have emitted epilogue loop if the loop
  /// had a non-constant trip count.
  PartiallyUnrolled,
 
  /// The loop was fully unrolled into straight-line code.  We no longer have
  /// any back-edges.
  FullyUnrolled
};

// TODO: Should these be here or in LoopUnroll?
STATISTIC(NumCompletelyUnrolled, "Number of loops completely unrolled");
STATISTIC(NumUnrolled, "Number of loops unrolled (completely or otherwise)");
STATISTIC(NumRuntimeUnrolled,
          "Number of loops unrolled with run-time trip counts");
// static cl::opt<bool>
// UnrollRuntimeEpilog("unroll-runtime-epilog", cl::init(false), cl::Hidden,
//                     cl::desc("Allow runtime unrolled loops to be unrolled "
//                              "with epilog instead of prolog."));

// static cl::opt<bool>
// UnrollVerifyDomtree("unroll-verify-domtree", cl::Hidden,
//                     cl::desc("Verify domtree after unrolling"),
// #ifdef NDEBUG
//     cl::init(false)
// #else
//     cl::init(true)
// #endif
//                     );

/// Convert the instruction operands from referencing the current values into
/// those specified by VMap.
static inline void remapInstruction(Instruction *I,
                                    ValueToValueMapTy &VMap) {
  for (unsigned op = 0, E = I->getNumOperands(); op != E; ++op) {
    Value *Op = I->getOperand(op);

    // Unwrap arguments of dbg.value intrinsics.
    bool Wrapped = false;
    if (auto *V = dyn_cast<MetadataAsValue>(Op))
      if (auto *Unwrapped = dyn_cast<ValueAsMetadata>(V->getMetadata())) {
        Op = Unwrapped->getValue();
        Wrapped = true;
      }

    auto wrap = [&](Value *V) {
      auto &C = I->getContext();
      return Wrapped ? MetadataAsValue::get(C, ValueAsMetadata::get(V)) : V;
    };

    ValueToValueMapTy::iterator It = VMap.find(Op);
    if (It != VMap.end())
      I->setOperand(op, wrap(It->second));
  }

  if (PHINode *PN = dyn_cast<PHINode>(I)) {
    for (unsigned i = 0, e = PN->getNumIncomingValues(); i != e; ++i) {
      ValueToValueMapTy::iterator It = VMap.find(PN->getIncomingBlock(i));
      if (It != VMap.end())
        PN->setIncomingBlock(i, cast<BasicBlock>(It->second));
    }
  }
}

/// Folds a basic block into its predecessor if it only has one predecessor, and
/// that predecessor only has one successor.
/// The LoopInfo Analysis that is passed will be kept consistent.  If folding is
/// successful references to the containing loop must be removed from
/// ScalarEvolution by calling ScalarEvolution::forgetLoop because SE may have
/// references to the eliminated BB.  The argument ForgottenLoops contains a set
/// of loops that have already been forgotten to prevent redundant, expensive
/// calls to ScalarEvolution::forgetLoop.  Returns the new combined block.
static BasicBlock *foldBlockIntoPredecessor(BasicBlock *BB, LoopInfo *LI, ScalarEvolution *SE,
                         SmallPtrSetImpl<Loop *> &ForgottenLoops,
                         DominatorTree *DT) {
  // Merge basic blocks into their predecessor if there is only one distinct
  // pred, and if there is only one distinct successor of the predecessor, and
  // if there are no PHI nodes.
  BasicBlock *OnlyPred = BB->getSinglePredecessor();
  if (!OnlyPred) return nullptr;

  if (OnlyPred->getTerminator()->getNumSuccessors() != 1)
    return nullptr;

  (errs() << "Merging: " << *BB << "into: " << *OnlyPred);

  // Resolve any PHI nodes at the start of the block.  They are all
  // guaranteed to have exactly one entry if they exist, unless there are
  // multiple duplicate (but guaranteed to be equal) entries for the
  // incoming edges.  This occurs when there are multiple edges from
  // OnlyPred to OnlySucc.
  FoldSingleEntryPHINodes(BB);

  // Delete the unconditional branch from the predecessor...
  OnlyPred->getInstList().pop_back();

  // Make all PHI nodes that referred to BB now refer to Pred as their
  // source...
  BB->replaceAllUsesWith(OnlyPred);

  // Move all definitions in the successor to the predecessor...
  OnlyPred->getInstList().splice(OnlyPred->end(), BB->getInstList());

  // OldName will be valid until erased.
  StringRef OldName = BB->getName();

  // Erase the old block and update dominator info.
  if (DT)
    if (DomTreeNode *DTN = DT->getNode(BB)) {
      DomTreeNode *PredDTN = DT->getNode(OnlyPred);
      SmallVector<DomTreeNode *, 8> Children(DTN->begin(), DTN->end());
      for (auto *DI : Children)
        DT->changeImmediateDominator(DI, PredDTN);

      DT->eraseNode(BB);
    }

  // ScalarEvolution holds references to loop exit blocks.
  if (SE) {
    if (Loop *L = LI->getLoopFor(BB)) {
      if (ForgottenLoops.insert(L).second)
        SE->forgetLoop(L);
    }
  }
  LI->removeBlock(BB);

  // Inherit predecessor's name if it exists...
  if (!OldName.empty() && !OnlyPred->hasName())
    OnlyPred->setName(OldName);

  BB->eraseFromParent();

  return OnlyPred;
}

/// Check if unrolling created a situation where we need to insert phi nodes to
/// preserve LCSSA form.
/// \param Blocks is a vector of basic blocks representing unrolled loop.
/// \param L is the outer loop.
/// It's possible that some of the blocks are in L, and some are not. In this
/// case, if there is a use is outside L, and definition is inside L, we need to
/// insert a phi-node, otherwise LCSSA will be broken.
/// The function is just a helper function for llvm::UnrollLoop that returns
/// true if this situation occurs, indicating that LCSSA needs to be fixed.
static bool needToInsertPhisForLCSSA(Loop *L, std::vector<BasicBlock *> Blocks,
                                     LoopInfo *LI) {
  for (BasicBlock *BB : Blocks) {
    if (LI->getLoopFor(BB) == L)
      continue;
    for (Instruction &I : *BB) {
      for (Use &U : I.operands()) {
        if (auto Def = dyn_cast<Instruction>(U)) {
          Loop *DefLoop = LI->getLoopFor(Def->getParent());
          if (!DefLoop)
            continue;
          if (DefLoop->contains(L))
            return true;
        }
      }
    }
  }
  return false;
}

/// Adds ClonedBB to LoopInfo, creates a new loop for ClonedBB if necessary
/// and adds a mapping from the original loop to the new loop to NewLoops.
/// Returns nullptr if no new loop was created and a pointer to the
/// original loop OriginalBB was part of otherwise.
const Loop* llvm::addClonedBlockToLoopInfo(BasicBlock *OriginalBB,
                                           BasicBlock *ClonedBB, LoopInfo *LI,
                                           NewLoopsMap &NewLoops) {
  // Figure out which loop New is in.
  const Loop *OldLoop = LI->getLoopFor(OriginalBB);
  assert(OldLoop && "Should (at least) be in the loop being unrolled!");

  Loop *&NewLoop = NewLoops[OldLoop];
  if (!NewLoop) {
    // Found a new sub-loop.
    assert(OriginalBB == OldLoop->getHeader() &&
           "Header should be first in RPO");

    NewLoop = LI->AllocateLoop();
    Loop *NewLoopParent = NewLoops.lookup(OldLoop->getParentLoop());

    if (NewLoopParent)
      NewLoopParent->addChildLoop(NewLoop);
    else
      LI->addTopLevelLoop(NewLoop);

    NewLoop->addBasicBlockToLoop(ClonedBB, *LI);
    return OldLoop;
  } else {
    NewLoop->addBasicBlockToLoop(ClonedBB, *LI);
    return nullptr;
  }
}

/// The function chooses which type of unroll (epilog or prolog) is more
/// profitabale.
/// Epilog unroll is more profitable when there is PHI that starts from
/// constant.  In this case epilog will leave PHI start from constant,
/// but prolog will convert it to non-constant.
///
/// loop:
///   PN = PHI [I, Latch], [CI, PreHeader]
///   I = foo(PN)
///   ...
///
/// Epilog unroll case.
/// loop:
///   PN = PHI [I2, Latch], [CI, PreHeader]
///   I1 = foo(PN)
///   I2 = foo(I1)
///   ...
/// Prolog unroll case.
///   NewPN = PHI [PrologI, Prolog], [CI, PreHeader]
/// loop:
///   PN = PHI [I2, Latch], [NewPN, PreHeader]
///   I1 = foo(PN)
///   I2 = foo(I1)
///   ...
///
static bool isEpilogProfitable(Loop *L) {
  BasicBlock *PreHeader = L->getLoopPreheader();
  BasicBlock *Header = L->getHeader();
  assert(PreHeader && Header);
  for (const PHINode &PN : Header->phis()) {
    if (isa<ConstantInt>(PN.getIncomingValueForBlock(PreHeader)))
      return true;
  }
  return false;
}

/// Returns true if we can safely unroll a multi-exit/exiting loop. OtherExits
/// is populated with all the loop exit blocks other than the LatchExit block.
static bool canSafelyUnrollMultiExitLoop(Loop *L, SmallVectorImpl<BasicBlock *> &OtherExits,
                             BasicBlock *LatchExit, bool PreserveLCSSA,
                             bool UseEpilogRemainder) {

  // We currently have some correctness constrains in unrolling a multi-exit
  // loop. Check for these below.

  // We rely on LCSSA form being preserved when the exit blocks are transformed.
  if (!PreserveLCSSA)
    return false;
  SmallVector<BasicBlock *, 4> Exits;
  L->getUniqueExitBlocks(Exits);
  for (auto *BB : Exits)
    if (BB != LatchExit)
      OtherExits.push_back(BB);

  // TODO: Support multiple exiting blocks jumping to the `LatchExit` when
  // UnrollRuntimeMultiExit is true. This will need updating the logic in
  // connectEpilog/connectProlog.
  if (!LatchExit->getSinglePredecessor()) {
    (errs() << "Bailout for multi-exit handling when latch exit has >1 "
                    "predecessor.\n");
    return false;
  }
  // FIXME: We bail out of multi-exit unrolling when epilog loop is generated
  // and L is an inner loop. This is because in presence of multiple exits, the
  // outer loop is incorrect: we do not add the EpilogPreheader and exit to the
  // outer loop. This is automatically handled in the prolog case, so we do not
  // have that bug in prolog generation.
  if (UseEpilogRemainder && L->getParentLoop())
    return false;

  // All constraints have been satisfied.
  return true;
}

/// Returns true if we can profitably unroll the multi-exit loop L. Currently,
/// we return true only if UnrollRuntimeMultiExit is set to true.
static bool canProfitablyUnrollMultiExitLoop(
    Loop *L, SmallVectorImpl<BasicBlock *> &OtherExits, BasicBlock *LatchExit,
    bool PreserveLCSSA, bool UseEpilogRemainder) {

  SmallVector<BasicBlock *, 8> OtherExitsDummyCheck;
  assert(canSafelyUnrollMultiExitLoop(L, OtherExitsDummyCheck, LatchExit,
                                      PreserveLCSSA, UseEpilogRemainder) &&
         "Should be safe to unroll before checking profitability!");

  // Priority goes to UnrollRuntimeMultiExit if it's supplied.
//   if (UnrollRuntimeMultiExit.getNumOccurrences())
//     return UnrollRuntimeMultiExit;

  // The main pain point with multi-exit loop unrolling is that once unrolled,
  // we will not be able to merge all blocks into a straight line code.
  // There are branches within the unrolled loop that go to the OtherExits.
  // The second point is the increase in code size, but this is true
  // irrespective of multiple exits.

  // Note: Both the heuristics below are coarse grained. We are essentially
  // enabling unrolling of loops that have a single side exit other than the
  // normal LatchExit (i.e. exiting into a deoptimize block).
  // The heuristics considered are:
  // 1. low number of branches in the unrolled version.
  // 2. high predictability of these extra branches.
  // We avoid unrolling loops that have more than two exiting blocks. This
  // limits the total number of branches in the unrolled loop to be atmost
  // the unroll factor (since one of the exiting blocks is the latch block).
  SmallVector<BasicBlock*, 4> ExitingBlocks;
  L->getExitingBlocks(ExitingBlocks);
  if (ExitingBlocks.size() > 2)
    return false;

  // The second heuristic is that L has one exit other than the latchexit and
  // that exit is a deoptimize block. We know that deoptimize blocks are rarely
  // taken, which also implies the branch leading to the deoptimize block is
  // highly predictable.
  return (OtherExits.size() == 1 &&
          OtherExits[0]->getTerminatingDeoptimizeCall());
  // TODO: These can be fine-tuned further to consider code size or deopt states
  // that are captured by the deoptimize exit block.
  // Also, we can extend this to support more cases, if we actually
  // know of kinds of multiexit loops that would benefit from unrolling.
}

/// Insert code in the prolog/epilog code when unrolling a loop with a
/// run-time trip-count.
///
/// This method assumes that the loop unroll factor is total number
/// of loop bodies in the loop after unrolling. (Some folks refer
/// to the unroll factor as the number of *extra* copies added).
/// We assume also that the loop unroll factor is a power-of-two. So, after
/// unrolling the loop, the number of loop bodies executed is 2,
/// 4, 8, etc.  Note - LLVM converts the if-then-sequence to a switch
/// instruction in SimplifyCFG.cpp.  Then, the backend decides how code for
/// the switch instruction is generated.
///
/// ***Prolog case***
///        extraiters = tripcount % loopfactor
///        if (extraiters == 0) jump Loop:
///        else jump Prol:
/// Prol:  LoopBody;
///        extraiters -= 1                 // Omitted if unroll factor is 2.
///        if (extraiters != 0) jump Prol: // Omitted if unroll factor is 2.
///        if (tripcount < loopfactor) jump End:
/// Loop:
/// ...
/// End:
///
/// ***Epilog case***
///        extraiters = tripcount % loopfactor
///        if (tripcount < loopfactor) jump LoopExit:
///        unroll_iters = tripcount - extraiters
/// Loop:  LoopBody; (executes unroll_iter times);
///        unroll_iter -= 1
///        if (unroll_iter != 0) jump Loop:
/// LoopExit:
///        if (extraiters == 0) jump EpilExit:
/// Epil:  LoopBody; (executes extraiters times)
///        extraiters -= 1                 // Omitted if unroll factor is 2.
///        if (extraiters != 0) jump Epil: // Omitted if unroll factor is 2.
/// EpilExit:

bool UnrollRuntimeLoopRemainder(Loop *L, unsigned Count,
                                      bool AllowExpensiveTripCount,
                                      bool UseEpilogRemainder,
                                      bool UnrollRemainder,
                                      LoopInfo *LI, ScalarEvolution *SE,
                                      DominatorTree *DT, AssumptionCache *AC,
                                      bool PreserveLCSSA) {
  (errs() << "Trying runtime unrolling on Loop: \n");
  (L->dump());
  (UseEpilogRemainder ? errs() << "Using epilog remainder.\n" :
        errs() << "Using prolog remainder.\n");

  // Make sure the loop is in canonical form.
  if (!L->isLoopSimplifyForm()) {
    (errs() << "Not in simplify form!\n");
    return false;
  }

  // Guaranteed by LoopSimplifyForm.
  BasicBlock *Latch = L->getLoopLatch();
  BasicBlock *Header = L->getHeader();

  BranchInst *LatchBR = cast<BranchInst>(Latch->getTerminator());
  unsigned ExitIndex = LatchBR->getSuccessor(0) == Header ? 1 : 0;
  BasicBlock *LatchExit = LatchBR->getSuccessor(ExitIndex);
  // Cloning the loop basic blocks (`CloneLoopBlocks`) requires that one of the
  // targets of the Latch be an exit block out of the loop. This needs
  // to be guaranteed by the callers of UnrollRuntimeLoopRemainder.
  assert(!L->contains(LatchExit) &&
         "one of the loop latch successors should be the exit block!");
  // These are exit blocks other than the target of the latch exiting block.
  SmallVector<BasicBlock *, 4> OtherExits;
  bool isMultiExitUnrollingEnabled =
      canSafelyUnrollMultiExitLoop(L, OtherExits, LatchExit, PreserveLCSSA,
                                   UseEpilogRemainder) &&
      canProfitablyUnrollMultiExitLoop(L, OtherExits, LatchExit, PreserveLCSSA,
                                       UseEpilogRemainder);
  // Support only single exit and exiting block unless multi-exit loop unrolling is enabled.
  if (!isMultiExitUnrollingEnabled &&
      (!L->getExitingBlock() || OtherExits.size())) {
    (
        errs()
        << "Multiple exit/exiting blocks in loop and multi-exit unrolling not "
           "enabled!\n");
    return false;
  }
  // Use Scalar Evolution to compute the trip count. This allows more loops to
  // be unrolled than relying on induction var simplification.
  if (!SE)
    return false;

  // Only unroll loops with a computable trip count, and the trip count needs
  // to be an int value (allowing a pointer type is a TODO item).
  // We calculate the backedge count by using getExitCount on the Latch block,
  // which is proven to be the only exiting block in this loop. This is same as
  // calculating getBackedgeTakenCount on the loop (which computes SCEV for all
  // exiting blocks).
  const SCEV *BECountSC = SE->getExitCount(L, Latch);
  if (isa<SCEVCouldNotCompute>(BECountSC) ||
      !BECountSC->getType()->isIntegerTy()) {
    (errs() << "Could not compute exit block SCEV\n");
    return false;
  }

  unsigned BEWidth = cast<IntegerType>(BECountSC->getType())->getBitWidth();

  // Add 1 since the backedge count doesn't include the first loop iteration.
  const SCEV *TripCountSC =
      SE->getAddExpr(BECountSC, SE->getConstant(BECountSC->getType(), 1));
  if (isa<SCEVCouldNotCompute>(TripCountSC)) {
    (errs() << "Could not compute trip count SCEV.\n");
    return false;
  }

  BasicBlock *PreHeader = L->getLoopPreheader();
  BranchInst *PreHeaderBR = cast<BranchInst>(PreHeader->getTerminator());
  const DataLayout &DL = Header->getModule()->getDataLayout();
  SCEVExpander Expander(*SE, DL, "loop-unroll");
  if (!AllowExpensiveTripCount &&
      Expander.isHighCostExpansion(TripCountSC, L, PreHeaderBR)) {
    (errs() << "High cost for expanding trip count scev!\n");
    return false;
  }

  // This constraint lets us deal with an overflowing trip count easily; see the
  // comment on ModVal below.
  if (Log2_32(Count) > BEWidth) {
    (errs()
          << "Count failed constraint on overflow trip count calculation.\n");
    return false;
  }

  // Loop structure is the following:
  //
  // PreHeader
  //   Header
  //   ...
  //   Latch
  // LatchExit

  BasicBlock *NewPreHeader;
  BasicBlock *NewExit = nullptr;
  BasicBlock *PrologExit = nullptr;
  BasicBlock *EpilogPreHeader = nullptr;
  BasicBlock *PrologPreHeader = nullptr;

  if (UseEpilogRemainder) {
    // If epilog remainder
    // Split PreHeader to insert a branch around loop for unrolling.
    NewPreHeader = SplitBlock(PreHeader, PreHeader->getTerminator(), DT, LI);
    NewPreHeader->setName(PreHeader->getName() + ".new");
    // Split LatchExit to create phi nodes from branch above.
    SmallVector<BasicBlock*, 4> Preds(predecessors(LatchExit));
    NewExit = SplitBlockPredecessors(LatchExit, Preds, ".unr-lcssa",
                                     DT, LI, PreserveLCSSA);
    // NewExit gets its DebugLoc from LatchExit, which is not part of the
    // original Loop.
    // Fix this by setting Loop's DebugLoc to NewExit.
    auto *NewExitTerminator = NewExit->getTerminator();
    NewExitTerminator->setDebugLoc(Header->getTerminator()->getDebugLoc());
    // Split NewExit to insert epilog remainder loop.
    EpilogPreHeader = SplitBlock(NewExit, NewExitTerminator, DT, LI);
    EpilogPreHeader->setName(Header->getName() + ".epil.preheader");
  } else {
    // If prolog remainder
    // Split the original preheader twice to insert prolog remainder loop
    PrologPreHeader = SplitEdge(PreHeader, Header, DT, LI);
    PrologPreHeader->setName(Header->getName() + ".prol.preheader");
    PrologExit = SplitBlock(PrologPreHeader, PrologPreHeader->getTerminator(),
                            DT, LI);
    PrologExit->setName(Header->getName() + ".prol.loopexit");
    // Split PrologExit to get NewPreHeader.
    NewPreHeader = SplitBlock(PrologExit, PrologExit->getTerminator(), DT, LI);
    NewPreHeader->setName(PreHeader->getName() + ".new");
  }
  // Loop structure should be the following:
  //  Epilog             Prolog
  //
  // PreHeader         PreHeader
  // *NewPreHeader     *PrologPreHeader
  //   Header          *PrologExit
  //   ...             *NewPreHeader
  //   Latch             Header
  // *NewExit            ...
  // *EpilogPreHeader    Latch
  // LatchExit              LatchExit

  // Calculate conditions for branch around loop for unrolling
  // in epilog case and around prolog remainder loop in prolog case.
  // Compute the number of extra iterations required, which is:
  //  extra iterations = run-time trip count % loop unroll factor
  PreHeaderBR = cast<BranchInst>(PreHeader->getTerminator());
  Value *TripCount = Expander.expandCodeFor(TripCountSC, TripCountSC->getType(),
                                            PreHeaderBR);
  Value *BECount = Expander.expandCodeFor(BECountSC, BECountSC->getType(),
                                          PreHeaderBR);
  IRBuilder<> B(PreHeaderBR);
  Value *ModVal;
  // Calculate ModVal = (BECount + 1) % Count.
  // Note that TripCount is BECount + 1.
  if (isPowerOf2_32(Count)) {
    // When Count is power of 2 we don't BECount for epilog case, however we'll
    // need it for a branch around unrolling loop for prolog case.
    ModVal = B.CreateAnd(TripCount, Count - 1, "xtraiter");
    //  1. There are no iterations to be run in the prolog/epilog loop.
    // OR
    //  2. The addition computing TripCount overflowed.
    //
    // If (2) is true, we know that TripCount really is (1 << BEWidth) and so
    // the number of iterations that remain to be run in the original loop is a
    // multiple Count == (1 << Log2(Count)) because Log2(Count) <= BEWidth (we
    // explicitly check this above).
  } else {
    // As (BECount + 1) can potentially unsigned overflow we count
    // (BECount % Count) + 1 which is overflow safe as BECount % Count < Count.
    Value *ModValTmp = B.CreateURem(BECount,
                                    ConstantInt::get(BECount->getType(),
                                                     Count));
    Value *ModValAdd = B.CreateAdd(ModValTmp,
                                   ConstantInt::get(ModValTmp->getType(), 1));
    // At that point (BECount % Count) + 1 could be equal to Count.
    // To handle this case we need to take mod by Count one more time.
    ModVal = B.CreateURem(ModValAdd,
                          ConstantInt::get(BECount->getType(), Count),
                          "xtraiter");
  }
  Value *BranchVal =
      UseEpilogRemainder ? B.CreateICmpULT(BECount,
                                           ConstantInt::get(BECount->getType(),
                                                            Count - 1)) :
                           B.CreateIsNotNull(ModVal, "lcmp.mod");
  BasicBlock *RemainderLoop = UseEpilogRemainder ? NewExit : PrologPreHeader;
  BasicBlock *UnrollingLoop = UseEpilogRemainder ? NewPreHeader : PrologExit;
  // Branch to either remainder (extra iterations) loop or unrolling loop.
  B.CreateCondBr(BranchVal, RemainderLoop, UnrollingLoop);
  PreHeaderBR->eraseFromParent();
  if (DT) {
    if (UseEpilogRemainder)
      DT->changeImmediateDominator(NewExit, PreHeader);
    else
      DT->changeImmediateDominator(PrologExit, PreHeader);
  }
  Function *F = Header->getParent();
  // Get an ordered list of blocks in the loop to help with the ordering of the
  // cloned blocks in the prolog/epilog code
  LoopBlocksDFS LoopBlocks(L);
  LoopBlocks.perform(LI);

  //
  // For each extra loop iteration, create a copy of the loop's basic blocks
  // and generate a condition that branches to the copy depending on the
  // number of 'left over' iterations.
  //
  std::vector<BasicBlock *> NewBlocks;
  ValueToValueMapTy VMap;

  // For unroll factor 2 remainder loop will have 1 iterations.
  // Do not create 1 iteration loop.
  bool CreateRemainderLoop = (Count != 2);

  // Clone all the basic blocks in the loop. If Count is 2, we don't clone
  // the loop, otherwise we create a cloned loop to execute the extra
  // iterations. This function adds the appropriate CFG connections.
  BasicBlock *InsertBot = UseEpilogRemainder ? LatchExit : PrologExit;
  BasicBlock *InsertTop = UseEpilogRemainder ? EpilogPreHeader : PrologPreHeader;
  Loop *remainderLoop = CloneLoopBlocks(
      L, ModVal, CreateRemainderLoop, UseEpilogRemainder, UnrollRemainder,
      InsertTop, InsertBot,
      NewPreHeader, NewBlocks, LoopBlocks, VMap, DT, LI);

  // Insert the cloned blocks into the function.
  F->getBasicBlockList().splice(InsertBot->getIterator(),
                                F->getBasicBlockList(),
                                NewBlocks[0]->getIterator(),
                                F->end());

  // Now the loop blocks are cloned and the other exiting blocks from the
  // remainder are connected to the original Loop's exit blocks. The remaining
  // work is to update the phi nodes in the original loop, and take in the
  // values from the cloned region. Also update the dominator info for
  // OtherExits and their immediate successors, since we have new edges into
  // OtherExits.
  SmallSet<BasicBlock*, 8> ImmediateSuccessorsOfExitBlocks;
  for (auto *BB : OtherExits) {
   for (auto &II : *BB) {

     // Given we preserve LCSSA form, we know that the values used outside the
     // loop will be used through these phi nodes at the exit blocks that are
     // transformed below.
     if (!isa<PHINode>(II))
       break;
     PHINode *Phi = cast<PHINode>(&II);
     unsigned oldNumOperands = Phi->getNumIncomingValues();
     // Add the incoming values from the remainder code to the end of the phi
     // node.
     for (unsigned i =0; i < oldNumOperands; i++){
       Value *newVal = VMap.lookup(Phi->getIncomingValue(i));
       // newVal can be a constant or derived from values outside the loop, and
       // hence need not have a VMap value. Also, since lookup already generated
       // a default "null" VMap entry for this value, we need to populate that
       // VMap entry correctly, with the mapped entry being itself.
       if (!newVal) {
         newVal = Phi->getIncomingValue(i);
         VMap[Phi->getIncomingValue(i)] = Phi->getIncomingValue(i);
       }
       Phi->addIncoming(newVal,
                           cast<BasicBlock>(VMap[Phi->getIncomingBlock(i)]));
     }
   }

   // Update the dominator info because the immediate dominator is no longer the
   // header of the original Loop. BB has edges both from L and remainder code.
   // Since the preheader determines which loop is run (L or directly jump to
   // the remainder code), we set the immediate dominator as the preheader.
   if (DT) {
     DT->changeImmediateDominator(BB, PreHeader);
     // Also update the IDom for immediate successors of BB.  If the current
     // IDom is the header, update the IDom to be the preheader because that is
     // the nearest common dominator of all predecessors of SuccBB.  We need to
     // check for IDom being the header because successors of exit blocks can
     // have edges from outside the loop, and we should not incorrectly update
     // the IDom in that case.
     for (BasicBlock *SuccBB: successors(BB))
       if (ImmediateSuccessorsOfExitBlocks.insert(SuccBB).second) {
         if (DT->getNode(SuccBB)->getIDom()->getBlock() == Header) {
           assert(!SuccBB->getSinglePredecessor() &&
                  "BB should be the IDom then!");
           DT->changeImmediateDominator(SuccBB, PreHeader);
         }
       }
    }
  }

  // Loop structure should be the following:
  //  Epilog             Prolog
  //
  // PreHeader         PreHeader
  // NewPreHeader      PrologPreHeader
  //   Header            PrologHeader
  //   ...               ...
  //   Latch             PrologLatch
  // NewExit           PrologExit
  // EpilogPreHeader   NewPreHeader
  //   EpilogHeader      Header
  //   ...               ...
  //   EpilogLatch       Latch
  // LatchExit              LatchExit

  // Rewrite the cloned instruction operands to use the values created when the
  // clone is created.
  for (BasicBlock *BB : NewBlocks) {
    for (Instruction &I : *BB) {
      RemapInstruction(&I, VMap,
                       RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);
    }
  }

  if (UseEpilogRemainder) {
    // Connect the epilog code to the original loop and update the
    // PHI functions.
    ConnectEpilog(L, ModVal, NewExit, LatchExit, PreHeader,
                  EpilogPreHeader, NewPreHeader, VMap, DT, LI,
                  PreserveLCSSA);

    // Update counter in loop for unrolling.
    // I should be multiply of Count.
    IRBuilder<> B2(NewPreHeader->getTerminator());
    Value *TestVal = B2.CreateSub(TripCount, ModVal, "unroll_iter");
    BranchInst *LatchBR = cast<BranchInst>(Latch->getTerminator());
    B2.SetInsertPoint(LatchBR);
    PHINode *NewIdx = PHINode::Create(TestVal->getType(), 2, "niter",
                                      Header->getFirstNonPHI());
    Value *IdxSub =
        B2.CreateSub(NewIdx, ConstantInt::get(NewIdx->getType(), 1),
                     NewIdx->getName() + ".nsub");
    Value *IdxCmp;
    if (LatchBR->getSuccessor(0) == Header)
      IdxCmp = B2.CreateIsNotNull(IdxSub, NewIdx->getName() + ".ncmp");
    else
      IdxCmp = B2.CreateIsNull(IdxSub, NewIdx->getName() + ".ncmp");
    NewIdx->addIncoming(TestVal, NewPreHeader);
    NewIdx->addIncoming(IdxSub, Latch);
    LatchBR->setCondition(IdxCmp);
  } else {
    // Connect the prolog code to the original loop and update the
    // PHI functions.
    ConnectProlog(L, BECount, Count, PrologExit, LatchExit, PreHeader,
                  NewPreHeader, VMap, DT, LI, PreserveLCSSA);
  }

  // If this loop is nested, then the loop unroller changes the code in the
  // parent loop, so the Scalar Evolution pass needs to be run again.
  if (Loop *ParentLoop = L->getParentLoop())
    SE->forgetLoop(ParentLoop);

  // Canonicalize to LoopSimplifyForm both original and remainder loops. We
  // cannot rely on the LoopUnrollPass to do this because it only does
  // canonicalization for parent/subloops and not the sibling loops.
  if (OtherExits.size() > 0) {
    // Generate dedicated exit blocks for the original loop, to preserve
    // LoopSimplifyForm.
    formDedicatedExitBlocks(L, DT, LI, PreserveLCSSA);
    // Generate dedicated exit blocks for the remainder loop if one exists, to
    // preserve LoopSimplifyForm.
    if (remainderLoop)
      formDedicatedExitBlocks(remainderLoop, DT, LI, PreserveLCSSA);
  }

  if (remainderLoop && UnrollRemainder) {
    (errs() << "Unrolling remainder loop\n");
    UnrollLoop(remainderLoop, /*Count*/ Count - 1, /*TripCount*/ Count - 1,
               /*Force*/ false, /*AllowRuntime*/ false,
               /*AllowExpensiveTripCount*/ false, /*PreserveCondBr*/ true,
               /*PreserveOnlyFirst*/ false, /*TripMultiple*/ 1,
               /*PeelCount*/ 0, /*UnrollRemainder*/ false, LI, SE, DT, AC,
               /*TI*/ nullptr, /*ORE*/ nullptr, PreserveLCSSA);
  }

  NumRuntimeUnrolled++;
  return true;
}

/// Unroll the given loop by Count. The loop must be in LCSSA form.  Unrolling
/// can only fail when the loop's latch block is not terminated by a conditional
/// branch instruction. However, if the trip count (and multiple) are not known,
/// loop unrolling will mostly produce more code that is no faster.
///
/// TripCount is the upper bound of the iteration on which control exits
/// LatchBlock. Control may exit the loop prior to TripCount iterations either
/// via an early branch in other loop block or via LatchBlock terminator. This
/// is relaxed from the general definition of trip count which is the number of
/// times the loop header executes. Note that UnrollLoop assumes that the loop
/// counter test is in LatchBlock in order to remove unnecesssary instances of
/// the test.  If control can exit the loop from the LatchBlock's terminator
/// prior to TripCount iterations, flag PreserveCondBr needs to be set.
///
/// PreserveCondBr indicates whether the conditional branch of the LatchBlock
/// needs to be preserved.  It is needed when we use trip count upper bound to
/// fully unroll the loop. If PreserveOnlyFirst is also set then only the first
/// conditional branch needs to be preserved.
///
/// Similarly, TripMultiple divides the number of times that the LatchBlock may
/// execute without exiting the loop.
///
/// If AllowRuntime is true then UnrollLoop will consider unrolling loops that
/// have a runtime (i.e. not compile time constant) trip count.  Unrolling these
/// loops require a unroll "prologue" that runs "RuntimeTripCount % Count"
/// iterations before branching into the unrolled loop.  UnrollLoop will not
/// runtime-unroll the loop if computing RuntimeTripCount will be expensive and
/// AllowExpensiveTripCount is false.
///
/// If we want to perform PGO-based loop peeling, PeelCount is set to the
/// number of iterations we want to peel off.
///
/// The LoopInfo Analysis that is passed will be kept consistent.
///
/// This utility preserves LoopInfo. It will also preserve ScalarEvolution and
/// DominatorTree if they are non-null.
Result llvm::UnrollLoop(
    Loop *L, unsigned Count, unsigned TripCount, bool Force, bool AllowRuntime,
    bool AllowExpensiveTripCount, bool PreserveCondBr, bool PreserveOnlyFirst,
    unsigned TripMultiple, unsigned PeelCount, bool UnrollRemainder,
    LoopInfo *LI, ScalarEvolution *SE, DominatorTree *DT, AssumptionCache *AC,
    TaskInfo *TI, OptimizationRemarkEmitter *ORE, MemorySSAUpdater *MSSAU, bool PreserveLCSSA) {

  BasicBlock *Preheader = L->getLoopPreheader();
  if (!Preheader) {
    (errs() << "  Can't unroll; loop preheader-insertion failed.\n");
    return Result::Unmodified;
  }

  BasicBlock *LatchBlock = L->getLoopLatch();
  if (!LatchBlock) {
    (errs() << "  Can't unroll; loop exit-block-insertion failed.\n");
    return Result::Unmodified;
  }

  // Loops with indirectbr cannot be cloned.
  if (!L->isSafeToClone()) {
    (errs() << "  Can't unroll; Loop body cannot be cloned.\n");
    return Result::Unmodified;
  }

  // The current loop unroll pass can only unroll loops with a single latch
  // that's a conditional branch exiting the loop.
  // FIXME: The implementation can be extended to work with more complicated
  // cases, e.g. loops with multiple latches.
  BasicBlock *Header = L->getHeader();
  BranchInst *BI = dyn_cast<BranchInst>(LatchBlock->getTerminator());

  if (!BI || BI->isUnconditional()) {
    // The loop-rotate pass can be helpful to avoid this in many cases.
    (errs() <<
             "  Can't unroll; loop not terminated by a conditional branch.\n");
    return Result::Unmodified;
  }

  auto CheckSuccessors = [&](unsigned S1, unsigned S2) {
    return BI->getSuccessor(S1) == Header && !L->contains(BI->getSuccessor(S2));
  };

  if (!CheckSuccessors(0, 1) && !CheckSuccessors(1, 0)) {
    (errs() << "Can't unroll; only loops with one conditional latch"
                    " exiting the loop can be unrolled\n");
    return Result::Unmodified;
  }

  if (Header->hasAddressTaken()) {
    // The loop-rotate pass can be helpful to avoid this in many cases.
    (errs() <<
          "  Won't unroll loop: address of header block is taken.\n");
    return Result::Unmodified;
  }

  if (TripCount != 0)
    (errs() << "  Trip Count = " << TripCount << "\n");
  if (TripMultiple != 1)
    (errs() << "  Trip Multiple = " << TripMultiple << "\n");

  // Effectively "DCE" unrolled iterations that are beyond the tripcount
  // and will never be executed.
  if (TripCount != 0 && Count > TripCount)
    Count = TripCount;

  // Don't enter the unroll code if there is nothing to do.
  if (TripCount == 0 && Count < 2 && PeelCount == 0) {
    (errs() << "Won't unroll; almost nothing to do\n");
    return Result::Unmodified;
  }

  assert(Count > 0);
  assert(TripMultiple > 0);
  assert(TripCount == 0 || TripCount % TripMultiple == 0);

  // Are we eliminating the loop control altogether?
  bool CompletelyUnroll = Count == TripCount;

  // Disallow partial unrolling of Tapir loops.
  // FIXME: Allow partial unrolling of a Tapir loop to produce a new Tapir loop.
  if (getTaskIfTapirLoop(L, TI) && !CompletelyUnroll)
    return Result::Unmodified;

  SmallVector<BasicBlock *, 4> ExitBlocks;
  L->getExitBlocks(ExitBlocks);
  std::vector<BasicBlock*> OriginalLoopBlocks = L->getBlocks();

  // Go through all exits of L and see if there are any phi-nodes there. We just
  // conservatively assume that they're inserted to preserve LCSSA form, which
  // means that complete unrolling might break this form. We need to either fix
  // it in-place after the transformation, or entirely rebuild LCSSA. TODO: For
  // now we just recompute LCSSA for the outer loop, but it should be possible
  // to fix it in-place.
  bool NeedToFixLCSSA = PreserveLCSSA && CompletelyUnroll &&
                        any_of(ExitBlocks, [](const BasicBlock *BB) {
                          return isa<PHINode>(BB->begin());
                        });

  // We assume a run-time trip count if the compiler cannot
  // figure out the loop trip count and the unroll-runtime
  // flag is specified.
  bool RuntimeTripCount = (TripCount == 0 && Count > 0 && AllowRuntime);

  assert((!RuntimeTripCount || !PeelCount) &&
         "Did not expect runtime trip-count unrolling "
         "and peeling for the same loop");

  if (PeelCount) {
    // bool Peeled = peelLoop(L, PeelCount, LI, SE, DT, AC, PreserveLCSSA);

    // // Successful peeling may result in a change in the loop preheader/trip
    // // counts. If we later unroll the loop, we want these to be updated.
    // if (Peeled) {
    //   BasicBlock *ExitingBlock = L->getExitingBlock();
    //   assert(ExitingBlock && "Loop without exiting block?");
    //   Preheader = L->getLoopPreheader();
    //   TripCount = SE->getSmallConstantTripCount(L, ExitingBlock);
    //   TripMultiple = SE->getSmallConstantTripMultiple(L, ExitingBlock);
    // }
  }

  // Loops containing convergent instructions must have a count that divides
  // their TripMultiple.
  (
      {
        bool HasConvergent = false;
        for (auto &BB : L->blocks())
          for (auto &I : *BB)
            if (auto CS = CallSite(&I))
              HasConvergent |= CS.isConvergent();
        assert((!HasConvergent || TripMultiple % Count == 0) &&
               "Unroll count must divide trip multiple if loop contains a "
               "convergent operation.");
      });

  bool EpilogProfitability = isEpilogProfitable(L);

  if (RuntimeTripCount && TripMultiple % Count != 0 &&
      !UnrollRuntimeLoopRemainder(L, Count, AllowExpensiveTripCount,
                                  EpilogProfitability, UnrollRemainder, LI, SE,
                                  DT, AC, PreserveLCSSA)) {
    if (Force)
      RuntimeTripCount = false;
    else {
      (
          errs() << "Wont unroll; remainder loop could not be generated"
                    "when assuming runtime trip count\n");
      return Result::Unmodified;
    }
  }

  // Notify ScalarEvolution that the loop will be substantially changed,
  // if not outright eliminated.
  if (SE)
    SE->forgetLoop(L);

  // If we know the trip count, we know the multiple...
  unsigned BreakoutTrip = 0;
  if (TripCount != 0) {
    BreakoutTrip = TripCount % Count;
    TripMultiple = 0;
  } else {
    // Figure out what multiple to use.
    BreakoutTrip = TripMultiple =
      (unsigned)GreatestCommonDivisor64(Count, TripMultiple);
  }

  using namespace ore;
  // Report the unrolling decision.
  if (CompletelyUnroll) {
    (errs() << "COMPLETELY UNROLLING loop %" << Header->getName()
                 << " with trip count " << TripCount << "!\n");
    
  } else if (PeelCount) {
    (errs() << "PEELING loop %" << Header->getName()
                 << " with iteration count " << PeelCount << "!\n");
    
  } else {
    auto DiagBuilder = [&]() {
      OptimizationRemark Diag(DEBUG_TYPE, "PartialUnrolled", L->getStartLoc(),
                              L->getHeader());
      return Diag << "unrolled loop by a factor of "
                  << NV("UnrollCount", Count);
    };

    (errs() << "UNROLLING loop %" << Header->getName()
          << " by " << Count);
    if (TripMultiple == 0 || BreakoutTrip != TripMultiple) {
      (errs() << " with a breakout at trip " << BreakoutTrip);
      
    } else if (TripMultiple != 1) {
      (errs() << " with " << TripMultiple << " trips per branch");
      
    } else if (RuntimeTripCount) {
      (errs() << " with run-time trip count");
      
    }
    (errs() << "!\n");
  }

  bool ContinueOnTrue = L->contains(BI->getSuccessor(0));
  BasicBlock *LoopExit = BI->getSuccessor(ContinueOnTrue);

  // For the first iteration of the loop, we should use the precloned values for
  // PHI nodes.  Insert associations now.
  ValueToValueMapTy LastValueMap;
  std::vector<PHINode*> OrigPHINode;
  for (BasicBlock::iterator I = Header->begin(); isa<PHINode>(I); ++I) {
    OrigPHINode.push_back(cast<PHINode>(I));
  }

  std::vector<BasicBlock*> Headers;
  std::vector<BasicBlock*> Latches;
  Headers.push_back(Header);
  Latches.push_back(LatchBlock);

  // The current on-the-fly SSA update requires blocks to be processed in
  // reverse postorder so that LastValueMap contains the correct value at each
  // exit.
  LoopBlocksDFS DFS(L);
  DFS.perform(LI);

  // Stash the DFS iterators before adding blocks to the loop.
  LoopBlocksDFS::RPOIterator BlockBegin = DFS.beginRPO();
  LoopBlocksDFS::RPOIterator BlockEnd = DFS.endRPO();

  std::vector<BasicBlock*> UnrolledLoopBlocks = L->getBlocks();

  // Loop Unrolling might create new loops. While we do preserve LoopInfo, we
  // might break loop-simplified form for these loops (as they, e.g., would
  // share the same exit blocks). We'll keep track of loops for which we can
  // break this so that later we can re-simplify them.
  SmallSetVector<Loop *, 4> LoopsToSimplify;
  for (Loop *SubLoop : *L)
    LoopsToSimplify.insert(SubLoop);

  if (Header->getParent()->isDebugInfoForProfiling())
    for (BasicBlock *BB : L->getBlocks())
      for (Instruction &I : *BB)
        if (!isa<DbgInfoIntrinsic>(&I))
          if (const DILocation *DIL = I.getDebugLoc())
            I.setDebugLoc(DIL->cloneWithDuplicationFactor(Count));

  for (unsigned It = 1; It != Count; ++It) {
    std::vector<BasicBlock*> NewBlocks;
    SmallDenseMap<const Loop *, Loop *, 4> NewLoops;
    NewLoops[L] = L;

    for (LoopBlocksDFS::RPOIterator BB = BlockBegin; BB != BlockEnd; ++BB) {
      ValueToValueMapTy VMap;
      BasicBlock *New = CloneBasicBlock(*BB, VMap, "." + Twine(It));
      Header->getParent()->getBasicBlockList().push_back(New);

      assert((*BB != Header || LI->getLoopFor(*BB) == L) &&
             "Header should not be in a sub-loop");
      // Tell LI about New.
      const Loop *OldLoop = addClonedBlockToLoopInfo(*BB, New, LI, NewLoops);
      if (OldLoop) {
        LoopsToSimplify.insert(NewLoops[OldLoop]);

        // Forget the old loop, since its inputs may have changed.
        if (SE)
          SE->forgetLoop(OldLoop);
      }

      if (*BB == Header)
        // Loop over all of the PHI nodes in the block, changing them to use
        // the incoming values from the previous block.
        for (PHINode *OrigPHI : OrigPHINode) {
          PHINode *NewPHI = cast<PHINode>(VMap[OrigPHI]);
          Value *InVal = NewPHI->getIncomingValueForBlock(LatchBlock);
          if (Instruction *InValI = dyn_cast<Instruction>(InVal))
            if (It > 1 && L->contains(InValI))
              InVal = LastValueMap[InValI];
          VMap[OrigPHI] = InVal;
          New->getInstList().erase(NewPHI);
        }

      // Update our running map of newest clones
      LastValueMap[*BB] = New;
      for (ValueToValueMapTy::iterator VI = VMap.begin(), VE = VMap.end();
           VI != VE; ++VI)
        LastValueMap[VI->first] = VI->second;

      // Add phi entries for newly created values to all exit blocks.
      for (BasicBlock *Succ : successors(*BB)) {
        if (L->contains(Succ))
          continue;
        for (PHINode &PHI : Succ->phis()) {
          Value *Incoming = PHI.getIncomingValueForBlock(*BB);
          ValueToValueMapTy::iterator It = LastValueMap.find(Incoming);
          if (It != LastValueMap.end())
            Incoming = It->second;
          PHI.addIncoming(Incoming, New);
        }
      }
      // Keep track of new headers and latches as we create them, so that
      // we can insert the proper branches later.
      if (*BB == Header)
        Headers.push_back(New);
      if (*BB == LatchBlock)
        Latches.push_back(New);

      NewBlocks.push_back(New);
      UnrolledLoopBlocks.push_back(New);

      // Update DomTree: since we just copy the loop body, and each copy has a
      // dedicated entry block (copy of the header block), this header's copy
      // dominates all copied blocks. That means, dominance relations in the
      // copied body are the same as in the original body.
      if (DT) {
        if (*BB == Header)
          DT->addNewBlock(New, Latches[It - 1]);
        else {
          auto BBDomNode = DT->getNode(*BB);
          auto BBIDom = BBDomNode->getIDom();
          BasicBlock *OriginalBBIDom = BBIDom->getBlock();
          DT->addNewBlock(
              New, cast<BasicBlock>(LastValueMap[cast<Value>(OriginalBBIDom)]));
        }
      }
    }

    // Remap all instructions in the most recent iteration
    for (BasicBlock *NewBlock : NewBlocks) {
      for (Instruction &I : *NewBlock) {
        ::remapInstruction(&I, LastValueMap);
        if (auto *II = dyn_cast<IntrinsicInst>(&I))
          if (II->getIntrinsicID() == Intrinsic::assume)
            AC->registerAssumption(II);
      }
    }
  }

  // Loop over the PHI nodes in the original block, setting incoming values.
  for (PHINode *PN : OrigPHINode) {
    if (CompletelyUnroll) {
      PN->replaceAllUsesWith(PN->getIncomingValueForBlock(Preheader));
      Header->getInstList().erase(PN);
    }
    else if (Count > 1) {
      Value *InVal = PN->removeIncomingValue(LatchBlock, false);
      // If this value was defined in the loop, take the value defined by the
      // last iteration of the loop.
      if (Instruction *InValI = dyn_cast<Instruction>(InVal)) {
        if (L->contains(InValI))
          InVal = LastValueMap[InVal];
      }
      assert(Latches.back() == LastValueMap[LatchBlock] && "bad last latch");
      PN->addIncoming(InVal, Latches.back());
    }
  }

  // Now that all the basic blocks for the unrolled iterations are in place,
  // set up the branches to connect them.
  for (unsigned i = 0, e = Latches.size(); i != e; ++i) {
    // The original branch was replicated in each unrolled iteration.
    BranchInst *Term = cast<BranchInst>(Latches[i]->getTerminator());

    // The branch destination.
    unsigned j = (i + 1) % e;
    BasicBlock *Dest = Headers[j];
    bool NeedConditional = true;

    if (RuntimeTripCount && j != 0) {
      NeedConditional = false;
    }

    // For a complete unroll, make the last iteration end with a branch
    // to the exit block.
    if (CompletelyUnroll) {
      if (j == 0)
        Dest = LoopExit;
      // If using trip count upper bound to completely unroll, we need to keep
      // the conditional branch except the last one because the loop may exit
      // after any iteration.
      assert(NeedConditional &&
             "NeedCondition cannot be modified by both complete "
             "unrolling and runtime unrolling");
      NeedConditional = (PreserveCondBr && j && !(PreserveOnlyFirst && i != 0));
    } else if (j != BreakoutTrip && (TripMultiple == 0 || j % TripMultiple != 0)) {
      // If we know the trip count or a multiple of it, we can safely use an
      // unconditional branch for some iterations.
      NeedConditional = false;
    }

    if (NeedConditional) {
      // Update the conditional branch's successor for the following
      // iteration.
      Term->setSuccessor(!ContinueOnTrue, Dest);
    } else {
      // Remove phi operands at this loop exit
      if (Dest != LoopExit) {
        BasicBlock *BB = Latches[i];
        for (BasicBlock *Succ: successors(BB)) {
          if (Succ == Headers[i])
            continue;
          for (PHINode &Phi : Succ->phis())
            Phi.removeIncomingValue(BB, false);
        }
      }
      // Replace the conditional branch with an unconditional one.
      BranchInst::Create(Dest, Term);
      Term->eraseFromParent();
    }
  }

  // Update dominators of blocks we might reach through exits.
  // Immediate dominator of such block might change, because we add more
  // routes which can lead to the exit: we can now reach it from the copied
  // iterations too.
  if (DT && Count > 1) {
    for (auto *BB : OriginalLoopBlocks) {
      auto *BBDomNode = DT->getNode(BB);
      SmallVector<BasicBlock *, 16> ChildrenToUpdate;
      for (auto *ChildDomNode : BBDomNode->getChildren()) {
        auto *ChildBB = ChildDomNode->getBlock();
        if (!L->contains(ChildBB))
          ChildrenToUpdate.push_back(ChildBB);
      }
      BasicBlock *NewIDom;
      if (BB == LatchBlock) {
        // The latch is special because we emit unconditional branches in
        // some cases where the original loop contained a conditional branch.
        // Since the latch is always at the bottom of the loop, if the latch
        // dominated an exit before unrolling, the new dominator of that exit
        // must also be a latch.  Specifically, the dominator is the first
        // latch which ends in a conditional branch, or the last latch if
        // there is no such latch.
        NewIDom = Latches.back();
        for (BasicBlock *IterLatch : Latches) {
          TerminatorInst *Term = IterLatch->getTerminator();
          if (isa<BranchInst>(Term) && cast<BranchInst>(Term)->isConditional()) {
            NewIDom = IterLatch;
            break;
          }
        }
      } else {
        // The new idom of the block will be the nearest common dominator
        // of all copies of the previous idom. This is equivalent to the
        // nearest common dominator of the previous idom and the first latch,
        // which dominates all copies of the previous idom.
        NewIDom = DT->findNearestCommonDominator(BB, LatchBlock);
      }
      for (auto *ChildBB : ChildrenToUpdate)
        DT->changeImmediateDominator(ChildBB, NewIDom);
    }
  }

  if (DT) // && UnrollVerifyDomtree 
    DT->verifyDomTree();

  // Merge adjacent basic blocks, if possible.
  SmallPtrSet<Loop *, 4> ForgottenLoops;
  for (BasicBlock *Latch : Latches) {
    BranchInst *Term = cast<BranchInst>(Latch->getTerminator());
    if (Term->isUnconditional()) {
      BasicBlock *Dest = Term->getSuccessor(0);
      if (BasicBlock *Fold =
              foldBlockIntoPredecessor(Dest, LI, SE, ForgottenLoops, DT)) {
        // Dest has been folded into Fold. Update our worklists accordingly.
        std::replace(Latches.begin(), Latches.end(), Dest, Fold);
        UnrolledLoopBlocks.erase(std::remove(UnrolledLoopBlocks.begin(),
                                             UnrolledLoopBlocks.end(), Dest),
                                 UnrolledLoopBlocks.end());
      }
    }
  }

  // Simplify any new induction variables in the partially unrolled loop.
  if (SE && !CompletelyUnroll && Count > 1) {
    SmallVector<WeakTrackingVH, 16> DeadInsts;
    simplifyLoopIVs(L, SE, DT, LI, TTI, DeadInsts);

    // Aggressively clean up dead instructions that simplifyLoopIVs already
    // identified. Any remaining should be cleaned up below.
    while (!DeadInsts.empty())
      if (Instruction *Inst =
              dyn_cast_or_null<Instruction>(&*DeadInsts.pop_back_val()))
        RecursivelyDeleteTriviallyDeadInstructions(Inst);
  }

  // At this point, the code is well formed.  We now do a quick sweep over the
  // inserted code, doing constant propagation and dead code elimination as we
  // go.
  const DataLayout &DL = Header->getModule()->getDataLayout();
  const std::vector<BasicBlock*> &NewLoopBlocks = L->getBlocks();
  for (BasicBlock *BB : NewLoopBlocks) {
    for (BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E; ) {
      Instruction *Inst = &*I++;

      if (Value *V = SimplifyInstruction(Inst, {DL, nullptr, DT, AC}))
        if (LI->replacementPreservesLCSSAForm(Inst, V))
          Inst->replaceAllUsesWith(V);
      if (isInstructionTriviallyDead(Inst))
        BB->getInstList().erase(Inst);
    }
  }

  // TODO: after peeling or unrolling, previously loop variant conditions are
  // likely to fold to constants, eagerly propagating those here will require
  // fewer cleanup passes to be run.  Alternatively, a LoopEarlyCSE might be
  // appropriate.

  NumCompletelyUnrolled += CompletelyUnroll;
  ++NumUnrolled;

  Loop *OuterL = L->getParentLoop();
  // Update LoopInfo if the loop is completely removed.
  if (CompletelyUnroll)
    LI->erase(L);

  // After complete unrolling most of the blocks should be contained in OuterL.
  // However, some of them might happen to be out of OuterL (e.g. if they
  // precede a loop exit). In this case we might need to insert PHI nodes in
  // order to preserve LCSSA form.
  // We don't need to check this if we already know that we need to fix LCSSA
  // form.
  // TODO: For now we just recompute LCSSA for the outer loop in this case, but
  // it should be possible to fix it in-place.
  if (PreserveLCSSA && OuterL && CompletelyUnroll && !NeedToFixLCSSA)
    NeedToFixLCSSA |= ::needToInsertPhisForLCSSA(OuterL, UnrolledLoopBlocks, LI);

  // If we have a pass and a DominatorTree we should re-simplify impacted loops
  // to ensure subsequent analyses can rely on this form. We want to simplify
  // at least one layer outside of the loop that was unrolled so that any
  // changes to the parent loop exposed by the unrolling are considered.
  if (DT) {
    if (OuterL) {
      // OuterL includes all loops for which we can break loop-simplify, so
      // it's sufficient to simplify only it (it'll recursively simplify inner
      // loops too).
      if (NeedToFixLCSSA) {
        // LCSSA must be performed on the outermost affected loop. The unrolled
        // loop's last loop latch is guaranteed to be in the outermost loop
        // after LoopInfo's been updated by LoopInfo::erase.
        Loop *LatchLoop = LI->getLoopFor(Latches.back());
        Loop *FixLCSSALoop = OuterL;
        if (!FixLCSSALoop->contains(LatchLoop))
          while (FixLCSSALoop->getParentLoop() != LatchLoop)
            FixLCSSALoop = FixLCSSALoop->getParentLoop();

        formLCSSARecursively(*FixLCSSALoop, *DT, LI, SE);
      } else if (PreserveLCSSA) {
        assert(OuterL->isLCSSAForm(*DT) &&
               "Loops should be in LCSSA form after loop-unroll.");
      }

      // TODO: That potentially might be compile-time expensive. We should try
      // to fix the loop-simplified form incrementally.
      simplifyLoop(OuterL, DT, LI, SE, AC, MSSAU, PreserveLCSSA);
    } else {
      // Simplify loops for which we might've broken loop-simplify form.
      for (Loop *SubLoop : LoopsToSimplify)
        simplifyLoop(SubLoop, DT, LI, SE, AC, MSSAU, PreserveLCSSA);
    }

    // Update TaskInfo manually using the updated DT.
    if (TI)
      // FIXME: Recalculating TaskInfo for the whole function is wasteful.
      // Optimize this routine in the future.
      TI->recalculate(*Header->getParent(), *DT);
  }

  return CompletelyUnroll ? Result::FullyUnrolled
                          : Result::PartiallyUnrolled;
}
