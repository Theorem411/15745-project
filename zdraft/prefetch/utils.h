#ifndef UTILS_H
#define UTILS_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/LLVMContext.h"

#include "llvm/Analysis/CodeMetrics.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/LoopInfo.h"
// cpp eigen library for linear algebra
#include <Eigen/Dense>
#include <stack>
#include <unordered_set>

using namespace llvm;
using namespace std;


/*= LLVM context & ConstantInt helper =================*/
ConstantInt * getConstantInt(LLVMContext &context, int64_t n);

/*= Induction variable info helper ======================*/
struct IndvarInfo {
    Value *Indvar; // (X)
    Loop *L; // (X)
    Instruction::BinaryOps opcode; // (X)
    Value *Init;
    Value *Limit;
    Value *Stride; // (X)

    IndvarInfo(Value *Indvar, Loop *L, Instruction::BinaryOps opcode, Value *Init, Value *Limit, Value *Stride) 
        : Indvar(Indvar), L(L), opcode(opcode), Init(Init), Limit(Limit), Stride(Stride) {}

    /// DEBUG: verify function?
};

/*= print function for llvm::Value ======================*/
std::string getShortValueName(Value *v);

/*= Eigen helper functions */
std::string printEigenMatrixXd(Eigen::MatrixXd mat, const double threshold);

std::string printEigenVectorXd(Eigen::VectorXd vec, const double threshold);