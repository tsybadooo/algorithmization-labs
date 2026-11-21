/*
 * Лабораторна робота №8. Індивідуальне завдання №19 (таблиця 3.2).
 * 1) is_leap_year(year) - чи є рік високосним (григоріанський календар);
 * 2) days_in_month(month, year) - кількість днів у місяці;
 * 3) day_of_year(day, month, year) - порядковий номер дня в році.
 * day_of_year реалізовано ітеративно і рекурсивно (порівняння).
 */
#include <stdio.h>
#include "../common_console.h"

/* ---------- прототипи ---------- */
int is_leap_year(int year);
int days_in_month(int month, int year);
int is_valid_date(int day, int month, int year);
int day_of_year(int day, int month, int year);
int day_of_year_rec(int day, int month, int year);

int main(void)
{
    console_utf8();
    int d, m, y;
    printf("Введіть дату (день місяць рік): ");
    if (scanf("%d %d %d", &d, &m, &y) != 3) {
        printf("Помилка: потрібно три цілих числа\n");
        return 1;
    }
    if (!is_valid_date(d, m, y)) {
        printf("Помилка: дата %02d.%02d.%d не існує\n", d, m, y);
        return 1;
    }
    printf("Рік %d %s високосним (%d днів)\n", y,
           is_leap_year(y) ? "є" : "не є", is_leap_year(y) ? 366 : 365);
    printf("У місяці %d року %d - %d днів\n", m, y, days_in_month(m, y));
    int it = day_of_year(d, m, y), rc = day_of_year_rec(d, m, y);
    printf("Дата %02d.%02d.%d - це %d-й день року (рекурсивно: %d) -> %s\n",
           d, m, y, it, rc, it == rc ? "збігається" : "РОЗБІЖНІСТЬ");
    return 0;
}

/* Повертає 1, якщо рік високосний: ділиться на 4, але не на 100,
   або ділиться на 400. Інакше 0. */
int is_leap_year(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/* Повертає кількість днів у місяці month (1..12) року year,
   або 0, якщо номер місяця некоректний. */
int days_in_month(int month, int year)
{
    static const int dm[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month < 1 || month > 12) return 0;
    if (month == 2 && is_leap_year(year)) return 29;
    return dm[month - 1];
}

/* Перевірка існування дати (рік >= 1). Повертає 1 - коректна, 0 - ні. */
int is_valid_date(int day, int month, int year)
{
    return year >= 1 && month >= 1 && month <= 12 &&
           day >= 1 && day <= days_in_month(month, year);
}

/* Ітеративно: сума днів повних попередніх місяців + день. */
int day_of_year(int day, int month, int year)
{
    int n = day;
    for (int m = 1; m < month; m++) n += days_in_month(m, year);
    return n;
}

/* Рекурсивно: day_of_year(d, m) = days_in_month(m-1) + day_of_year(d, m-1);
   база: m == 1 -> d. */
int day_of_year_rec(int day, int month, int year)
{
    if (month == 1) return day;
    return days_in_month(month - 1, year) + day_of_year_rec(day, month - 1, year);
}
