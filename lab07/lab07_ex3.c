/* Вправа 3. Операції з матрицею (з методичних вказівок) */
#include "../common_console.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define ROWS 4
#define COLS 4
void print_matrix(int a[][COLS], int r, int c) {
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++)
            printf("%5d", a[i][j]);
        printf("\n");
    }
}
int main(void) {
    console_utf8();
    srand(7);
    int m[ROWS][COLS];
    /* Заповнення */
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            m[i][j] = rand() % 20 - 5;
    printf("Матриця %dx%d:\n", ROWS, COLS);
    print_matrix(m, ROWS, COLS);
    /* Суми рядків */
    printf("\nСуми рядків: ");
    for (int i = 0; i < ROWS; i++) {
        int s = 0;
        for (int j = 0; j < COLS; j++) s += m[i][j];
        printf("%d ", s);
    }
    /* Суми стовпців */
    printf("\nСуми стовп.: ");
    for (int j = 0; j < COLS; j++) {
        int s = 0;
        for (int i = 0; i < ROWS; i++) s += m[i][j];
        printf("%d ", s);
    }
    /* Максимум і його координати */
    int mx = m[0][0], ri = 0, ci = 0;
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            if (m[i][j] > mx) { mx = m[i][j]; ri = i; ci = j; }
    printf("\nМакс: %d на позиції [%d][%d]\n", mx, ri, ci);
    /* Транспонування (квадратна матриця) */
    for (int i = 0; i < ROWS; i++)
        for (int j = i+1; j < COLS; j++) {
            int t = m[i][j]; m[i][j] = m[j][i]; m[j][i] = t;
        }
    printf("\nТранспонована:\n");
    print_matrix(m, ROWS, COLS);
    return 0;
}
