#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Optional.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopIterator.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Transforms/Utils/ScalarEvolutionExpander.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Analysis/MemorySSA.h"
#include "llvm/Analysis/MemorySSAUpdater.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Constants.h"
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
#include <cassert>
#include <memory>
#include <utility>
#include <Eigen/Dense>

#include "utils.h"
#include "AffineExpr.h"
#include "EigenUtils.h"

using namespace llvm;
using namespace std;
using namespace Eigen;

using AE_t = unique_ptr<AffineExpr>;
using AS_t = AffineExprSimple;
using SubExp_t = vector<AE_t>;
using SubExpS_t = vector<AS_t>;
using LT_t = vector<Loop *>;
using L2Idx_t = unordered_map<Loop *, unsigned>;
using L2Idv_t = unordered_map<Loop *, PHINode *>;
using Idv2L_t = unordered_map<PHINode *, Loop *>;

// AccessPattern
struct AccessPattern {
    static AccessPattern * getAccessPattern(Instruction *MemI, GetElementPtrInst *PtrValue, 
                                        ScalarEvolution *SE, LoopInfo *LI, 
                                        vector<PHINode *> &IVs);
    
    // check whether two access patterns belong to the same group, if so return the fresh one
    bool update(AccessPattern * NewAP, ScalarEvolution &SE);

    SubExpS_t getSubscriptExpr() { return SubExp; };
    void printSubscriptExpr();

    bool isWrite() { return isa<LoadInst>(MemI); }

    unsigned getBasePtrSize() { return BS; }
    const SCEVAddRecExpr *getPtrAR() { return PtrAR; }

    // called after grouping is stablized
    void computeAccessMatrix(LT_t &LT, L2Idx_t &LID, Idv2L_t &IV2L);
    // called after global loop structure is known
    void computeLocality(vector<Loop *> &LocLoops, LT_t &LT, L2Idx_t &LID);
    bool isSpatial(Loop *L, LT_t &LT, L2Idx_t &LID);
    bool isTemporal(Loop *L, LT_t &LT, L2Idx_t &LID);
    void printAccessMatrix();
    void printReuseMatrix();
    uint64_t getCacheLineSize() { return 64; }
    
    // expander
    Value *expandCodeFor(vector<PHINode *> &IVs, unsigned ItersAhead, PHINode *oldIV, 
                        Value *newValue, LoopInfo *LI, ScalarEvolution *SE, 
                        Instruction *InsertBefore);

    AccessPattern(Value *BasePtr, Type *BaseTyp, unsigned BaseSize, 
                Instruction *MemI, GetElementPtrInst *PtrValue,
                const SCEVAddRecExpr *PtrARSCEV, SubExpS_t SubscriptAES) 
        : BP(BasePtr), BT(BaseTyp), BS(BaseSize), PtrV(PtrValue), MemI(MemI), PtrAR(PtrARSCEV),
          SubExp(SubscriptAES) {}

private:
    bool isSameGroup(AccessPattern * NewAP);
    void copyFrom(AccessPattern * NewAP);
private:
    Value *BP;
    Type *BT;
    unsigned BS;
    GetElementPtrInst *PtrV;
    Instruction *MemI;
    const SCEVAddRecExpr *PtrAR;
    SubExpS_t SubExp;

    // compute after grouping stablizes
    MatrixXd AccM;
    VectorXd AccO;
    MatrixXd TReuse;
    vector<VectorXd> TLoc;
    MatrixXd SReuse;
    vector<VectorXd> SLoc;
};

// parsor helper functions 
static unsigned getArrayElementDataSize(Type *ArrayElementDataType, const DataLayout &DL);
static unsigned getElementDataSize(Type *BasePtrElementType, const DataLayout &DL);
static Type *getElementDataType(Type *NestArrayType);

bool parsePtrValue(GetElementPtrInst *gepInst, ScalarEvolution *SE, 
                        LoopInfo *LI, Value **BP, Type ** BT, unsigned &BS, 
                        SubExp_t &SubscriptAE);

AE_t parseIndexInst(Value *indexInst, ScalarEvolution *SE, LoopInfo *LI);

AE_t parseIndexInstBinaryOperator(BinaryOperator *binopI, ScalarEvolution *SE, 
                                LoopInfo *LI);

bool parseAccessPattern(GetElementPtrInst *PtrValue, ScalarEvolution *SE, 
                        LoopInfo *LI, vector<PHINode *> IVs, Value **BP,
                        Type **BT, unsigned &BS, SubExpS_t &SubscriptAE);

// after creating access pattern, use the following utity functions to compute locality
MatrixXd computeLocalizedIterSpace(vector<Loop *> &LocLoops, LT_t &LT, L2Idx_t &LID);
vector<VectorXd> computeLocalizedSpace(MatrixXd Reuse, vector<Loop *> &LocLoops, 
                                        LT_t &LT, L2Idx_t &LID);
#endif