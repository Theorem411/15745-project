#include <iostream>
#include <cilk/cilk.h>
#include <chrono>
#include <ctime>

const int matrixSize = 1000;
const int numNonZeroElements = 5000;

struct SparseMatrix {
    int rowIndices[matrixSize + 1];
    int colIndices[numNonZeroElements];
    double values[numNonZeroElements];
};

void sparseMatrixVectorMultiply(const SparseMatrix& matrix, const double vector[matrixSize],
                                double result[matrixSize]) {
    cilk_for (int i = 0; i < matrixSize; ++i) {
        result[i] = 0.0;

        for (int j = matrix.rowIndices[i]; j < matrix.rowIndices[i + 1]; ++j) {
            result[i] += matrix.values[j] * vector[matrix.colIndices[j]];
        }
    }
}

int main() {
    // Initialize a sparse matrix and a dense vector
    SparseMatrix matrix = {
        {0, 2, 4, 7, 9, 10, 12, 14, 16, 19, 20},
        {0, 1, 1, 2, 3, 0, 2, 4, 0, 1, 3, 4, 0, 3, 4, 1, 2, 4, 3, 4},
        {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0}
    };

    double vector[matrixSize];
    double result[matrixSize];

    // Seed for random number generation
    std::srand(std::time(0));

    // Fill the vector with random data between 0 and 1
    for (int i = 0; i < matrixSize; ++i) {
        vector[i] = static_cast<double>(std::rand()) / RAND_MAX;
    }

    // Measure the execution time of sparse matrix-vector multiplication
    auto start = std::chrono::high_resolution_clock::now();

    // Perform sparse matrix-vector multiplication
    sparseMatrixVectorMultiply(matrix, vector, result);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Sparse Matrix-Vector Multiplication Time: " << duration.count() << " seconds\n";

    return 0;
}
