/* lab01a.cpp - Лабораторна робота №1
   Варіант 19 (5): обчислити площу прямокутного трикутника
   за довжинами його катета a і гіпотенузи c.
   Алгоритм (лінійний):
     1) b = sqrt(c*c - a*a)   - другий катет (теорема Піфагора);
     2) S = a * b / 2         - площа прямокутного трикутника;
     3) вивести a, c, b, S.                                        */
#include <stdio.h>
#include <math.h>
#include "../common_console.h"
#include "lab01a.h"

int main()
{
    console_utf8();
    const double a = LEG_A;          /* катет      */
    const double c = HYPOTENUSE_C;   /* гіпотенуза */
    double b, s;

    b = sqrt(c * c - a * a);         /* другий катет */
    s = a * b / 2.0;                 /* площа        */

    printf("Лабораторна робота №1. Варіант 19 (5)\n");
    printf("Катет a      = %.3f\n", a);
    printf("Гіпотенуза c = %.3f\n", c);
    printf("Катет b      = %.3f\n", b);
    printf("Площа S      = %.3f\n", s);
    return 0;
}
