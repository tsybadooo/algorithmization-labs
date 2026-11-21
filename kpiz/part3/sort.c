/**
 * @file sort.c
 * @brief Реалізація алгоритмів сортування з лічильниками.
 */
#include "sort.h"

/** Обмін двох елементів з урахуванням лічильника. */
static void swap_int(int *a, int *b, SortStats *st)
{
    int t = *a;
    *a = *b;
    *b = t;
    if (st != NULL) st->swaps++;
}

/** Порівняння a > b з урахуванням лічильника. */
static int greater(int a, int b, SortStats *st)
{
    if (st != NULL) st->comparisons++;
    return a > b;
}

void bubble_sort(int *arr, size_t n, SortStats *st)
{
    if (arr == NULL || n < 2) return;
    for (size_t pass = 0; pass < n - 1; pass++) {
        int swapped = 0;
        for (size_t j = 0; j < n - 1 - pass; j++)
            if (greater(arr[j], arr[j + 1], st)) {
                swap_int(&arr[j], &arr[j + 1], st);
                swapped = 1;
            }
        if (!swapped) break;                    /* масив уже впорядкований */
    }
}

/** Рекурсивна частина швидкого сортування на відрізку [lo; hi]. */
static void quick_rec(int *arr, long lo, long hi, SortStats *st)
{
    if (lo >= hi) return;                       /* база рекурсії */
    int pivot = arr[lo + (hi - lo) / 2];
    long i = lo, j = hi;
    while (i <= j) {
        while (greater(pivot, arr[i], st)) i++;  /* arr[i] < pivot */
        while (greater(arr[j], pivot, st)) j--;  /* arr[j] > pivot */
        if (i <= j) {
            if (i != j) swap_int(&arr[i], &arr[j], st);
            i++;
            j--;
        }
    }
    quick_rec(arr, lo, j, st);                  /* ліва частина  */
    quick_rec(arr, i, hi, st);                  /* права частина */
}

void quick_sort(int *arr, size_t n, SortStats *st)
{
    if (arr == NULL || n < 2) return;
    quick_rec(arr, 0, (long)n - 1, st);
}

int is_sorted(const int *arr, size_t n)
{
    for (size_t i = 1; i < n; i++)
        if (arr[i - 1] > arr[i]) return 0;
    return 1;
}
