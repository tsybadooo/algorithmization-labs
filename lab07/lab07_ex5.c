/* Вправа 5. Множення матриць (з методичних вказівок) */
#include "../common_console.h"
#include <stdio.h>
#define MAXD 10
void read_matrix(int a[][MAXD], int r, int c, char name) {
    printf("Матриця %c (%dx%d):\n", name, r, c);
    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++) {
            printf("  %c[%d][%d] = ", name, i, j);
            scanf("%d", &a[i][j]);
        }
}
void print_matrix(int a[][MAXD], int r, int c) {
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) printf("%6d", a[i][j]);
        printf("\n");
    }
}
int main(void) {
    console_utf8();
    int rA, cA, rB, cB;
    printf("Розміри матриці A (рядки стовпці): "); scanf("%d %d",&rA,&cA);
    printf("Розміри матриці B (рядки стовпці): "); scanf("%d %d",&rB,&cB);
    if (cA != rB) {
        printf("Помилка: кількість стовпців A (%d) != рядків B (%d)\n",cA,rB);
        return 1;
    }
    int A[MAXD][MAXD], B[MAXD][MAXD], C[MAXD][MAXD];
    read_matrix(A, rA, cA, 'A');
    read_matrix(B, rB, cB, 'B');
    /* Множення C = A * B */
    for (int i = 0; i < rA; i++)
        for (int j = 0; j < cB; j++) {
            C[i][j] = 0;
            for (int k = 0; k < cA; k++)
                C[i][j] += A[i][k] * B[k][j];
        }
    printf("\nA (%dx%d):\n", rA, cA); print_matrix(A, rA, cA);
    printf("B (%dx%d):\n", rB, cB); print_matrix(B, rB, cB);
    printf("C = A*B (%dx%d):\n", rA, cB); print_matrix(C, rA, cB);
    return 0;
}
