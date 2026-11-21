/**
 * @file matrix.h
 * @brief КПІЗ, частина 3: динамічна матриця цілих чисел.
 * Варіант 19: підрахунок локальних мінімумів.
 */
#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>

/** Динамічна матриця: масив покажчиків на рядки. */
typedef struct {
    int  **data;  /**< data[i][j] - елемент i-го рядка, j-го стовпця */
    size_t rows;  /**< кількість рядків n  */
    size_t cols;  /**< кількість стовпців m */
} Matrix;

/** Координати елемента матриці. */
typedef struct {
    size_t row, col;
} Cell;

/**
 * Створює матрицю rows x cols, заповнену нулями (calloc).
 * @return 1 - успішно, 0 - помилка параметрів або нестача пам'яті
 */
int matrix_create(Matrix *mt, size_t rows, size_t cols);

/** Звільняє пам'ять матриці; після виклику mt->data == NULL. */
void matrix_free(Matrix *mt);

/**
 * Заповнює матрицю випадковими числами з діапазону [lo; hi].
 * Генератор rand() має бути ініціалізований викликом srand().
 */
void matrix_fill_random(Matrix *mt, int lo, int hi);

/** Друкує матрицю з вирівнюванням стовпців. */
void matrix_print(const Matrix *mt);

/**
 * Перевіряє, чи є елемент [r][c] локальним мінімумом: він строго
 * менший за всіх своїх сусідів (до 8 сусідів, з урахуванням меж).
 * Матриця 1x1: єдиний елемент сусідів не має і вважається мінімумом.
 */
int is_local_min(const Matrix *mt, size_t r, size_t c);

/**
 * Підраховує локальні мінімуми матриці.
 * @param mt     матриця
 * @param cells  [out] масив для координат знайдених мінімумів (може бути NULL)
 * @param max_cells розмір масиву cells
 * @return кількість локальних мінімумів
 */
size_t count_local_minima(const Matrix *mt, Cell *cells, size_t max_cells);

/**
 * Копіює останній стовпець матриці у масив dst (довжина rows).
 * @return 1 - успішно, 0 - порожня матриця
 */
int matrix_get_last_col(const Matrix *mt, int *dst);

/** Записує масив src (довжина rows) в останній стовпець матриці. */
void matrix_set_last_col(Matrix *mt, const int *src);

#endif
