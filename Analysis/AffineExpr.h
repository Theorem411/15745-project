#ifndef AFFINE_EXPR_H
#define AFFINE_EXPR_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/SmallVector.h"
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
// std lib 
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "utils.h"

using namespace llvm;
using namespace std;
/*= prototypes ====================================*/
struct AffineExpr;
struct AffineExprSimple;

string AffineExprToString(AffineExpr *AE);

/*= Affine Expression Recursive representation ============
The Affine Expression should only consist of the following BNF: 
    AE ::= C | indvar | C * AE | AE + AE
*/
/// AffineExpr base class
enum AffineExprType {
    AEConst,
    AEIndvar,
    AEMul,
    AEAdd
};

struct AffineExpr {
    virtual ~AffineExpr() = default;
    virtual AffineExprType getType() const = 0;
    virtual AffineExprSimple Simplify(vector<PHINode *> IVs) const = 0;
    virtual string getName() const = 0;
};

struct AffineExprConst : public AffineExpr {
    ConstantInt *Const;
    AffineExprConst(ConstantInt *Const) : Const(Const) {};
    AffineExprSimple Simplify(vector<PHINode *> IVs) const override;
    string getName() const override;
    AffineExprType getType() const override;
};

struct AffineExprIndVar : public AffineExpr {
    PHINode *Indvar;
    AffineExprIndVar(PHINode *Indvar) : Indvar(Indvar) {}
    AffineExprSimple Simplify(vector<PHINode *> IVs) const override;
    string getName() const override;
    AffineExprType getType() const override;
};

struct AffineExprMul : public AffineExpr {
    ConstantInt *Const;
    unique_ptr<AffineExpr> AE;
    AffineExprMul(ConstantInt *Const, unique_ptr<AffineExpr> AE) : 
        Const(Const), AE(move(AE)) {}

    AffineExprSimple Simplify(vector<PHINode *> IVs) const override;

    string getName() const override;
    AffineExprType getType() const override;
};

struct AffineExprAdd : public AffineExpr {
    unique_ptr<AffineExpr> AE1;
    unique_ptr<AffineExpr> AE2;
    AffineExprAdd(unique_ptr<AffineExpr> AE1, unique_ptr<AffineExpr> AE2) : AE1(move(AE1)), AE2(move(AE2)) {}

    AffineExprSimple Simplify(vector<PHINode *> IVs) const override;

    string getName() const override;
    AffineExprType getType() const override;
};

/*= Affine Expression Simplified representation ============ 
    Const: the left over constant offset
    Coeff: constant coefficient of each induction variable
    (c_d * i_d + ... + c_1 * i_1) + c_0 
*/
struct AffineExprSimple {
    AffineExprSimple(ConstantInt *Const, vector<PHINode *> IVs) : IVs(IVs) {
        Const64 = Const->getSExtValue();

        for (PHINode *indvar : IVs) {
            Coeffs[indvar] = 0;
        }
    }
    // public access function 
    bool getCoeff(PHINode *indvar, int64_t &coeff) const;
    int64_t getConst() const { return Const64; }
    // helper functions for computing AS
    void addToCoeffs(PHINode *indvar, int64_t coeff);
    void mulToCoeffs(int64_t factor);
    void addToConst(int64_t Delta);
    void addFromAS(AffineExprSimple AS);
    string toString() const;
    // Equality operator
    friend bool operator==(const AffineExprSimple& lhs, const AffineExprSimple& rhs) {
        return lhs.Const64 == rhs.Const64 && lhs.Coeffs == rhs.Coeffs;
    }


    private:
    vector<PHINode *> IVs;
    unordered_map<PHINode *, int64_t> Coeffs;
    // offset constant
    int64_t Const64;
};

#endif