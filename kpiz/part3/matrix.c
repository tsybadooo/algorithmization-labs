/**
 * @file matrix.c
 * @brief Реалізація операцій з динамічною матрицею.
 */
#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"

int matrix_create(Matrix *mt, size_t rows, size_t cols)
{
    if (mt == NULL) return 0;
    mt->data = NULL;
    mt->rows = mt->cols = 0;
    if (rows == 0 || cols == 0) return 0;

    int **d = (int **)malloc(rows * sizeof(int *));
    if (d == NULL) return 0;
    for (size_t i = 0; i < rows; i++) {
        d[i] = (int *)calloc(cols, sizeof(int));
        if (d[i] == NULL) {                  /* відкат уже виділених рядків */
            for (size_t k = 0; k < i; k++) free(d[k]);
            free(d);
            return 0;
        }
    }
    mt->data = d;
    mt->rows = rows;
    mt->cols = cols;
    return 1;
}

void matrix_free(Matrix *mt)
{
    if (mt == NULL || mt->data == NULL) return;
    for (size_t i = 0; i < mt->rows; i++) free(mt->data[i]);
    free(mt->data);
    mt->data = NULL;
    mt->rows = mt->cols = 0;
}

void matrix_fill_random(Matrix *mt, int lo, int hi)
{
    for (size_t i = 0; i < mt->rows; i++)
        for (size_t j = 0; j < mt->cols; j++)
            mt->data[i][j] = lo + rand() % (hi - lo + 1);
}

void matrix_print(const Matrix *mt)
{
    printf("       ");
    for (size_t j = 0; j < mt->cols; j++) printf("%5zu", j);
    printf("\n");
    for (size_t i = 0; i < mt->rows; i++) {
        printf("  %3zu |", i);
        for (size_t j = 0; j < mt->cols; j++) printf("%5d", mt->data[i][j]);
        printf("\n");
    }
}

int is_local_min(const Matrix *mt, size_t r, size_t c)
{
    int v = mt->data[r][c];
    /* Межі вікна 3x3 навколо елемента з урахуванням країв матриці.
       Використовуємо беззнакові індекси, тому r-1 обчислюємо лише при r > 0. */
    size_t r0 = (r > 0) ? r - 1 : 0;
    size_t r1 = (r + 1 < mt->rows) ? r + 1 : r;
    size_t c0 = (c > 0) ? c - 1 : 0;
    size_t c1 = (c + 1 < mt->cols) ? c + 1 : c;

    for (size_t i = r0; i <= r1; i++)
        for (size_t j = c0; j <= c1; j++) {
            if (i == r && j == c) continue;          /* сам елемент пропускаємо */
            if (mt->data[i][j] <= v) return 0;       /* знайдено не більшого сусіда */
        }
    return 1;
}

size_t count_local_minima(const Matrix *mt, Cell *cells, size_t max_cells)
{
    size_t count = 0;
    if (mt == NULL || mt->data == NULL) return 0;
    for (size_t i = 0; i < mt->rows; i++)
        for (size_t j = 0; j < mt->cols; j++)
            if (is_local_min(mt, i, j)) {
                if (cells != NULL && count < max_cells) {
                    cells[count].row = i;
                    cells[count].col = j;
                }
                count++;
            }
    return count;
}

int matrix_get_last_col(const Matrix *mt, int *dst)
{
    if (mt == NULL || mt->data == NULL || dst == NULL) return 0;
    for (size_t i = 0; i < mt->rows; i++) dst[i] = mt->data[i][mt->cols - 1];
    return 1;
}

void matrix_set_last_col(Matrix *mt, const int *src)
{
    for (size_t i = 0; i < mt->rows; i++) mt->data[i][mt->cols - 1] = src[i];
}
