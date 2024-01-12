#include "utils.h"

/*= LLVM context & ConstantInt helper =================*/
ConstantInt * getConstantInt(LLVMContext &context, int64_t n) {
    llvm::IntegerType *Int64Type = Type::getInt64Ty(context);
    ConstantInt *Zero = ConstantInt::get(Int64Type, n);
    return Zero;
}

/*= print function for llvm::Value ======================*/
std::string getShortValueName(Value *v) {
	if (v->getName().str().length() > 0) {
		return "%" + v->getName().str();
	} else if (isa<Instruction>(v)) {
		std::string s = "";
		raw_string_ostream *strm = new raw_string_ostream(s);
		v->print(*strm);
		std::string inst = strm->str();
		delete strm;
		size_t idx1 = inst.find("%");
		size_t idx2 = inst.find(" ", idx1);
		if (idx1 != std::string::npos && idx2 != std::string::npos) {
			return inst.substr(idx1, idx2 - idx1);
		} else {
			return "\"" + inst + "\"";
		}
	} else if (ConstantInt *cint = dyn_cast<ConstantInt>(v)) {
		std::string s = "";
		raw_string_ostream *strm = new raw_string_ostream(s);
		cint->getValue().print(*strm, true);
		std::string inst = strm->str();
		delete strm;
		return inst;
	} else if (Argument *arg = dyn_cast<Argument>(v)) {
		std::string s = "";
		raw_string_ostream *strm = new raw_string_ostream(s);
		v->print(*strm);
		std::string inst = strm->str();
		delete strm;
		size_t idx1 = inst.find("%");
		if (idx1 != std::string::npos) {
			return inst.substr(idx1);
		} else {
			return "\"" + inst + "\"";
		}

	} else if (isa<BasicBlock>(v)) {
		std::string s = "";
		raw_string_ostream *strm = new raw_string_ostream(s);
		v->print(*strm);
		std::string bb = strm->str();
		delete strm;
		size_t idx1 = bb.find(":");
		if (idx1 != std::string::npos) {
			return bb.substr(0, idx1);
		} else {  // for any block with no label (bc no one jumps to here) e.g.
				  // entry block
			return "\"" + bb + "\"";
		}
	} else {
		std::string s = "";
		raw_string_ostream *strm = new raw_string_ostream(s);
		v->print(*strm);
		std::string inst = strm->str();
		delete strm;
		return "\"" + inst + "\"";
	}
}

/*= Loop Body estimation functions */
BlockCost_t getBlockCost(Loop *L, AssumptionCache *AC, TargetTransformInfo *TTI) {
    SmallPtrSet<const Value *, 32> EphValues;
    CodeMetrics::collectEphemeralValues(L, AC, EphValues);

    CodeMetrics Metrics;
    for (auto *BB : L->blocks()) {
        Metrics.analyzeBasicBlock(BB, *TTI, EphValues);
    }

    // for (auto *BB : L->blocks()) {
    //     errs() << BB->getName() << ": " << Metrics.NumBBInsts[BB] << "\n";
    // }
    return Metrics.NumBBInsts;
}

unsigned loopBodyCost(Loop *L, BlockCost_t CostBook) {
    stack<pair<BasicBlock*, unsigned>> stack;
    unordered_map<BasicBlock*, unsigned> visited;
    unsigned maxCost = 0;

    BasicBlock *header = L->getHeader();
    stack.push({header, 0});

    while (!stack.empty()) {
        BasicBlock *current = stack.top().first;
        unsigned currentCost = stack.top().second;
        stack.pop();

        // Mark as visited with the path cost
        if (visited.find(current) != visited.end()) {
            continue; // Skip if the current path is not more expensive
        }
        unsigned blockCost = CostBook.lookup(current);
        unsigned pathCost = currentCost + blockCost;
        visited[current] = pathCost; 
        // errs() << "visit " << current->getName() << " at cost: " << pathCost << "\n";

        // Update the maximum path cost
        if (!L->contains(current)) { // succ_empty(current) || 
            if (pathCost > maxCost) {
                maxCost = pathCost;
            }
        }
        maxCost = std::max(maxCost, pathCost);
        // errs() << "maxCost update to " << maxCost << "\n";

        // Add successors to the stack
        for (BasicBlock *Succ : successors(current)) {
            if (L->contains(Succ)) {
                stack.push({Succ, pathCost});
            }
        }
    }

    return maxCost;
}   