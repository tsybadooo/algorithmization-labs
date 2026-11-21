/* Вправа 5. Динамічна матриця та операції (з методичних вказівок) */
#include "../common_console.h"
#include <stdio.h>
#include <stdlib.h>
int **alloc_matrix(int n) {
    int **m = (int **)malloc(n * sizeof(int *));
    if (!m) return NULL;
    for (int i = 0; i < n; i++) {
        m[i] = (int *)malloc(n * sizeof(int));
        if (!m[i]) {
            for (int j = 0; j < i; j++) free(m[j]);
            free(m); return NULL;
        }
    }
    return m;
}
void free_matrix(int **m, int n) {
    for (int i = 0; i < n; i++) free(m[i]);
    free(m);
}
void print_matrix(int **m, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) printf("%5d", m[i][j]);
        printf("\n");
    }
}
int main(void) {
    console_utf8();
    int n = 4;
    int **mat = alloc_matrix(n);
    if (!mat) { perror("alloc"); return 1; }
    /* Заповнити */
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            mat[i][j] = i * n + j + 1;
    printf("Матриця:\n"); print_matrix(mat, n);
    /* Суми діагоналей */
    int d1 = 0, d2 = 0;
    for (int i = 0; i < n; i++) {
        d1 += mat[i][i];
        d2 += mat[i][n-1-i];
    }
    printf("Головна діагональ: %d\n", d1);
    printf("Побічна діагональ: %d\n", d2);
    /* Транспонування */
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            int t = mat[i][j]; mat[i][j] = mat[j][i]; mat[j][i] = t;
        }
    printf("\nТранспонована:\n"); print_matrix(mat, n);
    free_matrix(mat, n);
    return 0;
}
