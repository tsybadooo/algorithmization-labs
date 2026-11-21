/**
 * @file func.h
 * @brief КПІЗ, частина 2 (варіант 19): кусково задана функція y(x).
 *
 *          | |sin x|,          x < -pi
 *   y(x) = | x + a,            -pi <= x <= pi
 *          | 1 / sqrt(x - pi), x > pi
 */
#ifndef FUNC_H
#define FUNC_H

#define KPIZ_PI 3.14159265358979323846

/** Результат обчислення y(x). */
typedef struct {
    double y;      /**< значення функції (має сенс, якщо ok == 1)     */
    int    ok;     /**< 1 - функцію визначено, 0 - не визначено        */
    int    branch; /**< номер гілки 1..3                               */
} FuncResult;

/**
 * Обчислює y(x) для варіанта 19.
 * @param x  аргумент
 * @param a  параметр функції (a > 0)
 * @param ok [out] 1 - значення визначене, 0 - ні (може бути NULL)
 * @return значення y(x) або 0.0, якщо функція не визначена
 */
double calc_y(double x, double a, int *ok);

/**
 * Номер гілки, до якої належить x (1, 2 або 3).
 * @param x аргумент
 * @return 1 для x < -pi, 2 для -pi <= x <= pi, 3 для x > pi
 */
int branch_of(double x);

/** Повна інформація про точку: значення, ознака визначеності, гілка. */
FuncResult eval_point(double x, double a);

/**
 * Кількість точок табулювання на [a0; b0] з кроком h.
 * Враховує похибку подання дійсних чисел (допуск 1e-9 * h).
 * @return кількість точок (>= 1) або 0 при некоректних параметрах
 */
long points_count(double a0, double b0, double h);

#endif
