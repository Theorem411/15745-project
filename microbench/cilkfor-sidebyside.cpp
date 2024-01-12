#include <cilk/cilk.h>

void loop(int A[10], int x) {
    auto B = new int [10][10];
    if (x > 100) {
        for (int i = 0; i < x; ++i) {
            for (int j = 0; j < x; ++j) {
                B[1 + i - 2 * j][i - j + 2] = B[i][j];
            }
        }
    } else {
        cilk_for(int i = 0; i < x; ++i) {
            A[i]++;
        }
    }
}