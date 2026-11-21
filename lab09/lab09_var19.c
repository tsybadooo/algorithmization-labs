/*
 * Лабораторна робота №9. Індивідуальне завдання №19.
 * Написати функцію matrix_mul(int **a, int **b, int **c, int n) -
 * множення квадратних матриць через вказівники.
 *
 * Матриці зберігаються як масив вказівників на рядки (int **),
 * пам'ять виділяється динамічно; результат malloc перевіряється на NULL,
 * усі блоки звільняються, після free() вказівник обнуляється.
 * Доступ до елементів - через арифметику вказівників: *(*(a+i)+k).
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../common_console.h"

/* ---------- прототипи ---------- */
int **alloc_matrix(int n);
void free_matrix(int ***pm, int n);
void fill_matrix(int **m, int n, int mode);
void print_matrix(const char *name, int **m, int n);
void matrix_mul(int **a, int **b, int **c, int n);

int main(void)
{
    console_utf8();
    int n, mode;
    printf("Розмір квадратних матриць n (1..20): ");
    if (scanf("%d", &n) != 1 || n < 1 || n > 20) {
        printf("Помилка: n має бути від 1 до 20\n");
        return 1;
    }
    printf("Заповнення: 1 - з клавіатури, 2 - випадкові [-9; 9]: ");
    if (scanf("%d", &mode) != 1 || (mode != 1 && mode != 2)) {
        printf("Помилка вибору режиму\n");
        return 1;
    }
    srand((unsigned)time(NULL));

    int **a = alloc_matrix(n), **b = alloc_matrix(n), **c = alloc_matrix(n);
    if (a == NULL || b == NULL || c == NULL) {
        printf("Помилка: недостатньо пам'яті\n");
        free_matrix(&a, n); free_matrix(&b, n); free_matrix(&c, n);
        return 1;
    }
    if (mode == 1) printf("Введіть матрицю A (%d x %d):\n", n, n);
    fill_matrix(a, n, mode);
    if (mode == 1) printf("Введіть матрицю B (%d x %d):\n", n, n);
    fill_matrix(b, n, mode);

    matrix_mul(a, b, c, n);
    print_matrix("A", a, n);
    print_matrix("B", b, n);
    print_matrix("C = A * B", c, n);

    free_matrix(&a, n); free_matrix(&b, n); free_matrix(&c, n);
    printf("Пам'ять звільнено: a=%p b=%p c=%p\n", (void *)a, (void *)b, (void *)c);
    return 0;
}

/* Виділяє матрицю n x n як масив вказівників на рядки.
   Повертає вказівник або NULL (у разі помилки вже виділене звільняється). */
int **alloc_matrix(int n)
{
    int **m = (int **)malloc((size_t)n * sizeof(int *));
    if (m == NULL) return NULL;
    for (int i = 0; i < n; i++) {
        m[i] = (int *)calloc((size_t)n, sizeof(int));
        if (m[i] == NULL) {                    /* відкат: звільняємо попередні рядки */
            for (int j = 0; j < i; j++) free(m[j]);
            free(m);
            return NULL;
        }
    }
    return m;
}

/* Звільняє матрицю і встановлює вказівник у NULL (тому передається int ***). */
void free_matrix(int ***pm, int n)
{
    if (pm == NULL || *pm == NULL) return;
    for (int i = 0; i < n; i++) { free((*pm)[i]); (*pm)[i] = NULL; }
    free(*pm);
    *pm = NULL;
}

/* Заповнює матрицю: mode 1 - з клавіатури, mode 2 - випадковими числами. */
void fill_matrix(int **m, int n, int mode)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            if (mode == 1) {
                if (scanf("%d", *(m + i) + j) != 1) *(*(m + i) + j) = 0;
            } else {
                *(*(m + i) + j) = rand() % 19 - 9;
            }
        }
}

/* Друк матриці з вирівнюванням стовпців. */
void print_matrix(const char *name, int **m, int n)
{
    printf("%s:\n", name);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) printf("%6d", *(*(m + i) + j));
        printf("\n");
    }
}

/* Множення квадратних матриць: c[i][j] = сума a[i][k]*b[k][j], k=0..n-1.
   Усі звертання - через вказівники. Складність O(n^3). */
void matrix_mul(int **a, int **b, int **c, int n)
{
    for (int i = 0; i < n; i++) {
        int *ci = *(c + i);               /* вказівник на i-й рядок результату */
        for (int j = 0; j < n; j++) {
            int s = 0;
            for (int k = 0; k < n; k++)
                s += *(*(a + i) + k) * *(*(b + k) + j);
            *(ci + j) = s;
        }
    }
}
