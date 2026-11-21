/*
 * Лабораторна робота №6 "Програмування циклічних алгоритмів". Варіант 19.
 *
 * Дано натуральне число n. Обчислити
 *
 *     S = sqrt(2 + sqrt(2 + ... + sqrt(2)))      (n коренів)
 *
 * Рекурентне співвідношення: s1 = sqrt(2),  s(k) = sqrt(2 + s(k-1)).
 * Обчислення виконано трьома видами циклів (for, while, do-while),
 * результати мають збігатися. Для контролю виведено також
 * відоме точне значення 2*cos(pi / 2^(n+1)); при n -> ∞ S -> 2.
 */
#include <stdio.h>
#include <math.h>
#include "../common_console.h"

int main(void)
{
    console_utf8();
    const double PI = 3.14159265358979323846;
    int n, k;
    double s_for, s_while, s_do;

    printf("Введіть натуральне n: ");
    if (scanf("%d", &n) != 1 || n < 1) {
        printf("Помилка: n має бути натуральним числом (n >= 1)\n");
        return 1;
    }

    /* 1) цикл for: s починається з 0, кожен крок додає один корінь */
    s_for = 0.0;
    for (k = 1; k <= n; k++)
        s_for = sqrt(2.0 + s_for);

    /* 2) цикл while (з передумовою) */
    s_while = 0.0;
    k = 0;
    while (k < n) {
        s_while = sqrt(2.0 + s_while);
        k++;
    }

    /* 3) цикл do-while (з післяумовою); n >= 1, тому хоча б одна ітерація коректна */
    s_do = 0.0;
    k = 0;
    do {
        s_do = sqrt(2.0 + s_do);
        k++;
    } while (k < n);

    printf("n = %d\n", n);
    printf("S (for)      = %.12f\n", s_for);
    printf("S (while)    = %.12f\n", s_while);
    printf("S (do-while) = %.12f\n", s_do);
    printf("Контроль 2*cos(pi/2^(n+1)) = %.12f\n", 2.0 * cos(PI / pow(2.0, n + 1)));

    /* Таблиця проміжних значень для перших кроків */
    printf("\n  k |      s(k)\n ---+----------------\n");
    s_for = 0.0;
    for (k = 1; k <= n && k <= 10; k++) {
        s_for = sqrt(2.0 + s_for);
        printf(" %2d | %.12f\n", k, s_for);
    }
    return 0;
}
