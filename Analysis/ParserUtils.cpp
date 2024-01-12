#include "ParserUtils.h"

/// locality helper
MatrixXd computeLocalizedIterSpace(vector<Loop *> &LocLoops, LT_t &LT, L2Idx_t &LID) {
    vector<unsigned> LocalizedIndices;
    for (Loop *L : LocLoops) {
        auto it = LID.find(L);
        assert(it != LID.end() && "Localized Loops contain loops not part of the loop trace");
        unsigned id = it->second;
        LocalizedIndices.push_back(id);
    }
    MatrixXd LocalizedSpace = mkIdentityColumns(LocalizedIndices, LT.size());
    return LocalizedSpace;
}

vector<VectorXd> computeLocalizedSpace(MatrixXd Reuse, vector<Loop *> &LocLoops, 
                                        LT_t &LT, L2Idx_t &LID) {
    MatrixXd LocalizedSpace = computeLocalizedIterSpace(LocLoops, LT, LID);
    // errs() << "Localized iter space: \n";
    // std::cout <<  LocalizedSpace << "\n";

    vector<VectorXd> Loc = intersectVectorSpace(Reuse, LocalizedSpace);
    return Loc;
}

//// AccessPattern functions
AccessPattern * AccessPattern::getAccessPattern(Instruction *MemI, GetElementPtrInst *PtrValue, 
                                        ScalarEvolution *SE, LoopInfo *LI,
                                        vector<PHINode *> &IVs) {
    
    Value *BasePtr = nullptr;
    Type *BaseTyp = nullptr;
    unsigned BaseSize;
    SubExpS_t SubscriptAES;
    if (!parseAccessPattern(PtrValue, SE, LI, IVs, &BasePtr, &BaseTyp, BaseSize, SubscriptAES)) {
        return nullptr;
    }
    // errs() << "-- getAccessPattern at ";
    // MemI->print(errs());
    // errs() << " get basesize = " << BaseSize << "\n";

    const SCEV *PtrSCEV = SE->getSCEV(PtrValue);
    const SCEVAddRecExpr *PtrAR = dyn_cast<SCEVAddRecExpr>(PtrSCEV);
    if (!PtrAR) 
        return nullptr;

    return new AccessPattern(BasePtr, BaseTyp, BaseSize, MemI, PtrValue, PtrAR, SubscriptAES);
}   

bool AccessPattern::isSameGroup(AccessPattern * NewAP) {
    return BP == NewAP->BP && BT == NewAP->BT && SubExp.size() == NewAP->SubExp.size();
}

void AccessPattern::copyFrom(AccessPattern * NewAP) {
    MemI = NewAP->MemI;
    PtrV = NewAP->PtrV;
    PtrAR = NewAP->PtrAR;
    SubExp = NewAP->SubExp;
}   

bool AccessPattern::update(AccessPattern * NewAP, ScalarEvolution &SE) {
    if (!isSameGroup(NewAP)) {
        return false;
    }


    auto *NewAR = NewAP->PtrAR;
    const Loop *L = NewAR->getLoop();
    assert(L == PtrAR->getLoop() && "cannot update from access patterns not in the same loop");
    Optional<Loop::LoopBounds> LB = L->getBounds(SE);
    assert(LB.hasValue() && "Unable to retrieve the loop bounds");

    const SCEV *PtrDiff = SE.getMinusSCEV(NewAR, PtrAR);

    // errs() << "update access pattern, old: \n";
    // PtrAR->print(errs());
    // errs() << "\nnew: \n";
    // NewAR->print(errs());
    // errs() << "\npointer diff: \n";
    // PtrDiff->print(errs());
    // errs() << "\n";
    // errs() << "\n";
    if (auto *ConstPtrDiff = dyn_cast<SCEVConstant>(PtrDiff)) {
        int64_t PD = ConstPtrDiff->getValue()->getSExtValue();
        // errs() << "difference is constant " << PD << "\n";
        if (abs(PD) > (int64_t) getCacheLineSize()) {
            errs() << "stride exceeds cachelinesize\n";
            return false;
        }   
        // errs() << "  PD = " << PD << "\n";
        // flag indicating whether the current AP needs updates from new one
        bool isnew = true;
        switch(LB->getDirection()) {
            case Loop::LoopBounds::Direction::Increasing: {
                // errs() << "loop is increasing\n";
                if (PD > 0) isnew = false;
                break;
            }
            case Loop::LoopBounds::Direction::Decreasing: {
                // errs() << "loop is decreasing\n";
                if (PD < 0) isnew = false;
                break;
            }
            case Loop::LoopBounds::Direction::Unknown: {
                // errs() << "loop has unknown direction\n";
                break;
            }
        }
        if (!isnew) {
            // update private info from AP
            // errs() << "  indeed copy from B[j+1][0] at stride " << PD << "\n";
            copyFrom(NewAP);
            return true;
        }
        return true;
    }
    return false;
}

void AccessPattern::printSubscriptExpr() {
    errs() << "( " << getShortValueName(BP) << " : " << BS << " )";
    for (auto &AS : SubExp) {
        errs() << "[";
        errs() << AS.toString();
        errs() << "]";
    }
    errs() << "\n::";
    PtrAR->print(errs());
}

void AccessPattern::computeAccessMatrix(LT_t &LT, 
                                            L2Idx_t &LID, Idv2L_t &IV2L) {
    unsigned SubscriptNum = SubExp.size();
    unsigned LoopNestDepth = LT.size();
    // errs() << "     begin init AccM & AccO, size = " << SubscriptNum << "x" << LoopNestDepth << "\n";
    AccM.resize(SubscriptNum, LoopNestDepth);
    AccO.resize(SubscriptNum);

    // for each subscript dimension, find AffineExprSimple
    for (int subscript = 0; subscript < SubscriptNum; ++subscript) {
        auto AS = SubExp[subscript];
        AccO(subscript) = static_cast<double>(AS.getConst());
        // errs() <<  "AccO(" << subscript << ") = " << AccO(subscript) << "\n";
        
        for (auto it : IV2L) {
            PHINode *indvar = it.first;
            Loop *L = it.second;
            int64_t ci;
            assert(AS.getCoeff(indvar, ci) && "computeAccessMatrix found unknown indvar!\n");
            uint64_t lid = LID[L];
            AccM(subscript, lid) = static_cast<double>(ci);
            // errs() << "AccM(" << subscript << ", " << lid << ") = " << AccM(subscript, lid) << "\n";
        }
    }
    // compute reuse
    TReuse = computeKernel(AccM);

    SReuse = computeKernel(eraseLastRow(AccM));
    // errs() << "AccM = \n";
    // std::cout << AccM << "\n";
    // errs() << "AccO = \n";
    // std::cout << AccO << "\n";
    // errs() << "TReuse = \n";
    // std::cout << TReuse << "\n";
}

void AccessPattern::computeLocality(vector<Loop *> &LocLoops, LT_t &LT, L2Idx_t &LID) {
    // errs() << "In computeLocality: \n";
    // errs() << "AccM: \n";
    // std::cout << AccM << "\n"; 
    // errs() << "with the following loops localized: \n";
    // for (auto *L : LocLoops) {
    //  errs() <<  "<" << L->getHeader()->getName() << ">, ";
    // }
    // errs() << "\n";

    // errs() << "TReuse: \n";
    // std::cout << TReuse << "\n";
    TLoc = computeLocalizedSpace(TReuse, LocLoops, LT, LID);
    // errs() << "TLoc: \n";
    // if (TLoc.size() > 0) std::cout << getMatrix(TLoc) << "\n";  
    
    // errs() << "SReuse: \n";
    // std::cout << SReuse << "\n";
    SLoc = computeLocalizedSpace(SReuse, LocLoops, LT, LID);
    // errs() << "TLoc: \n";
    // if (SLoc.size() > 0) td::cout << getMatrix(SLoc) << "\n";
    // errs() << "\n";

}

bool AccessPattern::isTemporal(Loop *L, LT_t &LT, L2Idx_t &LID) {
    // errs() << "isTemporal(" << L->getHeader()->getName() << "):\n";

    
    if (TLoc.size() == 0) 
        return false;
    unsigned lid = LID[L];
    unsigned rows = LT.size();
    vector<unsigned> lvec;
    lvec.push_back(lid);
    MatrixXd col = mkIdentityColumns(lvec, rows);
    
    // std::cout << "loop id col = \n" << col << "\n";

    MatrixXd matT = getMatrix(TLoc);

    // std::cout << "TLoc mat: \n";
    // std::cout << matT << "\n";

    vector<VectorXd> intersect = intersectVectorSpace(matT, col);
    
    // if (intersect.size() > 0) {
    //     // errs() << "intersect: \n";
    //     std::cout << getMatrix(intersect) << "\n";
    // }
    
    return intersect.size() != 0; 
}

bool AccessPattern::isSpatial(Loop *L, LT_t &LT, L2Idx_t &LID) {
    if (SLoc.size() == 0) 
        return false;
    unsigned lid = LID[L];
    unsigned rows = LT.size();
    vector<unsigned> lvec;
    lvec.push_back(lid);
    MatrixXd col = mkIdentityColumns(lvec, rows);
    MatrixXd matS = getMatrix(SLoc);
    vector<VectorXd> intersect = intersectVectorSpace(matS, col);
    return intersect.size() != 0; 
}

void AccessPattern::printAccessMatrix() {
    errs() << "accM: \n";
    std::cout << AccM << "\n";
    errs() << "accO: \n";
    std::cout << AccO << "\n";
}

void AccessPattern::printReuseMatrix() {
    errs() << "temporal reuseM: \n";
    std::cout << TReuse << "\n";
    errs() << "spatial reuseM: \n";
    std::cout << SReuse << "\n";
}

Value *AccessPattern::expandCodeFor(vector<PHINode *> &IVs, unsigned ItersAhead, 
                                    PHINode *oldIV, Value *newValue, LoopInfo *LI,
                                    ScalarEvolution *SE,  
                                    Instruction *InsertBefore) {

    // Module *M = Header->getModule();
    LLVMContext &context = InsertBefore->getContext();
    IntegerType* I32 = Type::getInt32Ty(context);
    IntegerType* I64 = Type::getInt64Ty(context);
    Type *I8Ptr = Type::getInt8PtrTy(context, 0/*PtrAddrSpace*/);

    Loop *OldL = LI->getLoopFor(oldIV->getParent());
    if (!OldL) {
        return nullptr;
    }
    Optional<Loop::LoopBounds> LB = OldL->getBounds(*SE);
    if (!LB.hasValue()) {
        errs() << "failed to expand code using AP because original loop can't be analyzed\n";
        return nullptr;
    }
    Loop::LoopBounds::Direction loopdir = LB->getDirection();

    IRBuilder<> Builder(InsertBefore);

    SmallVector<Value *, 4> IdxList;
    for (auto AS : SubExp) {
        // errs() << "-- AS: " << AS.toString() << "\n";

        Value *Sum = nullptr;
        for (PHINode *iv : IVs) {
            
            int64_t coeff;
            assert(AS.getCoeff(iv, coeff) && "wrong indvar when expanding pointer calculation");
            if (coeff == 0) {
                continue;
            }
            // substitute real indvar
            Value *ivTrue = iv;
            if (iv == oldIV) {
                bool pos = true;
                switch (loopdir) {
                    case Loop::LoopBounds::Direction::Decreasing: {
                        pos = false;
                    }
                }
                if (pos) {
                    ivTrue = Builder.CreateAdd(newValue, ConstantInt::get(I32, (uint64_t) ItersAhead), "add");
                    // errs() << "  ivTrue: ";
                    // ivTrue->print(errs());
                    // errs() << "\n";
                } else {    
                    ivTrue = Builder.CreateSub(newValue, ConstantInt::get(I32, (uint64_t) ItersAhead), "mul");
                }
            }
            // create term = (+/-)indvar * coeff
            Value *Term;
            if (coeff == 1) {
                Term = ivTrue;
            } else {
                Term = Builder.CreateMul(ivTrue, 
                                                ConstantInt::get(I32, static_cast<uint64_t>(coeff), true), 
                                                 "mul");
            }
            // errs() << "  term: ";
            // Term->print(errs());
            // errs() << "\n";
            // create sum = sum + term / sum = term
            if (!Sum) { 
                Sum = Term;
            } else {
                Sum = Builder.CreateAdd(Sum, Term, "add");
            }
            // errs() << "  sum: ";
            // Sum->print(errs());
            // errs() << "\n";
        }
        int64_t offset = AS.getConst();
        ConstantInt *offsetConst = ConstantInt::get(I32, static_cast<uint64_t>(offset), true);
        Value *IdxTerm;
        if (Sum) {
            if (offset) {
                IdxTerm = Builder.CreateAdd(Sum, offsetConst, "add");
            } else {
                IdxTerm = Sum;
            }
        } else {
            IdxTerm = offsetConst;
        }   
        // if (IdxTerm) 
        //     errs() << "Idxterm is nonnull\n";
        // errs() << "at least idx term is ";
        // IdxTerm->print(errs());
        // errs() << "\n";
        Value *IdxFinal = Builder.CreateSExt(IdxTerm, I64, "idxprom");
        IdxList.push_back(IdxFinal);
    }

    auto *BT = dyn_cast<PointerType>(BP->getType()->getScalarType())->getPointerElementType();
    Value *GEP = Builder.CreateGEP(BT, BP, IdxList, ".subst"); // refer base pointer type from base pointer
    errs() << "at least GEP is build\n";
    Value *BC = Builder.CreateBitCast(GEP, I8Ptr, "scevgep");
    
    return BC;
}

/// Memory Pointer Instruction Parser Implementation
AE_t parseIndexInstBinaryOperator(BinaryOperator *binopI, 
                                    ScalarEvolution *SE, LoopInfo *LI) {
    Value *op1 = binopI->getOperand(0);
    Value *op2 = binopI->getOperand(1); 

    switch (binopI->getOpcode()) {
        case Instruction::Add: {
            AE_t AE1 = parseIndexInst(op1, SE, LI);
            if (!AE1) {
                return nullptr;
            }

            AE_t AE2 = parseIndexInst(op2, SE, LI);
            if (!AE2) {
                return nullptr;
            }
            
            auto AEAdd = make_unique<AffineExprAdd>(move(AE1), move(AE2));
            return AEAdd;
        }
        case Instruction::Mul: {
            if (ConstantInt *C = dyn_cast<ConstantInt>(op1)) {
                AE_t AE2 = parseIndexInst(op2, SE, LI);
                if (!AE2) {
                    return nullptr;
                }
                auto AEMul = make_unique<AffineExprMul>(C, move(AE2));
                return AEMul;
            } else if (ConstantInt *C = dyn_cast<ConstantInt>(op2)) {
                AE_t AE1 = parseIndexInst(op1, SE, LI);
                if (!AE1) {
                    return nullptr;
                }
                auto AEMul = make_unique<AffineExprMul>(C, move(AE1));
            } else {
                // if not C * AE, e.g. i * j, then parse failed
                return nullptr; 
            }
        }
        case Instruction::Sub: {
            AE_t AE1 = parseIndexInst(op1, SE, LI);
            AE_t AE2 = parseIndexInst(op2, SE, LI);
            if (!AE1 || !AE2) {
                return nullptr;
            }
            LLVMContext &context = binopI->getParent()->getContext();
            auto negAE2 = make_unique<AffineExprMul>(getConstantInt(context, -1), move(AE2));
            auto AESub = make_unique<AffineExprAdd>(move(AE1), move(negAE2));
            return AESub;
        }
        default: {
            return nullptr;
        }
    }
}

AE_t parseIndexInst(Value *indexInst, ScalarEvolution *SE, LoopInfo *LI) {
    if (ConstantInt *constI = dyn_cast<ConstantInt>(indexInst)) {
        AE_t AEC = make_unique<AffineExprConst>(constI);
        return AEC;
    } else if (auto *binopI = dyn_cast<BinaryOperator>(indexInst)) {
        return parseIndexInstBinaryOperator(binopI, SE, LI);
    } else if (auto *sextI = dyn_cast<SExtInst>(indexInst)) {
        Value *indexInstNext = sextI->getOperand(0);
        return parseIndexInst(indexInstNext, SE, LI);
    } else if (auto *zextI = dyn_cast<ZExtInst>(indexInst)) {
        Value *indexInstNext = zextI->getOperand(0);
        return parseIndexInst(indexInstNext, SE, LI);
    } else if (auto *phiI = dyn_cast<PHINode>(indexInst)) {
        // check phiI is actually induction variable of its lopp
        Loop *L = LI->getLoopFor(phiI->getParent());
        if (!L) {
            return nullptr;
        }
        InductionDescriptor D;
        if (!InductionDescriptor::isInductionPHI(phiI, L, SE, D)) {
            return nullptr;
        }
        return make_unique<AffineExprIndVar>(phiI);
    } else {
        return nullptr;
    }
}

bool parsePtrValue(GetElementPtrInst *gepInst, ScalarEvolution *SE, 
                        LoopInfo *LI, Value **BP, Type ** BT, unsigned &BS, 
                        SubExp_t &SubscriptAE) {
    if (gepInst->getNumOperands() < 2) {
        errs() << "not an array subscription\n";
        return false;
    }

    *BP = gepInst->getOperand(0);

    *BT = gepInst->getResultElementType();

    const Module *M = gepInst->getModule();
    BS = getElementDataSize(*BT, M->getDataLayout());
    // errs() << "-- what is BS's value: " << BS << " should be: " << getElementDataSize(*BT, M->getDataLayout()) << "\n"; 

    for (unsigned idx = 1; idx < gepInst->getNumOperands(); ++idx) {
        Value *indexInst = gepInst->getOperand(idx);
        AE_t AE = parseIndexInst(indexInst, SE, LI);
        if (!AE) {
            return false;
        }
        SubscriptAE.push_back(move(AE));
    }
    return true;
}

bool parseAccessPattern(GetElementPtrInst *PtrValue, ScalarEvolution *SE, 
                        LoopInfo *LI, vector<PHINode *> IVs, Value **BP,
                        Type **BT, unsigned &BS, SubExpS_t &SubscriptAE) {
    SubExp_t SubExp;
    if (!parsePtrValue(PtrValue, SE, LI, BP, BT, BS, SubExp)) {
        errs() << "Parse failed for memory instruction\n";
        return false;
    }
    // errs() << "-- parseAccessPattern on ";
    // PtrValue->print(errs());
    // errs() << " end up with BS = " << BS << "\n";

    for (auto &AE : SubExp) {
        // errs() << "*     " << AffineExprToString(AE.get()) << "\n";
        auto AS = AE->Simplify(IVs);
        // errs() << "      " << AS->toString() << "\n";
        SubscriptAE.push_back(move(AS));
    }
    return true;
}

/// Data Type and Size: 
static unsigned getArrayElementDataSize(Type *ArrayElementDataType, const llvm::DataLayout &DL) {
    assert(ArrayElementDataType && "Type must not be null");

    if (ArrayElementDataType->isIntegerTy() 
        || ArrayElementDataType->isFloatingPointTy()) {
        // For primitive types like integers and floating points
        // errs() << " (a):";
        // errs() << ArrayElementDataType->getPrimitiveSizeInBits() << " ";
        return ArrayElementDataType->getPrimitiveSizeInBits() / 8;
    } else if (StructType *structType = llvm::dyn_cast<llvm::StructType>(ArrayElementDataType)) {
        // For structure types, sum the sizes of all elements
        unsigned totalSize = 0;
        for (unsigned i = 0, e = structType->getNumElements(); i < e; ++i) {
            totalSize += getArrayElementDataSize(structType->getElementType(i), DL);
        }
        return totalSize;
    } else if (PointerType *ptrType = dyn_cast<PointerType>(ArrayElementDataType)) {
        return DL.getPointerSize(ptrType->getPointerAddressSpace());
    } else {
        // Other types are not handled in this example
        assert(false && "Unsupported array element data type encountered");
        return 0;
    }
}
static unsigned getElementDataSize(Type *BasePtrElementType, const DataLayout &DL) {
    // Type *ArrayElementDataType = getElementDataType(BasePtrElementType);
    return getArrayElementDataSize(BasePtrElementType, DL);
}
static Type *getElementDataType(Type *NestArrayType) {
    if (!isa<ArrayType>(NestArrayType)) {
        return NestArrayType;
    }
    ArrayType *arrType = dyn_cast<ArrayType>(NestArrayType);
    return getElementDataType(arrType->getElementType());
}