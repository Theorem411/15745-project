#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#include <cilk/cilk.h>
#include <iostream>

inline unsigned long long readTSC() {
    // _mm_lfence();  // optionally wait for earlier insns to retire before reading the clock
    return __rdtsc();
    // _mm_lfence();  // optionally block later instructions until rdtsc retires
}

int loop(int A[10][10][10], int N) {
    int x = 0; 
    auto beginTime = readTSC();
    cilk_for(int i = 0; i < N; ++i) {
        cilk_for(int j = 1; j < N; ++j) {
            cilk_for(int k = 2; k < N; ++k) {
                A[i][j][k] = 1;
            }
        }
    }
    auto endTime = readTSC();
    std::cout << "clock cycles: " << endTime - beginTime << "\n"; 
    return x;
}

int main() {
    int A[10][10][10]; 
    int N = 10;
    loop(A, N);
    return 0;
}