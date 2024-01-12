#include <iostream>
#include <Eigen/Dense>
#include "EigenUtils.h"
using namespace std;
using namespace Eigen;

int main() {
    // test 
    // cout << "test on matrix: \n";
    // MatrixXd mat(4, 4);
    // mat << 1, 1, 1, 1,
    //        0, 2, 2, 2,
    //        0, 0, 3, 3,
    //        0, 0, 0, 4;
    // cout << mat << "\n";

    // test eraseLastRow
    // cout << "test erase last row: \n";
    // MatrixXd mat_erase = eraseLastRow(mat);
    // cout << mat_erase << "\n";

    // test mkIdentityCols
    // cout << "test make identity columns: \n";
    // vector<unsigned> indices({1, 2});
    // unsigned rows = 5;
    // MatrixXd mkIdCols = mkIdentityColumns(indices, rows);
    // cout << mkIdCols << "\n";

    // test rowreduceToRREF
    cout << "test row reduction RREF: \n";
    MatrixXd mat(4, 4);
    mat << 1, 1, 1, 1,
           0, 2, 2, 2,
           0, 0, 3, 3,
           0, 0, 0, 4;
    cout << mat << "\n begin rowReduceToRREF:\n";
    MatrixXd RREF = rowReduceToRREF(mat);   
    cout << RREF << "\n";

    // test compute kernel
    cout << "test computation of kernel: \n";
    mat.resize(1, 1);
    mat << 0;
    cout << mat << "\n begin computeKernel:\n";
    MatrixXd kernel = computeKernel(mat);    
    cout << kernel << "\n";


    cout << "test intersectVectorSpace: \n";
    vector<unsigned> indices({0});
    unsigned rows = 1;
    MatrixXd mkIdCols = mkIdentityColumns(indices, rows); 
    cout << "intersect with: \n";
    cout << mkIdCols << "\n";

    // MatrixXd comb(kernel.rows(), kernel.cols() + mkIdCols.cols());
    // comb << kernel, mkIdCols;
    // cout << "combined matrix: \n";
    // cout << comb << "\n";

    // MatrixXd combRREF = rowReduceToRREF(comb);
    // cout << "combined matrix rref: \n";
    // cout << combRREF << "\n";
    vector<VectorXd> intersect = intersectVectorSpace(kernel, mkIdCols);
    cout << "intersection results: \n";
    if (intersect.size() == 0) {
        cout << "the intersection between kernel and localized space is empty !\n";
    } else {
        MatrixXd intersectMat = getMatrix(intersect);
        cout << intersectMat << "\n";
    }

    return 0;
}