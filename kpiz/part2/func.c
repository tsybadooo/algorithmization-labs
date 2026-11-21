/**
 * @file func.c
 * @brief Реалізація функції y(x) варіанта 19.
 */
#include <stddef.h>
#include <math.h>
#include "func.h"

int branch_of(double x)
{
    if (x < -KPIZ_PI) return 1;
    return (x <= KPIZ_PI) ? 2 : 3;   /* друга/третя гілка - тернарний оператор */
}

double calc_y(double x, double a, int *ok)
{
    int defined = 1;
    double y = 0.0;

    switch (branch_of(x)) {
        case 1:
            y = fabs(sin(x));
            break;
        case 2:
            y = x + a;
            break;
        default: {
            double d = x - KPIZ_PI;           /* для x > pi завжди d > 0 */
            /* Захист від переповнення: якщо d настільки мале, що sqrt(d)
               дорівнює 0 у double, значення вважаємо невизначеним. */
            if (d <= 0.0 || sqrt(d) == 0.0) defined = 0;
            else y = 1.0 / sqrt(d);
            break;
        }
    }
    if (ok != NULL) *ok = defined;
    return defined ? y : 0.0;
}

FuncResult eval_point(double x, double a)
{
    FuncResult r;
    r.branch = branch_of(x);
    r.y = calc_y(x, a, &r.ok);
    return r;
}

long points_count(double a0, double b0, double h)
{
    if (!(h > 0.0) || b0 < a0) return 0;
    double n = floor((b0 - a0) / h + 1e-9);   /* допуск на похибку округлення */
    if (n > 1e7) return 0;                    /* захист від надто малого кроку */
    return (long)n + 1;
}
