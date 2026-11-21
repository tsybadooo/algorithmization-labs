/*
 * Лабораторна робота №7. Індивідуальне завдання №19 (таблиця 3.2).
 * Дано масив. Переставити елементи так, щоб парні стояли перед
 * непарними, зберігаючи відносний порядок.
 *
 * Алгоритм (стабільний, на місці): прохід зліва направо; кожен
 * знайдений парний елемент "вставляється" одразу після вже
 * зібраних парних - елементи між ними зсуваються вправо на 1
 * (як у сортуванні вставками). Відносний порядок і парних, і
 * непарних зберігається. Складність O(n^2), додаткова пам'ять O(1).
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
    int a[MAX], n, mode;

    printf("Кількість елементів (1..%d): ", MAX);
    if (scanf("%d", &n) != 1 || n < 1 || n > MAX) {
        printf("Помилка: n має бути від 1 до %d\n", MAX);
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
    print_arr("До обробки:    ", a, n);

    int k = 0;                      /* кількість уже переставлених парних */
    for (int i = 0; i < n; i++) {
        if (a[i] % 2 == 0) {        /* працює і для від'ємних: -4 % 2 == 0 */
            int t = a[i];
            for (int j = i; j > k; j--) a[j] = a[j - 1];   /* зсув вправо */
            a[k] = t;
            k++;
        }
    }
    print_arr("Після обробки: ", a, n);
    printf("Парних: %d, непарних: %d\n", k, n - k);
    return 0;
}
