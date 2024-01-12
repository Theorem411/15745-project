#include <cilk/cilk.h>
void loop(int A[10], int B[10]) {
    cilk_for (int i = 0; i < 10; ++i) {
        A[i] = B[i] + B[i+1];
    }
}