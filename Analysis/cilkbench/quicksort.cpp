#include <iostream>
#include <algorithm>
#include <cilk/cilk.h>
#include <chrono>

void quicksort(int arr[], int low, int high) {

    if (low < high) {
        int pivot = arr[high];
        int i = low - 1;

        cilk_for (int j = low; j <= high - 1; j++) {
            if (arr[j] < pivot) {
                i++;
                std::swap(arr[i], arr[j]);
            }
        }

        std::swap(arr[i + 1], arr[high]);

        int partitionIndex = i + 1;

        cilk_spawn quicksort(arr, low, partitionIndex - 1);
        quicksort(arr, partitionIndex + 1, high);
        cilk_sync;
    }
}

void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

int main() {
    // Initialize an array for sorting
    int arr[] = {12, 4, 5, 6, 7, 3, 1, 15};
    int arrSize = sizeof(arr) / sizeof(arr[0]);

    // Call quicksort function
    auto start = std::chrono::high_resolution_clock::now();
    quicksort(arr, 0, arrSize - 1);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Quicksort Time: " << duration.count() << " seconds\n";
    // std::cout << "Sorted Array: ";
    // printArray(arr, arrSize);

    return 0;
}