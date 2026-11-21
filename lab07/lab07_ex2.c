/* Вправа 2. Пошук у масиві: лінійний та бінарний (з методичних вказівок) */
#include "../common_console.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define N 15
int linear_search(int a[], int n, int x) {
    for (int i = 0; i < n; i++)
        if (a[i] == x) return i;
    return -1;
}
int binary_search(int a[], int n, int x) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        printf("  Перевіряємо a[%d]=%d\n", mid, a[mid]);
        if      (a[mid] == x) return mid;
        else if (a[mid] < x)  lo = mid + 1;
        else                  hi = mid - 1;
    }
    return -1;
}
void qsort_arr(int a[], int n) {  /* простий qsort через stdlib */
    /* використаємо бульбашку для простоти */
    for (int i=0;i<n-1;i++)
        for (int j=0;j<n-i-1;j++)
            if(a[j]>a[j+1]){int t=a[j];a[j]=a[j+1];a[j+1]=t;}
}
int main(void) {
    console_utf8();
    srand(42);
    int a[N];
    for (int i = 0; i < N; i++) a[i] = rand() % 50;
    printf("До сортування: ");
    for (int i = 0; i < N; i++) printf("%3d", a[i]);
    printf("\n");
    qsort_arr(a, N);
    printf("Після сортув.: ");
    for (int i = 0; i < N; i++) printf("%3d", a[i]);
    printf("\n");
    int target;
    printf("Шукаємо: "); scanf("%d", &target);
    int idx_l = linear_search(a, N, target);
    printf("Лінійний пошук: %d\n",
           idx_l >= 0 ? idx_l : -1);
    printf("Бінарний пошук (кроки):\n");
    int idx_b = binary_search(a, N, target);
    printf("Результат: %s (індекс %d)\n",
           idx_b >= 0 ? "знайдено" : "не знайдено", idx_b);
    return 0;
}
