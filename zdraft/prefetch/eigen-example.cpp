#include <Eigen/Dense>
#include <iostream>

int main() {
    // Define a sample matrix
    Eigen::MatrixXd A(4, 3);
    A << 1, 2, 3,
         4, 5, 6,
         7, 8, 9,
         10, 11, 12;

    // Compute the SVD
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(A, Eigen::ComputeFullV);

    // Extract the singular values
    auto singularValues = svd.singularValues();

    // Extract the matrix V
    Eigen::MatrixXd V = svd.matrixV();

    // Iterate through singular values to find the ones that are zero (or close to zero)
    for (int i = 0; i < singularValues.size(); ++i) {
        if (fabs(singularValues[i]) < 1e-6) { // Threshold for considering as zero
            // The corresponding column in V is a basis vector for the nullspace
            std::cout << "Basis vector for nullspace: " << V.col(i).transpose() << std::endl;
        }
    }

    return 0;
}