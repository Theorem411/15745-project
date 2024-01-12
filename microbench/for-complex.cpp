#include <stdlib.h>
void loop(int A[10][10]) {
    for (int i = 0; i < 10; i+=2) {
        for (int j = 0; j < 10; j+=2) {
            A[i][i] = 0;
            A[i+j][0] = 1;
            A[2*(i + j)][0] = 1;
        }
    }
}