/* Вправа 1. Базові операції з вказівниками (з методичних вказівок) */
#include "../common_console.h"
#include <stdio.h>
void swap(int *a, int *b) {
    int t = *a; *a = *b; *b = t;
}
/* Сума через арифметику вказівників */
int sum_ptr(const int *arr, int n) {
    int s = 0;
    const int *end = arr + n;
    while (arr < end) s += *arr++;
    return s;
}
/* Повернути вказівник на максимум */
int *find_max(int *arr, int n) {
    int *mx = arr;
    for (int i = 1; i < n; i++)
        if (*(arr + i) > *mx) mx = arr + i;
    return mx;
}
int main(void) {
    console_utf8();
    int a = 5, b = 10;
    printf("До: a=%d b=%d\n", a, b);
    swap(&a, &b);
    printf("Після: a=%d b=%d\n\n", a, b);
    int arr[] = {7, 2, 9, 4, 6, 1, 8, 3};
    int n = 8;
    printf("Масив: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n");
    printf("Сума = %d\n", sum_ptr(arr, n));
    int *mx = find_max(arr, n);
    printf("Максимум = %d (індекс %td)\n\n", *mx, mx - arr);
    /* Арифметика вказівників */
    printf("Арифметика вказівників:\n");
    int *p = arr;
    for (int i = 0; i < 4; i++) {
        printf("  p+%d: адреса=%p  значення=%d\n",
               i, (void*)(p+i), *(p+i));
    }
    return 0;
}
