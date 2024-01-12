#include <cilk/cilk.h>
void loop(int A[3][10], int B[3][10]) {

    cilk_for (int i = 0; i < 3; ++i) {
        
        cilk_for (int j = 0; j < 10; ++j) {
            A[i][j] = B[j][0] + B[j + 1][0];
        }
    }
    
}

int main() {
    int A[3][10];
    int B[3][10];
    for (int i = 0; i < 3; i++) {
        // pf
        for (int j = 0; j < 10; j++) {
            // pf()
            A[i][j] = 0;
            B[i][j] = 0;
        }
    }
    loop(A, B);
    return 0;
}