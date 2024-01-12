#include <iostream>
#include <cilk/cilk.h>
#include <chrono>

const int matrixSize = 200;

struct SparseMatrix {
    int rowIndices[matrixSize + 1];
    int colIndices[matrixSize];
    double values[matrixSize];
};

void sparseMatrixMatrixMultiply(const SparseMatrix& matrixA, const SparseMatrix& matrixB, double result[matrixSize][matrixSize]) {
    // Initialize the result matrix to zeros
    for (int i = 0; i < matrixSize; ++i) {
        for (int j = 0; j < matrixSize; ++j) {
            result[i][j] = 0.0;
        }
    }

    cilk_for (int i = 0; i < matrixSize; ++i) {
        for (int j = 0; j < matrixSize; ++j) {
            for (int k = matrixB.rowIndices[j]; k < matrixB.rowIndices[j + 1]; ++k) {
                result[i][j] += matrixA.values[i] * matrixB.values[k];
            }
        }
    }
}

int main() {
    // Initialize two sparse matrices
    SparseMatrix matrixA;
    for (int i = 0; i < matrixSize + 1; ++i) {
        matrixA.rowIndices[i] = i * (matrixSize / matrixSize);
    }

    for (int i = 0; i < matrixSize; ++i) {
        matrixA.colIndices[i] = i % matrixSize;
        matrixA.values[i] = static_cast<double>(i + 1);
    }

    SparseMatrix matrixB;
    for (int i = 0; i < matrixSize + 1; ++i) {
        matrixB.rowIndices[i] = i * (matrixSize / matrixSize);
    }

    for (int i = 0; i < matrixSize; ++i) {
        matrixB.colIndices[i] = i % matrixSize;
        matrixB.values[i] = static_cast<double>(i + 1);
    }

    double result[matrixSize][matrixSize];

    // Measure the execution time of sparse matrix-matrix multiplication
    auto start = std::chrono::high_resolution_clock::now();

    // Perform sparse matrix-matrix multiplication
    sparseMatrixMatrixMultiply(matrixA, matrixB, result);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Sparse Matrix-Matrix Multiplication Time: " << duration.count() << " seconds\n";

    return 0;
}