#include "AffineExpr.h"

/*= AffineExpr definition ============*/
/// AffineExprConst
AffineExprSimple AffineExprConst::Simplify(vector<PHINode *> IVs) const {
    return AffineExprSimple(Const, IVs);
}

std::string AffineExprConst::getName() const  { 
    return "AffineExprConst"; 
}

AffineExprType AffineExprConst::getType() const { 
    return AEConst; 
}

/// AffineExprIndVar
AffineExprSimple AffineExprIndVar::Simplify(vector<PHINode *> IVs) const  {
    Function *F = Indvar->getParent()->getParent();
    LLVMContext &context = F->getParent()->getContext();
    AffineExprSimple AS(getConstantInt(context, 0), IVs);
    AS.addToCoeffs(Indvar, 1);
    return AS;
}

std::string AffineExprIndVar::getName() const { 
    return "AffineExprIndvar"; 
}

AffineExprType AffineExprIndVar::getType() const {
    return AEIndvar; 
}

/// AffineExprMul //////////////////
AffineExprSimple AffineExprMul::Simplify(vector<PHINode *> IVs) const  {
    auto AS = AE->Simplify(IVs);
    AS.mulToCoeffs(Const->getSExtValue());
    return AS;
}

std::string AffineExprMul::getName() const {
    return "AffineExprMul"; 
}

AffineExprType AffineExprMul::getType() const { 
    return AEMul; 
}

/// AffineExprAdd /////////

AffineExprSimple AffineExprAdd::Simplify(vector<PHINode *> IVs) const  {
    AffineExprSimple AS1 = AE1->Simplify(IVs);
    AffineExprSimple AS2 = AE2->Simplify(IVs);
    AS1.addFromAS(AS2);
    AS1.addToConst(AS2.getConst());
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
void AffineExprSimple::addToCoeffs(PHINode *indvar, int64_t coeff) {
    // update Coeffs
    if (Coeffs.find(indvar) == Coeffs.end()) {
        Coeffs[indvar] = 0;
    }
    Coeffs[indvar] = Coeffs[indvar] + coeff;
}
void AffineExprSimple::mulToCoeffs(int64_t factor) {
    for (auto &it : Coeffs) { /// remember to use ref of it to update AS.Coeffs correctly
        it.second *= factor;
    }
}
void AffineExprSimple::addToConst(int64_t Delta) {
    Const64 += Delta;
}
void AffineExprSimple::addFromAS(AffineExprSimple AS) {
    for (auto it : AS.Coeffs) {
        auto *indvar = it.first;
        int64_t coeff = it.second;
        addToCoeffs(indvar, coeff);
    }
}
bool AffineExprSimple::getCoeff(PHINode *indvar, int64_t &coeff) const { 
    auto it = Coeffs.find(indvar);
    if (it != Coeffs.end()) {
        coeff = it->second;
        return true;
    } else {
        return false;
    }
}

std::string AffineExprSimple::toString() const {
    std::string s;
    raw_string_ostream strm(s);
    bool first = true;
    for (PHINode *indvar : IVs) {
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