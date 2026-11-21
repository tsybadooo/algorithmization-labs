/**
 * @file main.c
 * @brief КПІЗ, частина 2. Табулювання кусково заданої функції y(x)
 *        на відрізку [a0; b0] з кроком h трьома видами циклів.
 *
 * Вид циклу обирається через меню (switch). Для кожної точки
 * виводиться x, y та номер гілки; обчислюються сума, середнє,
 * максимум і мінімум значень, де функцію визначено.
 */
#include <stdio.h>
#include <float.h>
#include "func.h"
#include "../common/console.h"

/** Накопичувач статистики табулювання. */
typedef struct {
    double sum, min, max;
    long   count;      /* кількість точок, де y визначено */
    long   undefined;  /* кількість точок, де y не визначено */
} Stats;

/**
 * Читає дійсне число з повторним запитом у разі помилки.
 * @param prompt текст запиту
 * @param value [out] прочитане число
 * @return 1 - успішно, 0 - кінець вхідного потоку
 */
static int read_double(const char *prompt, double *value)
{
    for (;;) {
        printf("%s", prompt);
        int rc = scanf("%lf", value);
        if (rc == 1) { flush_line(); return 1; }
        if (rc == EOF) return 0;
        printf("  Помилка: введіть число.\n");
        flush_line();
    }
}

/** Обробляє одну точку: друк рядка таблиці та оновлення статистики. */
static void process_point(long i, double x, double a, Stats *st)
{
    FuncResult r = eval_point(x, a);
    if (r.ok) {
        printf("| %5ld | %12.5f | %14.6f |   %d   |\n", i + 1, x, r.y, r.branch);
        st->sum += r.y;
        if (st->count == 0 || r.y < st->min) st->min = r.y;
        if (st->count == 0 || r.y > st->max) st->max = r.y;
        st->count++;
    } else {
        printf("| %5ld | %12.5f | %14s |   %d   |\n", i + 1, x, "не визначено", r.branch);
        st->undefined++;
    }
}

/**
 * Табулювання з вибраним видом циклу.
 * x обчислюється як a0 + i*h (а не накопиченням x += h), щоб похибка
 * округлення не накопичувалась і не губилась остання точка b0.
 * @param kind 1 - for, 2 - while, 3 - do while
 */
static Stats tabulate(int kind, double a0, double b0, double h, double a)
{
    Stats st = {0.0, 0.0, 0.0, 0, 0};
    long n = points_count(a0, b0, h);
    long i;

    printf("+-------+--------------+----------------+-------+\n");
    printf("|   №   |      x       |      y(x)      | гілка |\n");
    printf("+-------+--------------+----------------+-------+\n");
    switch (kind) {
        case 1:                                   /* цикл з параметром */
            for (i = 0; i < n; i++)
                process_point(i, a0 + (double)i * h, a, &st);
            break;
        case 2:                                   /* цикл з передумовою */
            i = 0;
            while (i < n) {
                process_point(i, a0 + (double)i * h, a, &st);
                i++;
            }
            break;
        case 3:                                   /* цикл з післяумовою */
            i = 0;
            do {                                  /* n >= 1 гарантовано перевіркою */
                process_point(i, a0 + (double)i * h, a, &st);
                i++;
            } while (i < n);
            break;
        default:
            break;
    }
    printf("+-------+--------------+----------------+-------+\n");
    return st;
}

int main(void)
{
    console_utf8();
    printf("КПІЗ. Частина 2. Варіант 19\n");
    printf("y(x) = |sin x|, x < -pi;  x + a, -pi <= x <= pi;  1/sqrt(x - pi), x > pi\n\n");

    double a, a0, b0, h;
    if (!read_double("Параметр a (a > 0): ", &a)) return 1;
    while (!(a > 0.0)) {
        printf("  Помилка: a має бути додатним.\n");
        if (!read_double("Параметр a (a > 0): ", &a)) return 1;
    }
    if (!read_double("Початок відрізка a0: ", &a0)) return 1;
    if (!read_double("Кінець відрізка b0: ", &b0)) return 1;
    while (b0 < a0) {
        printf("  Помилка: b0 має бути не меншим за a0.\n");
        if (!read_double("Кінець відрізка b0: ", &b0)) return 1;
    }
    if (!read_double("Крок h (h > 0): ", &h)) return 1;
    while (!(h > 0.0) || points_count(a0, b0, h) == 0) {
        printf("  Помилка: h має бути додатним і не надто малим.\n");
        if (!read_double("Крок h (h > 0): ", &h)) return 1;
    }

    int choice = -1;
    while (choice != 0) {
        printf("\nМЕНЮ\n 1 - табулювання циклом for\n 2 - табулювання циклом while\n"
               " 3 - табулювання циклом do while\n 0 - вихід\nВаш вибір: ");
        if (scanf("%d", &choice) != 1) {
            if (feof(stdin)) break;
            flush_line();
            printf("  Помилка: введіть номер пункту.\n");
            continue;
        }
        flush_line();
        switch (choice) {
            case 1: case 2: case 3: {
                static const char *names[] = {"", "for", "while", "do while"};
                printf("\nТабулювання циклом %s, a = %g, [%g; %g], h = %g\n",
                       names[choice], a, a0, b0, h);
                Stats st = tabulate(choice, a0, b0, h, a);
                if (st.count > 0) {
                    printf("Кількість точок: %ld (не визначено: %ld)\n", st.count + st.undefined, st.undefined);
                    printf("Сума = %.6f; середнє = %.6f; максимум = %.6f; мінімум = %.6f\n",
                           st.sum, st.sum / (double)st.count, st.max, st.min);
                } else {
                    printf("Немає жодної точки, де функцію визначено.\n");
                }
                break;
            }
            case 0:
                printf("Завершення роботи.\n");
                break;
            default:
                printf("  Немає такого пункту меню.\n");
        }
    }
    return 0;
}
