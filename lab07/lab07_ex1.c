/* Вправа 1. Базові операції з одновимірним масивом (з методичних вказівок) */
#include "../common_console.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define N 10
void print_arr(int a[], int n) {
    printf("[ ");
    for (int i = 0; i < n; i++) printf("%4d", a[i]);
    printf(" ]\n");
}
void bubble_sort(int a[], int n) {
    for (int i = 0; i < n-1; i++)
        for (int j = 0; j < n-i-1; j++)
            if (a[j] > a[j+1]) {
                int t = a[j]; a[j] = a[j+1]; a[j+1] = t;
            }
}
int main(void) {
    console_utf8();
    srand((unsigned)time(NULL));
    int a[N];
    /* Заповнення випадковими числами від -50 до 50 */
    for (int i = 0; i < N; i++)
        a[i] = rand() % 101 - 50;
    printf("Масив:  "); print_arr(a, N);
    /* Статистика */
    int sum = 0, mn = a[0], mx = a[0];
    int neg = 0, pos = 0, zero = 0;
    for (int i = 0; i < N; i++) {
        sum += a[i];
        if (a[i] < mn) mn = a[i];
        if (a[i] > mx) mx = a[i];
        if      (a[i] < 0) neg++;
        else if (a[i] > 0) pos++;
        else               zero++;
    }
    printf("Сума: %d,  Середнє: %.2f\n", sum, (double)sum/N);
    printf("Мін: %d,  Макс: %d\n", mn, mx);
    printf("Від'ємних: %d, Нулів: %d, Додатніх: %d\n",
           neg, zero, pos);
    /* Сортування */
    bubble_sort(a, N);
    printf("Sorted: "); print_arr(a, N);
    /* Медіана */
    double median = (N % 2 == 0)
        ? (a[N/2-1] + a[N/2]) / 2.0
        : a[N/2];
    printf("Медіана: %.1f\n", median);
    return 0;
}
