/* Вправа 2. Динамічний масив з автозбільшенням (з методичних вказівок) */
#include "../common_console.h"
#include <stdio.h>
#include <stdlib.h>
int main(void) {
    console_utf8();
    int *arr     = NULL;
    int  size     = 0;
    int  capacity = 0;
    printf("Вводьте числа (0 — кінець):\n");
    int x;
    while (scanf("%d", &x) == 1 && x != 0) {
        /* Збільшити capacity якщо потрібно */
        if (size >= capacity) {
            int new_cap = capacity == 0 ? 4 : capacity * 2;
            int *tmp = (int *)realloc(arr, new_cap * sizeof(int));
            if (tmp == NULL) {
                printf("Помилка пам'яті!\n");
                free(arr);
                return 1;
            }
            arr = tmp;
            capacity = new_cap;
            printf("  [realloc: capacity=%d]\n", capacity);
        }
        arr[size++] = x;
    }
    printf("\nЗбережено %d чисел (capacity=%d):\n", size, capacity);
    for (int i = 0; i < size; i++) printf("%d ", arr[i]);
    printf("\n");
    /* Знайти мін та макс через вказівники */
    if (size > 0) {
        int *mn = arr, *mx = arr;
        for (int *p = arr + 1; p < arr + size; p++) {
            if (*p < *mn) mn = p;
            if (*p > *mx) mx = p;
        }
        printf("Мін=%d [%td]  Макс=%d [%td]\n",
               *mn, mn-arr, *mx, mx-arr);
    }
    free(arr);
    arr = NULL;
    return 0;
}
