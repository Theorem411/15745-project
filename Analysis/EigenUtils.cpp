#include "EigenUtils.h"
vector<VectorXd> getColumns(MatrixXd mat) {
    vector<VectorXd> columns;
    for (unsigned i = 0; i < mat.cols(); ++i) {
        columns.push_back(mat.col(i));
    }
    return columns;
}

MatrixXd getMatrix(vector<VectorXd> cols) {
    assert(cols.size() > 0 && "columns must be non-empty!");
    MatrixXd mat(cols[0].size(), cols.size());
    for (unsigned i = 0; i < cols.size(); ++i) {
        mat.col(i) = cols[i];
    }
    return mat;
}

MatrixXd rowReduceToRREF(MatrixXd mat) {
    int rows = mat.rows();
    int cols = mat.cols();

    int lead = 0;
    for (int r = 0; r < rows; ++r) {
        if (cols <= lead) {
            break;
        }

        int i = r;
        while (std::abs(mat(i, lead)) < 1e-12) {
            i++;
            if (rows == i) {
                i = r;
                lead++;
                if (cols == lead) {
                    return mat;
                }
            }
        }
        // std::// std::cout << "RREF lead: " << i << ", " << r << ", " << lead << "\n";

        VectorXd temp = mat.row(i);
        mat.row(i) = mat.row(r);
        mat.row(r) = temp;

        // std::// std::cout << "RREF swapped: \n";
        // std::// std::cout << mat << "\n";

        // Check if the pivot is non-zero to avoid division by zero
        double pivot = mat(r, lead);
        if (std::abs(pivot) > 1e-12) {
            mat.row(r) /= pivot; // Scale pivot to 1

            for (int i = 0; i < rows; ++i) {
                if (i != r) {
                    double lv = mat(i, lead);
                    mat.row(i) -= lv * mat.row(r); // Eliminate the other entries in the column
                }
            }
        }
        lead++;
    }
    // std::// std::cout << "RREF result: \n";
    // std::// std::cout << mat << "\n";
    return mat;
}

bool isExpressible(const VectorXd& v, const MatrixXd& Vb) {
    FullPivLU<MatrixXd> lu(Vb);
    VectorXd c = lu.solve(v);

    // Check if the solution is valid
    if (!lu.isInvertible() || (Vb * c - v).norm() > 1e-6) {
        // Not expressible if matrix is not invertible or solution is not accurate
        return false;
    }
    return true;
}

vector<unsigned> RREFPivotIndices(MatrixXd mat) {
    MatrixXd RREF = rowReduceToRREF(mat);
    vector<unsigned> pivotIndices;

    // Iterate over rows to find pivot columns
    for (unsigned i = 0; i < RREF.rows(); ++i) {
        for (unsigned j = 0; j < RREF.cols(); ++j) {
            if (std::abs(RREF(i, j)) > 1e-6) { // Found a pivot
                pivotIndices.push_back(j);
                break;
            }
        }
    }
    return pivotIndices;
}

vector<VectorXd> intersectVectorSpace(MatrixXd Ub, MatrixXd Vb) {
    MatrixXd concat(Ub.rows(), Ub.cols() + Vb.cols());
    concat << Ub, -Vb;
    // std::cout << "concat matrix: \n";
    // std::cout << concat << "\n";
    MatrixXd kernel = computeKernel(concat);
    // std::cout << "kernel matrix: \n";
    // std::cout << kernel << "\n\n";
    // take the Ub.size() upper parts of kernel
    MatrixXd seed = kernel.topRows(Ub.cols());
    // std::cout << "seed matrix: \n";
    // std::cout << seed << "\n";

    MatrixXd range = Ub * seed;
    // std::cout << "range matrix: \n";
    // std::cout << range << "\n";

    MatrixXd RREF = rowReduceToRREF(range);
    // std::cout << "range rref: \n";
    // std::cout << RREF << "\n";
    vector<unsigned> pivotColumns = RREFPivotIndices(range);
    vector<VectorXd> finalSpace;
    for (unsigned i : pivotColumns) {
        finalSpace.push_back(range.col(i));
    }
    return finalSpace;
}

/*= Eigen helper functions */
MatrixXd mkIdentityColumns(vector<unsigned> &indices, unsigned rows) {
    // Number of columns is the size of the indices vector
    unsigned cols = indices.size();
    // Initialize a matrix of the specified size with zeros
    MatrixXd mat = MatrixXd::Zero(rows, cols);
    // Fill each column with an identity column
    for (unsigned i = 0; i < cols; ++i) {
        unsigned idx = indices[i];
        assert(idx < rows && "Loop index exceeds bounds when computing matrix representation");
        mat(indices[i], i) = 1.0;
    }
    return mat;
}

MatrixXd eraseLastRow(MatrixXd mat) {
    MatrixXd mat_erased = mat;
    mat_erased.row(mat.rows()-1).setZero();
    return mat_erased;
}

MatrixXd computeKernel(MatrixXd mat) {
    FullPivLU<MatrixXd> LUDecompTemporal(mat);
    MatrixXd kernel = LUDecompTemporal.kernel();

    return kernel;
}

// vector<VectorXd> intersectCanonicalSpace(vector<VectorXd> vs, unordered_set<unsigned> &dims, const double thres) {
//     // for each column of mat1
//     vector<VectorXd> realizedCols;
//     for (VectorXd colVect: vs) {
//         bool realized = true;
//         for (unsigned dim = 0; dim < colVect.size(); ++dim) {
//             if (dims.find(dim) == dims.end()) {
//                 // if dim is not provided, colVect[dim] should be zero
//                 if (colVect(dim) >= thres) {
//                     realized = false;
//                     break;
//                 }
//             }
//         }
//         if (realized) {
//             realizedCols.push_back(colVect);
//         }
//     }
//     return realizedCols;
// }