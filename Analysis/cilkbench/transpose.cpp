#include <iostream>
#include <cilk/cilk.h>
#include <chrono>

// Function to transpose a matrix
void transposeMatrix(const int inputMatrix[1000][1000], int outputMatrix[1000][1000]) {
    const int numRows = 1000;
    const int numCols = 1000;

    cilk_for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            // Apply prefetching here if needed
            // __builtin_prefetch(&inputMatrix[i][j], 0, 1);

            outputMatrix[j][i] = inputMatrix[i][j];
        }
    }
}

// Function to print a matrix
void printMatrix(const int matrix[1000][1000]) {
    const int numRows = 1000;
    const int numCols = 1000;

    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

int main() {
    const int numRows = 1000;
    const int numCols = 1000;

    // Initialize a matrix
    int inputMatrix[numRows][numCols];

    // Fill the matrix with dummy data
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            inputMatrix[i][j] = i * numCols + j;
        }
    }

    // Create a transpose matrix
    int outputMatrix[numCols][numRows];

    // Measure the execution time of the matrix transpose operation
    auto start = std::chrono::high_resolution_clock::now();

    // Perform matrix transpose
    transposeMatrix(inputMatrix, outputMatrix);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Matrix Transpose Time: " << duration.count() << " seconds\n";

    // Optionally, print the matrices for verification
    // printMatrix(inputMatrix);
    // std::cout << std::endl;
    // printMatrix(outputMatrix);

    return 0;
}