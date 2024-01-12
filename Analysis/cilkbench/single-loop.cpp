#include <cilk/cilk.h>
#include <stdlib.h>
#include <stdio.h>
#include <chrono>

int ARRAY_SIZE = 50000;

inline
unsigned long long readTSC() {
    return __rdtsc();
}

unsigned long long todval (struct timeval *tp) {
    return tp->tv_sec * 1000 * 1000 + tp->tv_usec;
}

__attribute__((noinline))
void startup_cilk() {
    double g = 2.0;
    cilk_for(int i = 0; i < 1000; i++) {
        g++;
    }
}

#ifdef SERIAL
#include <cilk/cilk_stub.h>
#endif

int simpleLoop(int A[ARRAY_SIZE]) {
    int result = 0;
    cilk_for(int i = 0; i < ARRAY_SIZE; i++) {
        result += A[i];
    }
    return result;
}

int main(int argc, char *argv[]) {
    int n, result, round;
    round = 1;

    if (argc != 2) {
        fprintf(stderr, "Usage: simple-loop [<cilk options>] <number of rounds>\n");
        exit(1);
    }

    // Start up cilk runtime
    startup_cilk();

    round = atoi(argv[1]);

    int *A = (int *)calloc(ARRAY_SIZE, sizeof(int *));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        A[i] = i;
    }

    for (int r = 0; r < round; r++) {
        auto start_time = std::chrono::high_resolution_clock::now();

        result = simpleLoop(A);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto runtime_ms = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

        printf("simple-loop Time: %f seconds\n", runtime_ms / 1e6);
        fprintf(stderr, "Result: %d\n", result);
    }
    return 0;
}