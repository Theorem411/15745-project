#include <iostream>
#include <cilk/cilk.h>
#include <chrono>

const int dim1 = 500;
const int dim2 = 500;
const int filterSize = 3;

void apply2DConvolution(const double input[dim1][dim2],
                         const double filter[filterSize][filterSize],
                         double output[dim1][dim2]) {
    cilk_for (int i = 1; i < dim1 - 1; ++i) {
        for (int j = 1; j < dim2 - 1; ++j) {
            // Apply prefetching here if needed
            // __builtin_prefetch(&input[i][j], 0, 1);
            // __builtin_prefetch(&input[i + 1][j], 0, 1);
            // __builtin_prefetch(&input[i][j + 1], 0, 1);

            // Simple 2D convolution operation
            output[i][j] = 0.0;
            for (int m = 0; m < filterSize; ++m) {
                for (int n = 0; n < filterSize; ++n) {
                    output[i][j] += input[i - 1 + m][j - 1 + n] * filter[m][n];
                }
            }
        }
    }
}

int main() {
    // Initialize 2D input, filter, and output arrays
    double input[dim1][dim2];
    double filter[filterSize][filterSize];
    double output[dim1][dim2];

    // Fill input with dummy data
    for (int i = 0; i < dim1; ++i) {
        for (int j = 0; j < dim2; ++j) {
            input[i][j] = static_cast<double>(i * dim2 + j + 1);  // Use your initialization logic here
        }
    }

    // Fill filter with dummy data
    for (int i = 0; i < filterSize; ++i) {
        for (int j = 0; j < filterSize; ++j) {
            filter[i][j] = 0.5;
        }
    }

    // Initialize output to zeros
    for (int i = 0; i < dim1; ++i) {
        for (int j = 0; j < dim2; ++j) {
            output[i][j] = 0.0;
        }
    }

    // Measure the execution time of the 2D convolution
    auto start = std::chrono::high_resolution_clock::now();

    // Perform the 2D convolution
    apply2DConvolution(input, filter, output);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "2D Convolution Time: " << duration.count() << " seconds\n";

    return 0;
}
