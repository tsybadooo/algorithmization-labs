/* Вправа 4. Функції для масивів — обробка через вказівники (з методичних вказівок) */
#include "../common_console.h"
#include <stdio.h>
#include <math.h>
/* Бібліотека функцій для масивів */
void   arr_print(const int *a, int n, const char *label);
int    arr_sum(const int *a, int n);
double arr_avg(const int *a, int n);
int    arr_min(const int *a, int n);
int    arr_max(const int *a, int n);
int    arr_count_if(const int *a, int n, int (*pred)(int));
void   arr_sort_bubble(int *a, int n);
void arr_print(const int *a, int n, const char *label) {
    printf("%s [ ", label);
    for (int i = 0; i < n; i++) printf("%d ", a[i]);
    printf("]\n");
}
int arr_sum(const int *a, int n) {
    int s = 0;
    for (int i = 0; i < n; i++) s += a[i];
    return s;
}
double arr_avg(const int *a, int n) {
    return n > 0 ? (double)arr_sum(a, n) / n : 0.0;
}
int arr_min(const int *a, int n) {
    int m = a[0];
    for (int i = 1; i < n; i++) if (a[i] < m) m = a[i];
    return m;
}
int arr_max(const int *a, int n) {
    int m = a[0];
    for (int i = 1; i < n; i++) if (a[i] > m) m = a[i];
    return m;
}
/* Підрахувати елементи, що задовольняють предикату */
int arr_count_if(const int *a, int n, int (*pred)(int)) {
    int c = 0;
    for (int i = 0; i < n; i++) if (pred(a[i])) c++;
    return c;
}
void arr_sort_bubble(int *a, int n) {
    for (int i = 0; i < n-1; i++)
        for (int j = 0; j < n-1-i; j++)
            if (a[j] > a[j+1]) {
                int t = a[j]; a[j] = a[j+1]; a[j+1] = t;
            }
}
/* Предикати */
int is_even(int x)     { return x % 2 == 0; }
int is_positive(int x) { return x > 0; }
int is_prime_p(int x)  {
    if (x < 2) return 0;
    for (int i = 2; (long long)i*i <= x; i++)
        if (x % i == 0) return 0;
    return 1;
}
int main(void) {
    console_utf8();
    int arr[] = {7, -3, 15, 2, 11, -8, 4, 23, 6, 1};
    int n = 10;
    arr_print(arr, n, "Масив:");
    printf("Сума: %d\n", arr_sum(arr, n));
    printf("Середнє: %.2f\n", arr_avg(arr, n));
    printf("Мін: %d, Макс: %d\n", arr_min(arr, n), arr_max(arr, n));
    printf("Парних: %d\n",   arr_count_if(arr, n, is_even));
    printf("Додатніх: %d\n", arr_count_if(arr, n, is_positive));
    printf("Простих: %d\n",  arr_count_if(arr, n, is_prime_p));
    arr_sort_bubble(arr, n);
    arr_print(arr, n, "Відсортований:");
    return 0;
}
