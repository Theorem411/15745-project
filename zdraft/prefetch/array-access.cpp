#include "array-access.h"

void AccessPattern::computeAccessMatrix() {
    SubscriptNum = SubscriptAES.size();
    LoopNestDepth = LoopTrace.size();
    // outs() << "     begin init AccessMatrix & AccessOffset\n";
    AccessMatrix.resize(SubscriptNum, LoopNestDepth);
    AccessOffset.resize(SubscriptNum);

    // for each subscript dimension, find AffineExprSimple
    for (int subscript = 0; subscript < SubscriptNum; ++subscript) {
        auto AS = std::move(SubscriptAES[subscript]);
        AccessOffset(subscript) = static_cast<double>(AS->Const64);
        // outs() << "    AccessOffset update no problem!\n";
        for (auto it : AS->Coeffs) {
            PHINode *indvar = it.first;
            int64_t ci = it.second;

            Loop *L = AS->Indvar2Loop[indvar];
            uint64_t Lidx = LoopIndex[L];
            AccessMatrix(subscript, Lidx) = static_cast<double>(ci);
            // outs() << "    * AccessMatrix update no problem!\n";
        }
    }
}

bool GroupAccess::isConstPtrDiff(const SCEVAddRecExpr *MPtrSCEV) {
    /// TODO: 
    // unsigned InnermostIdx = MAP->LoopIndex[Loop];
    // Eigen::VectorXd LastCol = MAP->AccessMatrix.col(InnermostIdx);
    
    // double threshold = 1e-3;
    // for (auto it : MemberAp) {
    //     auto *AP = it->second;
    //     Eigen::VectorXd col = AP->AccessMatrix.col(InnermostIdx);
    
    //     // for each position of the last column
    //     double newDiff, ptrDiff = 0.0;
    //     bool first = true;
    //     for (size_t i = 0; i < LastCol.size(); ++i) {
    //         newDiff = LastCol(i) - col(i);
    //         if (std::abs(LastCol(i)) < threshold) {
    //             // for i such that lastcol[i] = 0, check leaderlastcol[i] = memberlastcol[i]
    //             if (std::abs(newDiff) >= threshold) {
    //                 return false;
    //             }
    //         } else {
    //             if (!first) {
    //                 // for all i such that lastcol[i] =!= 0, check if leaderlastcol[i] - memberlastcol[i] = d (d shared for all such i)
    //                 if (std::abs(newDiff - ptrDiff) >= threshold) {
    //                     return false;
    //                 }
    //             } 
    //         }
    //         first = false;
    //         ptrDiff = newDiff;
    //     }

    //     if (abs(ptrDiff) > getCacheLineSize()) {
    //         return false;
    //     }
    // }
    // return true;
}

bool GroupAccess::addMember(Instruction *MMemI, GetElementPtrInst *MPtrValue, const SCEVAddRecExpr *MPtrSCEV, AccessPattern *MAP) {
    // only use for other memory access in the same loop
    if (MemberAP.find(MMemI) != MemberAP.end()) {
        return false;
    }
    if (!canBeMember(MAP)) {
        // check base pointer type 
        return false;
    }
    if (!isConstPtrDiff(MPtrSCEV)) {
        // check whether access pattern diff only by constant less than cache line size
        return false;
    }
    // if new MemI is member of this GA, update write status
    Write |= isa<LoadInst>(MMemI);
    // update MemI, PtrValue, PtrSCEV, LeaderAP
    updateLeaderAP(MPtrSCEV);
    // update MemberAP
    MemberAP.insert({<MMemI, MAP});
    return true;
}

bool GroupAccess::updateLeaderAP(const SCEVAddRecExpr *MPtrSCEV) {
    return false;
}

AccessPattern *GroupAccess::parseAccessPattern(Instruction *MemI, GetElementPtrInst *PtrValue) {
    vector<unique_ptr<AffineExpr>> SubscriptAE;
    Value *BasePtr;
    Type *BaseTyp;
    if (!parsePtrValue(PtrValue, BasePtr, BaseTyp, SubscriptAE)) {
        errs() << "Parse failed for memory instruction: ";
        MemI->print(errs());
        errs() << "\n\n";
        return nullptr;
    }

    vector<unique_ptr<AffineExprSimpl>> SubScriptAES;
    for (auto &AE : SubscriptAE) {
        // outs() << "*     " << AffineExprToString(AE.get()) << "\n";
        auto AS = AE->Simplify(LoopOrder, Indvar2Loop, Loop2Indvar);
        // outs() << "      " << AS->toString() << "\n";
        SubScriptAES.push_back(std::move(AS));
    }
    return new AccessPattern(BasePtr, BaseTyp, SubscriptAES, LoopTrace, LoopIndex);
}

bool GroupAccess::parsePtrValue(GetElementPtrInst *gepInst, Value *&BasePtr, Type *& BaseTyp, vector<unique_ptr<AffineExpr>> &SubscriptAE) {
    if (gepInst->getNumOperands() == 3) {
        /** this is not the tomost subscript / subscript d/../1 */
        Value *val = gepInst->getOperand(0);
        if (!isa<GetElementPtrInst>(val)) {
            return false;
        }
        GetElementPtrInst *gepInstNext = dyn_cast<GetElementPtrInst>(val);
        if (!parsePtrValue(gepInstNext, SubscriptAE)) {
            // move onto previous subscript and update SubscriptAE
            return false;
        }
        Value *indexInst = gepInst->getOperand(2);
        std::unique_ptr<AffineExpr> AE = parseIndexInst(indexInst);
        if (!AE) {
            return false;
        }
        SubscriptAE.push_back(std::move(AE));
    } else {
        /** this is subscript 0 / topmost subscript */
        if (auto *loadAddr = dyn_cast<LoadInst>(gepInst->getOperand(0))) {
            ArrayBasePtr = loadAddr->getOperand(0);
        } else {
            ArrayBasePtr = gepInst->getOperand(0);
        }
        
        // get element data type of base pointer;
        Type *ResultType = ArrayBasePtr->getType();
        if (!isa<PointerType>(ResultType)) {
            throw std::runtime_error("encounter a gep instruction whose base type is not a pointer type!");
        }

        PointerType *PtrType = dyn_cast<PointerType>(ResultType);
        setElementDataTypeAndSize(PtrType->getPointerElementType());

        Value *indexInst = gepInst->getOperand(1);
        std::unique_ptr<AffineExpr> AE = parseIndexInst(indexInst);
        if (!AE) {
            return false;
        }
        SubscriptAE.push_back(std::move(AE));
    }
    return true;
}

bool GroupAccess::canBeMember(AccessPattern *MemberAP) {
    if (LeaderAP->BasePtr != MemberAP->BasePtr
        || LeaderAP->BaseTyp != MemberAP->BaseTyp) {
        // must be in the same loop, same base pointer, same loop to indvar mapping
        return false;
    }

    if (LeaderAP->SubscriptNum != MemberAP->SubscriptNum 
        || LeaderAP->LoopNestDepth != MemberAP->LoopNestDepth) {
        // their matrix dimension must be the same
        return false;
    }

    if (!LeaderAP->AccessMatrix.isApprox(MemberAP->AccessMatrix)) {
        // the coefficients of induction variables for each subscript must be exactly equal
        return false;
    }

    return true;
}

// Loop *ArrayAccess::getLoopFromInductionVariable(Value *val) {
//     auto it = Indvar2Loop.find(val);
//     if (it == Indvar2Loop.end()) {
//         return nullptr;
//     } 
//     return it->second;
// }   

// std::unique_ptr<IndvarInfo> ArrayAccess::getIndvarInfoFromInst(Loop *L, ICmpInst *cmpI, Instruction *I) {
//     if (auto *BI  = dyn_cast<BinaryOperator>(I)) {
//         Value *op1 = BI->getOperand(0);
//         Value *op2 = BI->getOperand(1); 
//         Instruction::BinaryOps opcode = BI->getOpcode();
//         Value *indvar;
//         Value *stride;
//         switch (opcode) {
//             case Instruction::Add: {
//                 if (L->isLoopInvariant(op1)) {
//                     indvar = op2;
//                     stride = op1;
//                 } else if (L->isLoopInvariant(op2)) {
//                     indvar = op1;
//                     stride = op2;
//                 } else {
//                     return nullptr;
//                 }
//                 break;
//             }
//             case Instruction::Mul: {
//                 break;
//             }
//             case Instruction::Sub: {
//                 break;
//             }
//             default: {
//                 return nullptr;
//             }
//         }
//         auto idv = std::make_unique<IndvarInfo>(indvar, L, opcode, stride, cmpI);
//         return std::move(idv);
//     } else if (auto *loadI = dyn_cast<LoadInst>(I)) {
//         Value *indvar = loadI->getOperand(0);
//         BasicBlock *LatchBlock = loadI->getParent();
//         for (Use &u : indvar->uses()) {
//             User *user = u.getUser();
//             if (StoreInst *storeI = dyn_cast<StoreInst>(user)) {
//                 if (storeI->getParent() != LatchBlock) {
//                     // store inst in the latch
//                     continue;
//                 }
//                 Value *inc = storeI->getOperand(0);
//                 auto *BI  = dyn_cast<BinaryOperator>(inc);
//                 if (!BI) {
//                     continue;
//                 } else {
//                     return getIndvarInfoFromInst(L, cmpI, BI);
//                 }
//             }
//         }
//         return nullptr;
//     } else {
//         return nullptr;
//     }
    
// }

// std::unique_ptr<IndvarInfo> ArrayAccess::getInductionVariableFromOptimizableLoop(Loop *L) {
//     if (!L) {
//         return nullptr;
//     }
//     // get latch's backedge
//     ICmpInst *LatchCmpInst = L->getLatchCmpInst();
//     if (!LatchCmpInst) {
//         return nullptr;
//     }
//     Instruction *Op1 = dyn_cast<Instruction>(LatchCmpInst->getOperand(0));
//     Instruction *Op2 = dyn_cast<Instruction>(LatchCmpInst->getOperand(1));
//     if (!Op1 || !Op2) {
//         return nullptr;
//     }
    
//     if (!Op1->getParent() || !Op2->getParent()) {
//         throw std::runtime_error("odd latch cmp operands don't have basic blocks!");
//     }
//     Loop *ParentL1 = LI.getLoopFor(Op1->getParent());
//     Loop *ParentL2 = LI.getLoopFor(Op2->getParent());
//     if (ParentL2 && ParentL2 == L && L->isLoopInvariant(Op1)) {
//         // Op2 is candidate induction variable
//         return std::move(getIndvarInfoFromInst(L, LatchCmpInst, Op2));
//     } else if (ParentL1 && ParentL1 == L && L->isLoopInvariant(Op2)) {
//         // Op1 is candidate induction variable
//         return std::move(getIndvarInfoFromInst(L, LatchCmpInst, Op1));
//     } else {
//         return nullptr;
//     }
// }
// /*= ArraryAccess representation ================*/
// bool ArrayAccess::buildLoop2Indvar(Loop *L, std::vector<Loop *>& LO) {
//     if (!L) return false;

//     std::unique_ptr<IndvarInfo> indvarInfo = getInductionVariableFromOptimizableLoop(L);
//     if (!indvarInfo) {
//         return false;
//     }
//     Value *indvar = indvarInfo->Indvar;

//     LO.push_back(L); // inner to outer // DEBUG: 
//     Loop2Indvar.insert({L, std::move(indvarInfo)});
//     Indvar2Loop.insert({indvar, L});
//     Loop *OuterLoop = L->getParentLoop();
//     if (!OuterLoop) {
//         return true;
//     }
//     return buildLoop2Indvar(OuterLoop, LO);
// }

// void ArrayAccess::loopIndexInit() {
//     for (int i = 0; i < LoopOrder.size(); ++i) {
//         Loop *L = LoopOrder[i];
//         LoopIndex[L] = i;
//     }
// }

// bool ArrayAccess::parseLoadOrStoreInst(Instruction *I, std::vector<std::unique_ptr<AffineExpr>>& SubscriptAE) {
//     if (!(isa<LoadInst>(I) || isa<StoreInst>(I))) {
//         return false;
//     }
//     // get operand of load/store instruction
//     Value *ptrOperand = nullptr;
//     if (auto *loadI = dyn_cast<LoadInst>(&*I)) {
//         ptrOperand = loadI->getPointerOperand();
//     } else if (auto *storeI = dyn_cast<StoreInst>(&*I)) {
//         ptrOperand = storeI->getPointerOperand();
//     } 
//     if (!ptrOperand) {
//         throw std::runtime_error("load/store instruction has null pointer operand!");
//     }
//     // 
//     if (!isa<GetElementPtrInst>(ptrOperand)) {
//         return false;
//     }
//     if (auto *gepInst = dyn_cast<GetElementPtrInst>(ptrOperand)) {   
//         return parseGetElementPtrInst(gepInst, SubscriptAE);
//     } 
//     return false;
// }

// bool ArrayAccess::parseGetElementPtrInst(GetElementPtrInst *gepInst, std::vector<std::unique_ptr<AffineExpr>>& SubscriptAE) {
//     if (gepInst->getNumOperands() == 3) {
//         /** this is not the tomost subscript / subscript d/../1 */
//         Value *val = gepInst->getOperand(0);
//         if (!isa<GetElementPtrInst>(val)) {
//             return false;
//         }
//         GetElementPtrInst *gepInstNext = dyn_cast<GetElementPtrInst>(val);
//         if (!parseGetElementPtrInst(gepInstNext, SubscriptAE)) {
//             // move onto previous subscript and update SubscriptAE
//             return false;
//         }
//         Value *indexInst = gepInst->getOperand(2);
//         std::unique_ptr<AffineExpr> AE = parseIndexInst(indexInst);
//         if (!AE) {
//             return false;
//         }
//         SubscriptAE.push_back(std::move(AE));
//     } else {
//         /** this is subscript 0 / topmost subscript */
//         if (auto *loadAddr = dyn_cast<LoadInst>(gepInst->getOperand(0))) {
//             ArrayBasePtr = loadAddr->getOperand(0);
//         } else {
//             ArrayBasePtr = gepInst->getOperand(0);
//         }
        
//         // get element data type of base pointer;
//         Type *ResultType = ArrayBasePtr->getType();
//         if (!isa<PointerType>(ResultType)) {
//             throw std::runtime_error("encounter a gep instruction whose base type is not a pointer type!");
//         }

//         PointerType *PtrType = dyn_cast<PointerType>(ResultType);
//         setElementDataTypeAndSize(PtrType->getPointerElementType());

//         Value *indexInst = gepInst->getOperand(1);
//         std::unique_ptr<AffineExpr> AE = parseIndexInst(indexInst);
//         if (!AE) {
//             return false;
//         }
//         SubscriptAE.push_back(std::move(AE));
//     }
//     return true;
// }

// std::unique_ptr<AffineExpr> ArrayAccess::parseIndexInst(Value *indexInst) {
//     if (ConstantInt *constI = dyn_cast<ConstantInt>(indexInst)) {
//         auto AEC = std::make_unique<AffineExprConst>(constI);
//         return AEC;
//     } else if (auto *binopI = dyn_cast<BinaryOperator>(indexInst)) {
//         return parseIndexInstBinaryOperator(binopI);
//     } else if (auto *sextI = dyn_cast<SExtInst>(indexInst)) {
//         Value *indexInstNext = sextI->getOperand(0);
//         return parseIndexInst(indexInstNext);
//     } else if (auto *loadI = dyn_cast<LoadInst>(indexInst)) {
//         /// DEBUG: in TapirIR, sequential loop indvar can sometimes be stored in stack-alloc memory
//         return parseIndexInst(loadI->getOperand(0));
//     } else {
        
//         /** DEBUG: considering TapirIR can mess up sequential loop indvar by storing them into stack-alloc memory */

//         // Loop *L = getLoopFromInductionVariable(SE, phiI);
//         // if (!L) {
//         //     return nullptr;
//         // }
//         // PHINode *indvar = getInductionVariableFromOptimizableLoop(L);
//         // if (indvar != phiI) {
//         //     return nullptr;
//         // }
//         // auto AEI = std::make_unique<AffineExprIndVar>(phiI, L, context);
//         // return AEI;

//         /** TODO: create map from indvar to associated loop & map from loop to struct IndVar{var *, loop *, init *, cmp *, stride *}  */
//         if (Loop *L = getLoopFromInductionVariable(indexInst)) {
//             auto &context = indexInst->getParent()->getContext();
//             auto AEI = std::make_unique<AffineExprIndVar>(indexInst, L, context);
//             return AEI;
//         } else {
//             return nullptr;
//         }
//     }
// }

// std::unique_ptr<AffineExpr> ArrayAccess::parseIndexInstBinaryOperator(BinaryOperator *binopI) {
//     Value *op1 = binopI->getOperand(0);
//     Value *op2 = binopI->getOperand(1); 

//     switch (binopI->getOpcode()) {
//         case Instruction::Add: {
//             std::unique_ptr<AffineExpr> AE1 = parseIndexInst(op1);
//             if (!AE1) {
//                 return nullptr;
//             }

//             std::unique_ptr<AffineExpr> AE2 = parseIndexInst(op2);
//             if (!AE2) {
//                 return nullptr;
//             }
            
//             auto AEAdd = std::make_unique<AffineExprAdd>(std::move(AE1), std::move(AE2));
//             return AEAdd;
//         }
//         case Instruction::Mul: {
//             if (ConstantInt *C = dyn_cast<ConstantInt>(op1)) {
//                 std::unique_ptr<AffineExpr> AE2 = parseIndexInst(op2);
//                 if (!AE2) {
//                     return nullptr;
//                 }
//                 auto AEMul = std::make_unique<AffineExprMul>(C, std::move(AE2));
//                 return AEMul;
//             } else if (ConstantInt *C = dyn_cast<ConstantInt>(op2)) {
//                 std::unique_ptr<AffineExpr> AE1 = parseIndexInst(op1);
//                 if (!AE1) {
//                     return nullptr;
//                 }
//                 auto AEMul = std::make_unique<AffineExprMul>(C, std::move(AE1));
//             } else {
//                 // if not C * AE, e.g. i * j, then parse failed
//                 return nullptr; 
//             }
//         }
//         case Instruction::Sub: {
//             std::unique_ptr<AffineExpr> AE1 = parseIndexInst(op1);
//             std::unique_ptr<AffineExpr> AE2 = parseIndexInst(op2);
//             if (!AE1 || !AE2) {
//                 return nullptr;
//             }
//             auto negAE2 = std::make_unique<AffineExprMul>(getConstantInt(context, -1), std::move(AE2));
//             auto AESub = std::make_unique<AffineExprAdd>(std::move(AE1), std::move(negAE2));
//             return AESub;
//         }
//         default: {
//             return nullptr;
//         }
//     }
// }

// std::vector<std::unique_ptr<AffineExprSimple>> ArrayAccess::getSubscriptExprs(std::vector<std::unique_ptr<AffineExpr>>& SubscriptAE) {
//     std::vector<std::unique_ptr<AffineExprSimple>> res;
//     for (auto &AE : SubscriptAE) {
//         (errs() << "*     " << AffineExprToString(AE.get()) << "\n");
//         auto AS = AE->Simplify(LoopOrder, Indvar2Loop, Loop2Indvar);
//         (errs() << "      " << AS->toString() << "\n");
//         res.push_back(std::move(AS));
//     }
//     return res;
// }

// Type * ArrayAccess::nestedArrayElementDataType(Type *NestArrayType) {
//     if (!isa<ArrayType>(NestArrayType)) {
//         return NestArrayType;
//     }
//     ArrayType *arrType = dyn_cast<ArrayType>(NestArrayType);
//     return nestedArrayElementDataType(arrType->getElementType());
// }
// unsigned ArrayAccess::ArrayElementDataSize(Type *ArrayElementDataType) {
//     assert(ArrayElementDataType && "Type must not be null");

//     if (ArrayElementDataType->isIntegerTy() || ArrayElementDataType->isFloatingPointTy()) {
//         // For primitive types like integers and floating points
//         return ArrayElementDataType->getPrimitiveSizeInBits();
//     } else if (StructType *structType = llvm::dyn_cast<llvm::StructType>(ArrayElementDataType)) {
//         // For structure types, sum the sizes of all elements
//         unsigned totalSize = 0;
//         for (unsigned i = 0, e = structType->getNumElements(); i < e; ++i) {
//             totalSize += ArrayElementDataSize(structType->getElementType(i));
//         }
//         return totalSize;
//     } else if (PointerType *ptrType = dyn_cast<PointerType>(ArrayElementDataType)) {
//         return DL.getPointerSize(ptrType->getPointerAddressSpace());
//     } else {
//         // Other types are not handled in this example
//         assert(false && "Unsupported array element data type encountered");
//         return 0;
//     }
// }

// void ArrayAccess::setElementDataTypeAndSize(Type *BasePtrElementType) {
//     Type *ArrayElementDataType = nestedArrayElementDataType(BasePtrElementType);
//     ElementDataType = ArrayElementDataType;
//     ElementDataSize = ArrayElementDataSize(ArrayElementDataType);
// }

// bool ArrayAccess::ArrayAccessInit() {
//     (errs() << "- begin checkLoopNestOptimizable - - - - - - - - - \n");
//     if (!checkLoopNestOptimizable()) {
//         // loop nests must all be optimizable loops
//         return false;
//     }
//     (errs() << "- begin parseSubscriptExpression - - - - - - - - - \n");
//     if (!parseSubscriptExpression()) {
//         // array subscript must be parsable
//         return false;
//     }
//     (errs() << "- begin computeAccessMatrix - - - - - - - - - - - -\n");
//     // compute access pattern matrix
//     computeAccessMatrix();
//     // // compute reuse matrix
//     // computeAccessReuse();
//     // // compute locality matrix by intersecting with localized iteration space

//     return true;
// }

// bool ArrayAccess::checkLoopNestOptimizable() {
//     // check whether I's loop nest are all optimizable while building Loop2Indvar
//     std::vector<Loop *> LO;
//     if (!buildLoop2Indvar(ParentLoop, LO)) {
//         return false;
//     }
//     // update "LoopOrder"
//     std::reverse(LO.begin(), LO.end()); // outer to inner
//     LoopOrder = LO;
//     // update "LoopIndex": inverse mapping of LoopOrder
//     loopIndexInit();
//     return true;
// }  

// void ArrayAccess::buildIndvar2SubscriptExprs() {
//     for (auto &AS : SubscriptExprs) {
//         for (auto it : AS->Coeffs) {
//             PHINode *indvar = it.first;
//             int64_t coeff = it.second;
//             if (coeff != 0) {
//                 auto it = Indvar2SubscriptExpr.find(indvar);
//                 if (it == Indvar2SubscriptExpr.end()) {
//                     Indvar2SubscriptExpr[indvar] = std::vector<std::unique_ptr<AffineExprSimple>>();
//                 }
//                 Indvar2SubscriptExpr[indvar].push_back(std::make_unique<AffineExprSimple>(*AS.get()));
//             }
//         }
//     }
// }

// bool ArrayAccess::parseSubscriptExpression() {
//     if (!isa<LoadInst>(OriginI) && !isa<StoreInst>(OriginI)) {
//         throw std::runtime_error("ArrayAccess cannot be constructed on an instruction that's not load or store!");
//     }
//     std::vector<std::unique_ptr<AffineExpr>> SubscriptExprsRec;
//     (errs() << "    - begin run parseLoadOrStoreInst to parse - - - - - - - -\n");
//     if (!parseLoadOrStoreInst(OriginI, SubscriptExprsRec)) {
//         // (errs() << "    >> parseLoadOrStoreInst failed to parse load/store instr " << getShortValueName(OriginI) << "\n");
//         return false;
//     }
//     (errs() << "    - begin converting AE to simple form - - - - - - - - - - - - -\n");
//     SubscriptExprs = getSubscriptExprs(SubscriptExprsRec);

//     // map from induction variable to subscripts expression they are used in
//     buildIndvar2SubscriptExprs();
//     return true;
// }

// std::string ArrayAccess::ppSubscriptExpr() {
//     std::string s;
//     raw_string_ostream strm(s);
    
//     int subscriptCnt = 0;
//     for (auto &AE : SubscriptExprs) {
//         strm << "[";
//         strm << AE->toString();
//         strm << "]";
//         subscriptCnt++;
//     }

//     strm.flush();
//     return s;
// }


// /*= GroupAccess main class & helper function =============================*/
// void GroupAccess::GroupAccessInitAfterStablize(std::vector<Loop *> LocalizedLoops) {
//     // compute temporal & spatial reuse
//     computeAccessReuse();
//     // compute localized iteration space
//     computeLocalizedIterationSpace(LocalizedLoops);
//     // compute temporal & spatial locality 
//     computeAccessLocality();
// }

// bool GroupAccess::isInSameGroup(std::unique_ptr<ArrayAccess>& NewMember) {
//     if (Leader->ArrayBasePtr != NewMember->ArrayBasePtr
//         || Leader->ParentLoop != NewMember->ParentLoop
//         || Leader->Loop2Indvar != NewMember->Loop2Indvar) {
//         // must be in the same loop, same base pointer, same loop to indvar mapping
//         return false;
//     }

//     if (Leader->SubscriptNum != NewMember->SubscriptNum 
//         || Leader->LoopNestDepth != NewMember->LoopNestDepth) {
//         // their matrix dimension must be the same
//         return false;
//     }
//     size_t SubscriptNum = Leader->SubscriptNum; 
//     size_t LoopNestDepth = Leader->LoopNestDepth;

//     Loop *Innermost = Leader->ParentLoop;
//     unsigned InnermostIdx = Leader->LoopIndex[Innermost];
//     Eigen::VectorXd LeaderLastCol= Leader->AccessMatrix.col(InnermostIdx);
//     Eigen::VectorXd MemberLastCol = NewMember->AccessMatrix.col(InnermostIdx);
//     // if (!LeaderLastCol.isApprox(MemberLastCol)) {
//     //     return false;
//     // }
//     if (!Leader->AccessMatrix.isApprox(NewMember->AccessMatrix)) {
//         // the coefficients of induction variables for each subscript must be exactly equal
//         return false;
//     }

//     // for each position of the last column
//     double threshold = 1e-3;
//     double newDiff, constDiff = 0.0;
//     bool first = true;
//     for (size_t i = 0; i < LeaderLastCol.size(); ++i) {
//         newDiff = LeaderLastCol(i) - MemberLastCol(i);
//         if (std::abs(LeaderLastCol(i)) < threshold) {
//             // for i such that lastcol[i] = 0, check leaderlastcol[i] = memberlastcol[i]
//             if (std::abs(newDiff) >= threshold) {
//                 return false;
//             }
//         } else {
//             if (!first) {
//                 // for all i such that lastcol[i] =!= 0, check if leaderlastcol[i] - memberlastcol[i] = d (d shared for all such i)
//                 if (std::abs(newDiff - constDiff) >= threshold) {
//                     return false;
//                 }
//             } 
//         }
//         first = false;
//         constDiff = newDiff;
//     }

//     // check if constDiff is smaller than cacheline size
//     int64_t ptrDiff = static_cast<int64_t>(constDiff);
//     if (ptrDiff >= (int64_t)TTI->getCacheLineSize()) {
//         return false;
//     }
//     return true;
// }
// // 
// void updateLeader(std::unique_ptr<ArrayAccess> NewLeader) {
//     /// TODO: 
// }

// bool GroupAccess::addMember(std::unique_ptr<ArrayAccess>& NewMember) {
//     // decide if NewMember belongs to the same group as leader
//     if (!isInSameGroup(NewMember)) {
//         return false;
//     }
//     // if newMember is in the same group, update Members
//     Members.insert(NewMember.get());
//     // decide who's the new leader
//     updateLeader(NewMember);
//     return true;
// }

// void GroupAccess::computeAccessReuse() {
//     Eigen::FullPivLU<Eigen::MatrixXd> LUDecompTemporal(Leader->AccessMatrix);
//     TemporalReuse = LUDecompTemporal.kernel();

//     Eigen::MatrixXd LastRowZero(Leader->AccessMatrix);
//     LastRowZero.row(LastRowZero.rows() - 1).setZero();
//     Eigen::FullPivLU<Eigen::MatrixXd> LUDecomSpatial(LastRowZero);
//     SpatialReuse = LUDecomSpatial.kernel(); // assignment is deep copy 
// }

// void GroupAccess::computeLocalizedIterationSpace(std::vector<Loop *> &LocalizedLoops) {
//     /** TODO: 
//     convert all the localized loops (if found in Leader->Loop2Index)
//     to identity vector
//     */
// }

// void GroupAccess::computeAccessLocality() {
//     /** TODO: 
//     compute the intersection of localizedIterationSpace & Reuse space
//     */
// }

// void GroupAccess::printAccessLinearAlgebra() {
//     const double threshold = 1e-2;
//     outs() << "Array Access Matrix : \n";
//     outs() << printEigenMatrixXd(Leader->AccessMatrix, threshold);

//     outs() << "Array Access Offset : \n";
//     outs() << printEigenVectorXd(Leader->AccessOffset, threshold);

//     outs() << "Array Access Temporal Locality iteration space (column-wise): \n";
//     outs() << printEigenMatrixXd(TemporalReuse, threshold);

//     outs() << "Array Access Spatial Locality iteration space (column-wise): \n";
//     outs() << printEigenMatrixXd(SpatialReuse, threshold);
// }
