/**
 * @file main.c
 * @brief КПІЗ, частина 3 (варіант 19). Консольне меню; динамічна матриця n x m:
 *  - заповнення вручну або випадковими числами [-50; 50];
 *  - підрахунок локальних мінімумів;
 *  - сортування останнього стовпця бульбашкою (ітер.) та швидким (рекурс.),
 *    вибір алгоритму через покажчик на функцію;
 *  - лінійний і бінарний пошук у відсортованому стовпці;
 *  - експеримент: кількість порівнянь і перестановок для 10..10000 елементів.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "matrix.h"
#include "sort.h"
#include "search.h"
#include "../common/console.h"

#define MAX_DIM 50

/** Читає ціле з діапазону [lo; hi] з повторним запитом. 0 - кінець вводу. */
static int read_int(const char *prompt, int lo, int hi, int *value)
{
    for (;;) {
        printf("%s", prompt);
        int rc = scanf("%d", value);
        if (rc == EOF) return 0;
        if (rc == 1 && *value >= lo && *value <= hi) return 1;
        flush_line();                 /* відкинути помилковий залишок рядка */
        printf("  Помилка: потрібне ціле число від %d до %d.\n", lo, hi);
    }
}

/** Заповнення матриці з клавіатури з перевіркою кожного елемента. */
static int fill_manual(Matrix *mt)
{
    for (size_t i = 0; i < mt->rows; i++)
        for (size_t j = 0; j < mt->cols; j++) {
            char prompt[64];
            snprintf(prompt, sizeof prompt, "  a[%zu][%zu] = ", i, j);
            if (!read_int(prompt, -1000000, 1000000, &mt->data[i][j])) return 0;
        }
    return 1;
}

/** Виводить масив у рядок. */
static void print_array(const char *title, const int *a, size_t n)
{
    printf("%s", title);
    for (size_t i = 0; i < n; i++) printf("%5d", a[i]);
    printf("\n");
}

/**
 * Експеримент зі складністю: для розмірів 10, 100, 1000, 10000 сортує
 * однакові випадкові масиви обома алгоритмами, друкує таблицю і
 * записує її у файл CSV для побудови графіка.
 */
static void complexity_experiment(sort_fn algs[], const char *names[], const char *csv_name)
{
    const size_t sizes[] = {10, 100, 1000, 10000};
    FILE *csv = fopen(csv_name, "w");
    if (csv == NULL) printf("  Попередження: не вдалося створити %s\n", csv_name);
    else fprintf(csv, "n,algorithm,comparisons,swaps\n");

    printf("\n+--------+--------------+-----------------+-----------------+\n");
    printf("|   n    |   алгоритм   |   порівняння    |  перестановки   |\n");
    printf("+--------+--------------+-----------------+-----------------+\n");
    for (size_t s = 0; s < sizeof sizes / sizeof sizes[0]; s++) {
        size_t n = sizes[s];
        int *base = (int *)malloc(n * sizeof(int));
        int *work = (int *)malloc(n * sizeof(int));
        if (base == NULL || work == NULL) {
            printf("  Помилка: недостатньо пам'яті для n = %zu\n", n);
            free(base); free(work);
            break;
        }
        for (size_t i = 0; i < n; i++) base[i] = rand() % 101 - 50;
        for (int a = 0; a < 2; a++) {
            SortStats st = {0, 0};
            memcpy(work, base, n * sizeof(int));
            algs[a](work, n, &st);                 /* виклик через покажчик на функцію */
            /* %-12s рахує байти, а кирилиця в UTF-8 займає 2 байти на літеру,
               тому доповнюємо пробілами вручну за кількістю символів */
            int pad = 12;
            for (const char *p = names[a]; *p; p++)
                if (((unsigned char)*p & 0xC0) != 0x80) pad--;
            printf("| %6zu | %s%*s | %15llu | %15llu |%s\n", n, names[a], pad > 0 ? pad : 0, "",
                   st.comparisons, st.swaps, is_sorted(work, n) ? "" : " ПОМИЛКА");
            if (csv != NULL) fprintf(csv, "%zu,%s,%llu,%llu\n", n, a == 0 ? "bubble" : "quick",
                                     st.comparisons, st.swaps);
        }
        free(base);
        free(work);
    }
    printf("+--------+--------------+-----------------+-----------------+\n");
    printf("Теоретично: бульбашка ~ n(n-1)/2 порівнянь (O(n^2)), швидке ~ 1.39 n log2 n (O(n log n)).\n");
    if (csv != NULL) { fclose(csv); printf("Таблицю збережено у файл %s\n", csv_name); }
}

int main(void)
{
    console_utf8();
    srand((unsigned)time(NULL));
    sort_fn algs[2] = {bubble_sort, quick_sort};
    const char *names[2] = {"бульбашка", "швидке"};

    printf("КПІЗ. Частина 3. Варіант 19\n");
    Matrix mt = {NULL, 0, 0};
    int choice = -1;

    while (choice != 0) {
        printf("\nМЕНЮ\n"
               " 1 - створити і заповнити матрицю\n"
               " 2 - вивести матрицю\n"
               " 3 - знайти локальні мінімуми\n"
               " 4 - відсортувати останній стовпець (вибір алгоритму)\n"
               " 5 - порівняти обидва алгоритми на останньому стовпці\n"
               " 6 - пошук значення в останньому стовпці\n"
               " 7 - експеримент зі складністю (10..10000 елементів)\n"
               " 0 - вихід\n");
        if (!read_int("Ваш вибір: ", 0, 7, &choice)) break;
        if (choice >= 2 && choice <= 6 && mt.data == NULL) {
            printf("Спочатку створіть матрицю (пункт 1).\n");
            continue;
        }
        switch (choice) {
            case 1: {
                int n, m, mode;
                if (!read_int("Кількість рядків n (1..50): ", 1, MAX_DIM, &n)) { choice = 0; break; }
                if (!read_int("Кількість стовпців m (1..50): ", 1, MAX_DIM, &m)) { choice = 0; break; }
                matrix_free(&mt);                       /* стара матриця, якщо була */
                if (!matrix_create(&mt, (size_t)n, (size_t)m)) {
                    printf("Помилка: не вдалося виділити пам'ять для матриці\n");
                    break;
                }
                if (!read_int("Заповнення: 1 - вручну, 2 - випадкові [-50; 50]: ", 1, 2, &mode)) { choice = 0; break; }
                if (mode == 1) {
                    if (!fill_manual(&mt)) { choice = 0; break; }
                } else {
                    matrix_fill_random(&mt, -50, 50);
                }
                printf("Матрицю %d x %d створено.\n", n, m);
                matrix_print(&mt);
                break;
            }
            case 2:
                matrix_print(&mt);
                break;
            case 3: {
                size_t total = mt.rows * mt.cols;
                Cell *cells = (Cell *)malloc(total * sizeof(Cell));
                if (cells == NULL) { printf("Помилка пам'яті\n"); break; }
                size_t cnt = count_local_minima(&mt, cells, total);
                printf("Кількість локальних мінімумів: %zu\n", cnt);
                for (size_t k = 0; k < cnt; k++)
                    printf("  a[%zu][%zu] = %d\n", cells[k].row, cells[k].col, mt.data[cells[k].row][cells[k].col]);
                free(cells);
                break;
            }
            case 4: {
                int alg;
                if (!read_int("Алгоритм: 1 - бульбашка (ітераційний), 2 - швидке (рекурсивний): ", 1, 2, &alg)) { choice = 0; break; }
                int *col = (int *)malloc(mt.rows * sizeof(int));
                if (col == NULL) { printf("Помилка пам'яті\n"); break; }
                matrix_get_last_col(&mt, col);
                print_array("Останній стовпець до сортування:   ", col, mt.rows);
                SortStats st = {0, 0};
                sort_fn chosen = algs[alg - 1];         /* покажчик на обрану функцію */
                chosen(col, mt.rows, &st);
                print_array("Останній стовпець після сортування:", col, mt.rows);
                printf("Алгоритм: %s; порівнянь: %llu; перестановок: %llu\n", names[alg - 1], st.comparisons, st.swaps);
                matrix_set_last_col(&mt, col);
                free(col);
                printf("Матриця після сортування останнього стовпця:\n");
                matrix_print(&mt);
                break;
            }
            case 5: {
                /* обидва алгоритми на однакових копіях стовпця; матриця не змінюється */
                int *base = (int *)malloc(mt.rows * sizeof(int));
                int *work = (int *)malloc(mt.rows * sizeof(int));
                if (base == NULL || work == NULL) { printf("Помилка пам'яті\n"); free(base); free(work); break; }
                matrix_get_last_col(&mt, base);
                print_array("Останній стовпець:  ", base, mt.rows);
                for (int a = 0; a < 2; a++) {
                    SortStats st = {0, 0};
                    memcpy(work, base, mt.rows * sizeof(int));
                    algs[a](work, mt.rows, &st);
                    printf("%s", a == 0 ? "Бульбашка:  " : "Швидке:     ");   /* вирівняно вручну (UTF-8) */
                    print_array(" ", work, mt.rows);
                    printf("             порівнянь: %llu, перестановок: %llu\n", st.comparisons, st.swaps);
                }
                free(base);
                free(work);
                break;
            }
            case 6: {
                int *col = (int *)malloc(mt.rows * sizeof(int));
                if (col == NULL) { printf("Помилка пам'яті\n"); break; }
                matrix_get_last_col(&mt, col);
                if (!is_sorted(col, mt.rows))
                    printf("Увага: стовпець не відсортовано - бінарний пошук може бути некоректним (виконайте пункт 4).\n");
                int key;
                if (read_int("Значення для пошуку: ", -1000000, 1000000, &key)) {
                    unsigned long cl, cb;
                    long il = linear_search(col, mt.rows, key, &cl);
                    long ib = binary_search(col, mt.rows, key, &cb);
                    printf("Лінійний пошук: індекс %ld, порівнянь %lu\n", il, cl);
                    printf("Бінарний пошук: індекс %ld, порівнянь %lu\n", ib, cb);
                }
                free(col);
                break;
            }
            case 7:
                complexity_experiment(algs, names, "complexity.csv");
                break;
            case 0:
                printf("Завершення роботи.\n");
                break;
            default:
                break;
        }
    }
    matrix_free(&mt);
    return 0;
}
