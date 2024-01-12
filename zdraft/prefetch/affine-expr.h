#ifndef AFFINE_EXPR_H
#define AFFINE_EXPR_H

#include "utils.h"
// llvm function, pass, const, iterators
#include "llvm/IR/Constants.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
// std lib 
#include <vector>
#include <unordered_map>
#include <unordered_set>
// cpp eigen library for linear algebra
#include <Eigen/Dense>

using namespace llvm;

/*= prototypes ====================================*/
struct AffineExpr;
struct AffineExprSimple;

std::string AffineExprToString(AffineExpr *AE);

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
    virtual std::unique_ptr<AffineExprSimple> Simplify(
        std::vector<Loop *> LoopOrder, 
        std::unordered_map<Value *, Loop *> Indvar2Loop, 
        std::unordered_map<Loop *, IndvarInfo *> Loop2Indvar) const = 0;
    virtual std::string getName() const = 0;
};

struct AffineExprConst : public AffineExpr {
    ConstantInt *Const;
    AffineExprConst(ConstantInt *Const) : Const(Const) {};
    std::unique_ptr<AffineExprSimple> Simplify(
        std::vector<Loop *> LoopOrder,
        std::unordered_map<Value *, Loop *> Indvar2Loop, 
        std::unordered_map<Loop *, IndvarInfo *> Loop2Indvar
    ) const override;
    std::string getName() const override;
    AffineExprType getType() const override;
};

struct AffineExprIndVar : public AffineExpr {
private:
    LLVMContext &context;
public:
    Value *Indvar;
    Loop *AssociatedLoop; 
    AffineExprIndVar(Value *Indvar, Loop *AssociatedLoop, LLVMContext &context) : Indvar(Indvar), AssociatedLoop(AssociatedLoop), context(context) {}

    std::unique_ptr<AffineExprSimple> Simplify(std::vector<Loop *> LoopOrder, std::unordered_map<Value *, Loop *> Indvar2Loop, std::unordered_map<Loop *, IndvarInfo *> Loop2Indvar) const override;
    std::string getName() const override;
    AffineExprType getType() const override;
};

struct AffineExprMul : public AffineExpr {
    ConstantInt *Const;
    std::unique_ptr<AffineExpr> AE;
    AffineExprMul(ConstantInt *Const, std::unique_ptr<AffineExpr> AE) : 
        Const(Const), AE(std::move(AE)) {}

    std::unique_ptr<AffineExprSimple> Simplify(
        std::vector<Loop *> LoopOrder, 
        std::unordered_map<Value *, Loop *> Indvar2Loop, 
        std::unordered_map<Loop *, IndvarInfo *> Loop2Indvar
    ) const override;

    std::string getName() const override;
    AffineExprType getType() const override;
};

struct AffineExprAdd : public AffineExpr {
    std::unique_ptr<AffineExpr> AE1;
    std::unique_ptr<AffineExpr> AE2;
    AffineExprAdd(std::unique_ptr<AffineExpr> AE1, std::unique_ptr<AffineExpr> AE2) : AE1(std::move(AE1)), AE2(std::move(AE2)) {}

    std::unique_ptr<AffineExprSimple> Simplify(
        std::vector<Loop *> LoopOrder, 
        std::unordered_map<Value *, Loop *> Indvar2Loop, 
        std::unordered_map<Loop *, IndvarInfo *> Loop2Indvar
    ) const override;

    std::string getName() const override;
    AffineExprType getType() const override;
};

/*= Affine Expression Simplified representation ============ 
    Const: the left over constant offset
    Coeff: constant coefficient of each induction variable
    (c_d * i_d + ... + c_1 * i_1) + c_0 
*/
struct AffineExprSimple {
    AffineExprSimple(
        ConstantInt *Const, 
        std::vector<Loop *> LoopOrder, 
        std::unordered_map<Value *, Loop *> Indvar2Loop, 
        std::unordered_map<Loop *, IndvarInfo *> Loop2Indvar
        ) 
        : Indvar2Loop(Indvar2Loop), Loop2Indvar(Loop2Indvar), LoopOrder(LoopOrder) {
            Const64 = Const->getSExtValue();

            for (auto it : Indvar2Loop) {
                Value *indvar = it.first;
                Coeffs[indvar] = 0;
            }
    }

    std::vector<Loop *> LoopOrder; // outermost to innermost loop
    std::unordered_map<Value *, Loop *> Indvar2Loop;
    std::unordered_map<Loop *, IndvarInfo *> Loop2Indvar;
    // offset constant
    int64_t Const64;
    // coefficients of induction variable
    std::unordered_map<Value *, int64_t> Coeffs;
    // Equality operator
    friend bool operator==(const AffineExprSimple& lhs, const AffineExprSimple& rhs) {
        return lhs.Const64 == rhs.Const64 && lhs.Coeffs == rhs.Coeffs;
    }
    // helper functions for computing AS
    void addToCoeffs(Value *indvar, Loop *L, int64_t coeff);
    void addToConst(int64_t Delta);
    std::string toString() const;
};

#endif