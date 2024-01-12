// #include "llvm/Support/raw_ostream.h"
#include <Eigen/Dense>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
// using namespace llvm;
using namespace Eigen;
using namespace std;
/*= Eigen helper functions */
// string printEigenMatrixXd(MatrixXd mat, const double threshold);
// string printEigenVectorXd(VectorXd vec, const double threshold);

/*= Eigen helper for computing intersection and kernel */
MatrixXd rowReduceToRREF(MatrixXd mat);
bool isExpressible(const VectorXd& v, const MatrixXd& Vb);
vector<unsigned> RREFPivotIndices(MatrixXd mat);

MatrixXd eraseLastRow(MatrixXd mat);
MatrixXd mkIdentityColumns(vector<unsigned> &indices, unsigned rows);
MatrixXd computeKernel(MatrixXd mat);
vector<VectorXd> getColumns(MatrixXd mat);
MatrixXd getMatrix(vector<VectorXd> cols);
vector<VectorXd> intersectVectorSpace(MatrixXd Ub, MatrixXd Vb);