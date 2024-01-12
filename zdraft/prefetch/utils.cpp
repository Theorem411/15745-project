#include "utils.h"

/*= LLVM context & ConstantInt helper =================*/
ConstantInt * getConstantInt(LLVMContext &context, int64_t n) {
    llvm::IntegerType *Int64Type = Type::getInt64Ty(context);
    ConstantInt *Zero = ConstantInt::get(Int64Type, n);
    return Zero;
}

/*= print function for llvm::Value ======================*/
string getShortValueName(Value *v) {
	if (v->getName().str().length() > 0) {
		return "%" + v->getName().str();
	} else if (isa<Instruction>(v)) {
		string s = "";
		raw_string_ostream *strm = new raw_string_ostream(s);
		v->print(*strm);
		string inst = strm->str();
		delete strm;
		size_t idx1 = inst.find("%");
		size_t idx2 = inst.find(" ", idx1);
		if (idx1 != string::npos && idx2 != string::npos) {
			return inst.substr(idx1, idx2 - idx1);
		} else {
			return "\"" + inst + "\"";
		}
	} else if (ConstantInt *cint = dyn_cast<ConstantInt>(v)) {
		string s = "";
		raw_string_ostream *strm = new raw_string_ostream(s);
		cint->getValue().print(*strm, true);
		string inst = strm->str();
		delete strm;
		return inst;
	} else if (Argument *arg = dyn_cast<Argument>(v)) {
		string s = "";
		raw_string_ostream *strm = new raw_string_ostream(s);
		v->print(*strm);
		string inst = strm->str();
		delete strm;
		size_t idx1 = inst.find("%");
		if (idx1 != string::npos) {
			return inst.substr(idx1);
		} else {
			return "\"" + inst + "\"";
		}

	} else if (isa<BasicBlock>(v)) {
		string s = "";
		raw_string_ostream *strm = new raw_string_ostream(s);
		v->print(*strm);
		string bb = strm->str();
		delete strm;
		size_t idx1 = bb.find(":");
		if (idx1 != string::npos) {
			return bb.substr(0, idx1);
		} else {  // for any block with no label (bc no one jumps to here) e.g.
				  // entry block
			return "\"" + bb + "\"";
		}
	} else {
		string s = "";
		raw_string_ostream *strm = new raw_string_ostream(s);
		v->print(*strm);
		string inst = strm->str();
		delete strm;
		return "\"" + inst + "\"";
	}
}

/*= Eigen helper functions */
string printEigenMatrixXd(Eigen::MatrixXd mat, const double threshold) {
    string s;
    raw_string_ostream oss(s);

    for (int i = 0; i < mat.rows(); ++i) {
        bool first = true;
        oss << "[";
        for (int j = 0; j < mat.cols(); ++j) {
            if (!first) {
                oss << ", ";
            }
            first = false;
            double entry = mat(i, j);
            oss << (abs(entry) < threshold ? 0.0 : entry);
        }
        oss << "]\n";
    }
    oss.flush();
    return s;
}

string printEigenVectorXd(Eigen::VectorXd vec, const double threshold) {
    string s;
    raw_string_ostream oss(s);
    oss << "[";
    bool first = true;
    for (int i = 0; i < vec.size(); ++i) {
        if (!first) {
            oss << ", ";
        }
        first = false;
        double entry = vec(i);
        oss << (abs(entry) < threshold ? 0.0 : entry);
    }
    oss << "]\n";
    oss.flush();
    return s;
}

