/* Вправа 3. const-вказівники та передача масивів (з методичних вказівок) */
#include "../common_console.h"
#include <stdio.h>
#include <string.h>
/* const int* — не можна змінити *p (захист даних) */
int sum_const(const int *arr, int n) {
    int s = 0;
    for (int i = 0; i < n; i++) {
        s += arr[i];
        /* arr[i] = 0; -- ПОМИЛКА компіляції */
    }
    return s;
}
/* Копіювання рядка без strcpy */
char *my_strcpy(char *dst, const char *src) {
    char *d = dst;
    while ((*d++ = *src++) != '\0');
    return dst;
}
/* Довжина рядка без strlen */
size_t my_strlen(const char *s) {
    const char *p = s;
    while (*p) p++;
    return p - s;
}
int main(void) {
    console_utf8();
    int arr[] = {5, 3, 8, 1, 9};
    int n = 5;
    printf("Сума = %d\n", sum_const(arr, n));
    /* const int * vs int * const */
    int x = 10, y = 20;
    const int *p1 = &x;  /* не можна: *p1 = 5; */
    int * const p2 = &x; /* не можна: p2 = &y; */
    printf("p1 вказує на %d\n", *p1);
    p1 = &y;    /* можна змінити адресу */
    printf("p1 тепер вказує на %d\n", *p1);
    *p2 = 99;   /* можна змінити значення */
    printf("x = %d\n", x);
    /* Рядкові функції */
    char src[] = "Hello World";
    char dst[50];
    my_strcpy(dst, src);
    printf("Копія: %s\n", dst);
    printf("Довжина: %zu  (strlen: %zu)\n",
           my_strlen(src), strlen(src));
    return 0;
}
