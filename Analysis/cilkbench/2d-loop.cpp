#include <iostream>
#include <cilk/cilk.h>
#include <chrono>

const int numRows = 1000;
const int numCols = 1000;

// Function to calculate the sum of all elements in a 2D array
int calculateSum(const int array[numRows][numCols]) {
    int sum = 0;

    cilk_for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            // Apply prefetching here
            // __builtin_prefetch(&array[i][j], 0, 1);

            sum += array[i][j];
        }
    }

    return sum;
}

int main() {
    // Initialize a 2D array
    int array[numRows][numCols];

    // Fill the array with dummy data
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            array[i][j] = i * numCols + j;
        }
    }

    // Measure the execution time of the array traversal
    auto start = std::chrono::high_resolution_clock::now();

    // Perform array traversal and calculate the sum
    int sum = calculateSum(array);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Array Traversal and Sum Time: " << duration.count() << " seconds\n";
    std::cout << "Sum: " << sum << std::endl;

    return 0;
}