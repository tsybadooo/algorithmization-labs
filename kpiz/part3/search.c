/**
 * @file search.c
 * @brief Реалізація лінійного та бінарного пошуку.
 */
#include "search.h"

long linear_search(const int *arr, size_t n, int key, unsigned long *comparisons)
{
    unsigned long c = 0;
    long found = -1;
    for (size_t i = 0; i < n; i++) {
        c++;
        if (arr[i] == key) { found = (long)i; break; }
    }
    if (comparisons != NULL) *comparisons = c;
    return found;
}

long binary_search(const int *arr, size_t n, int key, unsigned long *comparisons)
{
    unsigned long c = 0;
    long lo = 0, hi = (long)n - 1, found = -1;
    while (lo <= hi) {
        long mid = lo + (hi - lo) / 2;          /* без переповнення lo+hi */
        c++;
        if (arr[mid] == key) { found = mid; break; }
        if (arr[mid] < key) lo = mid + 1;
        else hi = mid - 1;
    }
    if (comparisons != NULL) *comparisons = c;
    return found;
}
