/*
 * Лабораторна робота №7. Індивідуальне завдання №19 (таблиця 3.1).
 * Знайти в масиві пару елементів з найменшою різницею.
 * Вивести пару та різницю.
 *
 * Алгоритм: перебір усіх пар (i < j), різниця |a[i] - a[j]|,
 * запам'ятовується пара з мінімальною різницею. Складність O(n^2).
 * Додатково для перевірки: копію масиву сортуємо вставками - тоді
 * найближча пара обов'язково стоїть поруч, і мінімум шукається за O(n).
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../common_console.h"

#define MAX 100

static void print_arr(const char *title, const int a[], int n)
{
    printf("%s[", title);
    for (int i = 0; i < n; i++) printf("%5d", a[i]);
    printf(" ]\n");
}

int main(void)
{
    console_utf8();
    int a[MAX], s[MAX], n, mode;

    printf("Кількість елементів (2..%d): ", MAX);
    if (scanf("%d", &n) != 1 || n < 2 || n > MAX) {
        printf("Помилка: n має бути від 2 до %d\n", MAX);
        return 1;
    }
    printf("Заповнення: 1 - з клавіатури, 2 - випадкові [-50; 50]: ");
    if (scanf("%d", &mode) != 1 || (mode != 1 && mode != 2)) {
        printf("Помилка вибору режиму\n");
        return 1;
    }
    if (mode == 1) {
        printf("Введіть %d цілих чисел: ", n);
        for (int i = 0; i < n; i++)
            if (scanf("%d", &a[i]) != 1) { printf("Помилка введення\n"); return 1; }
    } else {
        srand((unsigned)time(NULL));
        for (int i = 0; i < n; i++) a[i] = rand() % 101 - 50;
    }
    print_arr("Масив до обробки:     ", a, n);

    /* --- основний алгоритм: перебір усіх пар --- */
    int bi = 0, bj = 1;
    int best = abs(a[0] - a[1]);
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++) {
            int d = abs(a[i] - a[j]);
            if (d < best) { best = d; bi = i; bj = j; }
        }
    printf("Пара з найменшою різницею: a[%d] = %d, a[%d] = %d\n", bi, a[bi], bj, a[bj]);
    printf("Різниця: %d\n", best);

    /* --- перевірка через відсортовану копію (сортування вставками) --- */
    for (int i = 0; i < n; i++) s[i] = a[i];
    for (int i = 1; i < n; i++) {
        int key = s[i], j = i - 1;
        while (j >= 0 && s[j] > key) { s[j + 1] = s[j]; j--; }
        s[j + 1] = key;
    }
    int best2 = s[1] - s[0];
    for (int i = 1; i < n - 1; i++)
        if (s[i + 1] - s[i] < best2) best2 = s[i + 1] - s[i];
    print_arr("Відсортована копія:   ", s, n);
    printf("Перевірка (сусідні у відсортованому): мінімальна різниця = %d -> %s\n",
           best2, best2 == best ? "збігається" : "ПОМИЛКА");
    print_arr("Масив після обробки:  ", a, n);   /* вихідний масив не змінено */
    return 0;
}
