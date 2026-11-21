/**
 * @file search.h
 * @brief КПІЗ, частина 3: лінійний і бінарний пошук.
 */
#ifndef SEARCH_H
#define SEARCH_H

#include <stddef.h>

/**
 * Лінійний пошук значення key у масиві.
 * @param comparisons [out] кількість порівнянь (може бути NULL)
 * @return індекс першого входження або -1
 */
long linear_search(const int *arr, size_t n, int key, unsigned long *comparisons);

/**
 * Бінарний пошук у масиві, відсортованому за зростанням.
 * @param comparisons [out] кількість порівнянь (може бути NULL)
 * @return індекс знайденого елемента або -1
 */
long binary_search(const int *arr, size_t n, int key, unsigned long *comparisons);

#endif
