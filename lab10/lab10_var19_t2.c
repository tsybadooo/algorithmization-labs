/*
 * Лабораторна робота №10. Індивідуальне завдання №19 (таблиця 3.2).
 * Ввести рядок з числом у римській системі. Перетворити у арабське
 * число. Підтримати I, V, X, L, C, D, M.
 *
 * Алгоритм:
 *  - рядок читається fgets(), пропуски на краях відкидаються,
 *    літери переводяться у верхній регістр (toupper);
 *  - прохід зліва направо: якщо значення поточної цифри менше за
 *    наступну - воно віднімається (IV = 5 - 1), інакше додається;
 *  - для перевірки коректності отримане число переводиться назад
 *    у римський запис (функція to_roman) і порівнюється з введеним:
 *    так відкидаються записи на зразок IIII, VX, IC, MMMM.
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../common_console.h"

int roman_value(char c);
int roman_to_int(const char *s);
void to_roman(int n, char *out);

int main(void)
{
    console_utf8();
    char line[256], s[256], back[32];
    printf("Введіть римське число: ");
    if (fgets(line, sizeof line, stdin) == NULL) { printf("Помилка читання\n"); return 1; }

    /* вилучити пропуски та перевести у верхній регістр */
    size_t k = 0;
    for (size_t i = 0; line[i] != '\0'; i++)
        if (!isspace((unsigned char)line[i])) s[k++] = (char)toupper((unsigned char)line[i]);
    s[k] = '\0';

    if (k == 0) { printf("Помилка: порожній рядок\n"); return 1; }
    int v = roman_to_int(s);
    if (v <= 0) { printf("Помилка: '%s' містить недопустимі символи\n", s); return 1; }
    to_roman(v, back);
    if (strcmp(back, s) != 0) {
        printf("Помилка: '%s' не є правильним римським записом (правильно для %d: %s)\n", s, v, back);
        return 1;
    }
    printf("%s = %d\n", s, v);
    return 0;
}

/* Значення однієї римської цифри або 0, якщо символ недопустимий. */
int roman_value(char c)
{
    switch (c) {
        case 'I': return 1;    case 'V': return 5;
        case 'X': return 10;   case 'L': return 50;
        case 'C': return 100;  case 'D': return 500;
        case 'M': return 1000; default:  return 0;
    }
}

/* Перетворення римського запису в ціле; повертає -1 для недопустимих символів. */
int roman_to_int(const char *s)
{
    int total = 0;
    size_t n = strlen(s);
    for (size_t i = 0; i < n; i++) {
        int cur = roman_value(s[i]);
        if (cur == 0) return -1;
        int next = (i + 1 < n) ? roman_value(s[i + 1]) : 0;
        if (cur < next) total -= cur; else total += cur;
    }
    return total;
}

/* Канонічний римський запис числа 1..3999 (жадібний алгоритм). */
void to_roman(int n, char *out)
{
    static const int val[13] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
    static const char *sym[13] = {"M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I"};
    out[0] = '\0';
    if (n <= 0 || n > 3999) return;
    for (int i = 0; i < 13; i++)
        while (n >= val[i]) { strcat(out, sym[i]); n -= val[i]; }
}
