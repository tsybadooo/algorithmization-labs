/*
 * Лабораторна робота 2 "Умовні оператори". Варіант 19.
 *
 *       | a^2 + b^2,               якщо d = 2
 *   p = | ln a + ln b,             якщо d = 3
 *       | ln(a/b) - ln(b/a),       інакше
 *
 *   при d = 2,2;  a = 2,506;  b = 2,4003.
 *
 * Значення d вводиться з клавіатури (щоб перевірити всі гілки),
 * a та b задано константами за умовою.
 * Показано обидва способи розгалуження: if - else if - else
 * та тернарний оператор ?: .
 */
#include <stdio.h>
#include <math.h>
#include "../common_console.h"

int main(void)
{
    console_utf8();
    const double a = 2.506;     /* за умовою */
    const double b = 2.4003;    /* за умовою */
    const double EPS = 1e-9;    /* точність порівняння дійсних чисел */
    double d, p, p2;

    printf("Введіть d (за умовою d = 2.2): ");
    if (scanf("%lf", &d) != 1) {                 /* перевірка введення */
        printf("Помилка: потрібно ввести число\n");
        return 1;
    }

    /* 1-й спосіб: ланцюжок if - else if - else.
       Дійсні числа порівнюємо з точністю EPS, а не через ==. */
    if (fabs(d - 2.0) < EPS) {
        p = a * a + b * b;
        printf("Гілка 1 (d = 2): p = a^2 + b^2\n");
    } else if (fabs(d - 3.0) < EPS) {
        p = log(a) + log(b);
        printf("Гілка 2 (d = 3): p = ln a + ln b\n");
    } else {
        p = log(a / b) - log(b / a);
        printf("Гілка 3 (інакше): p = ln(a/b) - ln(b/a)\n");
    }

    /* 2-й спосіб: вкладений тернарний оператор (для перевірки) */
    p2 = (fabs(d - 2.0) < EPS) ? a * a + b * b
       : (fabs(d - 3.0) < EPS) ? log(a) + log(b)
       : log(a / b) - log(b / a);

    printf("a = %.4f, b = %.4f, d = %.4f\n", a, b, d);
    printf("p (if-else) = %.6f\n", p);
    printf("p (?:)      = %.6f\n", p2);
    { ; }   /* приклад порожнього оператора всередині блоку */
    return 0;
}
