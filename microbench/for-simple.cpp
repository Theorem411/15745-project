#include <stdio.h>
#include <stdlib.h>

// void init(int **A, int **B, int rows, int cols) {
//     for (int i = 0; i < rows; ++i) {
//         A[i] = (int *)calloc(cols, sizeof(int));
//         for (int j = 0; j < cols; ++j) {
//             A[i][j] = 0;
//         }
//     }

//     for (int i = 0; i < cols; ++i) {
//         B[i] = (int *)calloc(rows, sizeof(int));
//         for (int j = i; j < rows; ++j) {
//             B[i][j] = 1;
//         }
//     }
// }

void loop(int A[10][10], int B[10][10], int N) {
    // do operations: transpose
    for (int i = 0; i < N; ++i) {
        if (B[0][i] < 10) {
            A[i][0] = 1;
        } else {
            A[i][0] = 0;
        }
    }
    return; 
}