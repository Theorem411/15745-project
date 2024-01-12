#include "affine-expr.h"

/*= AffineExpr definition ============*/
/// AffineExprConst
std::unique_ptr<AffineExprSimple> AffineExprConst::Simplify(
    std::vector<Loop *> LoopOrder,
    std::unordered_map<Value *, Loop *> Indvar2Loop, 
    std::unordered_map<Loop *, IndvarInfo *> Loop2Indvar
) const  {
    auto res = std::make_unique<AffineExprSimple>(Const, LoopOrder, Indvar2Loop, Loop2Indvar);
    return res;
}

std::string AffineExprConst::getName() const  { 
    return "AffineExprConst"; 
}

AffineExprType AffineExprConst::getType() const { 
    return AEConst; 
}

/// AffineExprIndVar
std::unique_ptr<AffineExprSimple> AffineExprIndVar::Simplify(
    std::vector<Loop *> LoopOrder, 
    std::unordered_map<Value *, Loop *> Indvar2Loop, 
    std::unordered_map<Loop *, IndvarInfo *> Loop2Indvar
) const  {
    auto AS = std::make_unique<AffineExprSimple>(getConstantInt(context, 0), LoopOrder, Indvar2Loop, Loop2Indvar);
    AS->addToCoeffs(Indvar, AssociatedLoop, 1);
    return AS;
}

std::string AffineExprIndVar::getName() const { 
    return "AffineExprIndvar"; 
}

AffineExprType AffineExprIndVar::getType() const {
    return AEIndvar; 
}

/// AffineExprMul //////////////////
std::unique_ptr<AffineExprSimple> AffineExprMul::Simplify(
    std::vector<Loop *> LoopOrder, 
    std::unordered_map<Value *, Loop *> Indvar2Loop, 
    std::unordered_map<Loop *, IndvarInfo *> Loop2Indvar
) const  {
    auto AS = AE->Simplify(LoopOrder, Indvar2Loop, Loop2Indvar); // std::move(AE->Simplify());
    for (auto &it : AS->Coeffs) { /// remember to use ref of it to update AS->Coeffs correctly
        it.second *= Const->getSExtValue();
    }
    return AS;
}

std::string AffineExprMul::getName() const {
    return "AffineExprMul"; 
}

AffineExprType AffineExprMul::getType() const { 
    return AEMul; 
}

/// AffineExprAdd /////////

std::unique_ptr<AffineExprSimple> AffineExprAdd::Simplify(
    std::vector<Loop *> LoopOrder, 
    std::unordered_map<Value *, Loop *> Indvar2Loop, 
    std::unordered_map<Loop *, IndvarInfo *> Loop2Indvar
) const  {
    std::unique_ptr<AffineExprSimple> AS1 = AE1->Simplify(LoopOrder, Indvar2Loop, Loop2Indvar);
    std::unique_ptr<AffineExprSimple> AS2 = AE2->Simplify(LoopOrder, Indvar2Loop, Loop2Indvar);
    for (auto it : AS2->Coeffs) {
        auto *indvar2 = it.first;
        int64_t coeff2 = it.second;
        Loop *AssociatedLoop2 = AS2->Indvar2Loop[indvar2];
        AS1->addToCoeffs(indvar2, AssociatedLoop2, coeff2);
    }
    AS1->addToConst(AS2->Const64);
    return AS1;
}

std::string AffineExprAdd::getName() const  { 
    return "AffineExprAdd"; 
}
AffineExprType AffineExprAdd::getType() const  { 
    return AEAdd;
}

/// AffineExprToString printing helper 

std::string AffineExprToString(AffineExpr *AE) {
    if (!AE) {
        throw std::runtime_error("What are you doing passing AffineExprToString a nullptr?");
    }
    std::string s;
    raw_string_ostream oss(s);

    switch (AE->getType()) {
        case AEConst: {
            auto *AEC = static_cast<AffineExprConst *>(AE);
            ConstantInt *C = AEC->Const;
            oss << C->getSExtValue();
            break;
        }
        case AEIndvar: {
            auto *AEI = static_cast<AffineExprIndVar *>(AE);
            oss << getShortValueName(AEI->Indvar);
            break;
        }
        case AEAdd: {
            auto *AERes = static_cast<AffineExprAdd *>(AE);
            oss << "(" << AffineExprToString(AERes->AE1.get()) << ")";
            oss << " + ";
            oss << "(" << AffineExprToString(AERes->AE2.get()) << ")";
            break;
        }
        case AEMul: {
            auto *AERes = static_cast<AffineExprMul *>(AE);
            ConstantInt *C = AERes->Const;
            oss << C->getSExtValue();
            oss << " * ";
            oss << "(" << AffineExprToString(AERes->AE.get()) << ")";
            break;
        }
    }

    return s;
}

/*= AffineExprSimple definition ============*/
void AffineExprSimple::addToCoeffs(Value *indvar, Loop *L, int64_t coeff) {
    // update Indvar2Loop
    if (Indvar2Loop.find(indvar) != Indvar2Loop.end()) {
        if (Indvar2Loop[indvar] != L) {
            std::ostringstream oss;
            oss << "Induction variable " << indvar->getName().str() << " has two associated loops!";
            throw std::runtime_error(oss.str());
        }
    }
    Indvar2Loop[indvar] = L;
    // update Coeffs
    if (Coeffs.find(indvar) == Coeffs.end()) {
        Coeffs[indvar] = 0;
    }
    Coeffs[indvar] = Coeffs[indvar] + coeff;
}

void AffineExprSimple::addToConst(int64_t Delta) {
    Const64 += Delta;
}

std::string AffineExprSimple::toString() const {
    std::string s;
    raw_string_ostream strm(s);
    bool first = true;
    for (auto *L : LoopOrder) {
        auto indvarIt = Loop2Indvar.find(L);
.end()) {
            continue;
        }
        Value *indvar = indvarIt->second;

        auto coeffIt = Coeffs.find(indvar);
        if (coeffIt == Coeffs.end()) {
            continue;
        }
        int64_t coeff = coeffIt->second;
        
        if (!first) {
            strm << " + ";
        }
        first = false;
        strm << "(" << coeff << " * " << getShortValueName(indvar) << ")";
    }
    if (!first) {
        strm << " + ";
    }
    strm << "(" << Const64 << ")";

    strm.flush();
    return s;
}