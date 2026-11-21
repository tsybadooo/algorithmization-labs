/* Вправа 4. Сортування через вказівник на функцію (з методичних вказівок) */
#include "../common_console.h"
#include <stdio.h>
#include <stdlib.h>
/* Компаратори */
int cmp_asc(const void *a, const void *b) {
    return *(int*)a - *(int*)b;
}
int cmp_desc(const void *a, const void *b) {
    return *(int*)b - *(int*)a;
}
int cmp_abs(const void *a, const void *b) {
    int va = *(int*)a < 0 ? -*(int*)a : *(int*)a;
    int vb = *(int*)b < 0 ? -*(int*)b : *(int*)b;
    return va - vb;
}
void print_arr(const int *arr, int n) {
    for (int i = 0; i < n; i++) printf("%4d", arr[i]);
    printf("\n");
}
int main(void) {
    console_utf8();
    int arr[] = {5, -3, 8, -1, 9, -7, 2, 4};
    int n = 8;
    int tmp[8];
    printf("Оригінал:    "); print_arr(arr, n);
    for (int i = 0; i < n; i++) tmp[i] = arr[i];
    qsort(tmp, n, sizeof(int), cmp_asc);
    printf("Зростання:   "); print_arr(tmp, n);
    for (int i = 0; i < n; i++) tmp[i] = arr[i];
    qsort(tmp, n, sizeof(int), cmp_desc);
    printf("Спадання:    "); print_arr(tmp, n);
    for (int i = 0; i < n; i++) tmp[i] = arr[i];
    qsort(tmp, n, sizeof(int), cmp_abs);
    printf("За модулем:  "); print_arr(tmp, n);
    return 0;
}
