/**
 * @file sort.h
 * @brief КПІЗ, частина 3: сортування з підрахунком операцій.
 * Варіант 19: бульбашкою (ітераційно) та швидке (рекурсивно).
 */
#ifndef SORT_H
#define SORT_H

#include <stddef.h>

/** Лічильники операцій сортування. */
typedef struct {
    unsigned long long comparisons; /**< кількість порівнянь елементів */
    unsigned long long swaps;       /**< кількість перестановок (обмінів) */
} SortStats;

/** Тип покажчика на функцію сортування. */
typedef void (*sort_fn)(int *arr, size_t n, SortStats *st);

/**
 * Сортування бульбашкою (ітераційне) за зростанням з ранньою зупинкою,
 * якщо за прохід не було обмінів. O(n^2) у середньому та гіршому, O(n) у кращому.
 * @param st лічильники (може бути NULL)
 */
void bubble_sort(int *arr, size_t n, SortStats *st);

/**
 * Швидке сортування (рекурсивне, схема Хоара, опорний - середній елемент).
 * O(n log n) у середньому, O(n^2) у гіршому випадку.
 * @param st лічильники (може бути NULL)
 */
void quick_sort(int *arr, size_t n, SortStats *st);

/** Перевіряє, чи впорядкований масив за неспаданням. */
int is_sorted(const int *arr, size_t n);

#endif
