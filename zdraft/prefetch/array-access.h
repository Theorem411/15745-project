#ifndef ARRAY_ACCESS_H
#define ARRAY_ACCESS_H

#include "utils.h"
#include "affine-expr.h"
// llvm function, pass, const, iterators
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/Debug.h"
// llvm scalar evolution
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
// std lib 
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
// cpp eigen library for linear algebra
#include <Eigen/Dense>

using namespace llvm;
using namespace std;

using LT_t = vector<Loop *>;
using L2Idx_t = unordered_map<Loop *, unsigned>;
using L2Idv_t = unordered_map<Loop *, PHINode *>;
using Idv2L_t = unordered_map<PHINode *, Loop *>;
using SubExpr_t = vector<AffineExpr>;
using SubExprS_t = vector<AffineExprSimpl>;

/// GroupAccess
struct AccessPattern {
    AccessPattern(Value *BasePtr, Type *BaseTyp, SubExprS_t SubscriptAES, 
        LT_t LoopTrace, L2Idx_t LoopIndex, L2Idv_t LoopToIndvar, Idv2L_t IndvarToLoop) 
        : BasePtr(BasePtr), BaseTyp(BaseTyp), SubscriptAES(SubscriptAES), 
            LoopTrace(LoopTrace), LoopIndex(LoopIndex)
    {
        computeAccessMatrix();
    }
    private:
    void computeAccessMatrix();
    private:
    bool Write; 

    Value *BasePtr;
    Type *BaseTyp;

    SubExprS_t SubscriptAES;
    size_t SubscriptNum;

    LT_t LoopTrace;
    L2Idx_t LoopIndex;
    size_t LoopNestDepth;

    Eigen::MatrixXd AccessMatrix; // of size SubscriptNum x LoopNestDepth
    Eigen::VectorXd AccessOffset; // of size SubscriptNum
}

struct GroupAccess {
    GroupAccess(Instruction *MemI, GetElementPtrInst *PtrValue, const SCEVAddRecExpr *PtrSCEV,
        Loop *L, LT_t LoopTrace, L2Idx_t LoopIndex, L2Idv_t LoopToIndvar, Idv2L_t IndvarToLoop) 
        : MemI(MemI), PtrValue(PtrValue), PtrSCEV(PtrSCEV),
          L(L), LI(LI), TI(TI), LoopTrace(LoopTrace), 
          LoopIndex(LoopIndex), LoopToIndvar(LoopToIndvar), IndvarToLoop(IndvarToLoop) 
    {   
        LeaderAP = parseAccessPattern(MemI, PtrValue);
        MemberAP.insert({PtrValue, LeaderAP});

        Write = isa<LoadInst>(MemI);
    }
    bool isWrite() { return Write; }
    Value *leadAccess() { return PtrValue; }
    AccessPattern *parseAccessPattern(Instruction *MemI, GetElementPtrInst *PtrValue);
    bool addMember(Instruction *MemI, GetElementPtrInst *PtrValue, const SCEVAddRecExpr *PtrSCEV, AccessPattern *AP);
    
    private:
    bool parsePtrValue(GetElementPtrInst *gepInst, Value *&BasePtr, Type *& BaseTyp, SubExpr_t &SubscriptAE);
    std::unique_ptr<AffineExpr> parseIndexInstBinaryOperator(BinaryOperator *binopI);
    std::unique_ptr<AffineExpr> parseIndexInst(Value *indexInst);
    bool canBeMember(AccessPattern *MemberAP);
    bool isConstPtrDiff(const SCEVAddRecExpr *PtrSCEV);
    unsigned getCacheLineSize() { return 64; }
    bool updateLeaderAP(const SCEVAddRecExpr *PtrSCEV);

    private:
    Loop *L;
    LT_t LoopTrace;
    L2Idx_t LoopIndex;
    L2Idv_t LoopToIndvar;
    Idv2L_t IndvarToLoop;

    // Leader Access Pattern
    bool Write; 
    Instruction *MemI;
    GetElementPtrInst *PtrValue;
    const SCEVAddRecExpr *PtrSCEV;
    AccessPattern *LeaderAP;
    // Member Accesses
    unordered_map<Instruction *, AccessPattern *> MemberAP; 
}



#endif