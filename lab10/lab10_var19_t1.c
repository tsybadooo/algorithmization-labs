/*
 * Лабораторна робота №10. Індивідуальне завдання №19 (таблиця 3.1).
 * Ввести рядок. Замінити всі числа у рядку на їх квадрати.
 * Приклад: 'abc 3 def 5' -> 'abc 9 def 25'.
 *
 * Алгоритм:
 *  - рядок читається fgets(), символ '\n' прибирається;
 *  - прохід по рядку: якщо поточний символ - цифра (isdigit) і перед
 *    ним не літера/цифра, то це початок числа; враховується знак '-',
 *    якщо перед ним пропуск або початок рядка;
 *  - цифри числа накопичуються у long long, квадрат дописується у
 *    результуючий рядок через sprintf; інші символи копіюються як є;
 *  - числа, "приклеєні" до слів (напр. 'x2'), числами не вважаються.
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../common_console.h"

#define LEN 256

int main(void)
{
    console_utf8();
    char s[LEN], res[LEN * 4] = "";
    size_t rlen = 0;
    int count = 0;

    printf("Введіть рядок: ");
    if (fgets(s, sizeof s, stdin) == NULL) { printf("Помилка читання\n"); return 1; }
    s[strcspn(s, "\n")] = '\0';                     /* прибрати символ нового рядка */

    for (size_t i = 0; s[i] != '\0'; ) {
        unsigned char c = (unsigned char)s[i];
        unsigned char prev = i > 0 ? (unsigned char)s[i - 1] : ' ';
        int neg = (c == '-' && isdigit((unsigned char)s[i + 1]) && (i == 0 || isspace(prev)));
        if ((isdigit(c) && !isalnum(prev)) || neg) {
            size_t j = i + (neg ? 1 : 0);
            long long v = 0;
            while (isdigit((unsigned char)s[j]) && v < 100000000LL) v = v * 10 + (s[j++] - '0');
            if (isalpha((unsigned char)s[j]) || isdigit((unsigned char)s[j])) {
                /* число переходить у слово або занадто довге - копіюємо без змін */
                while (s[i] != '\0' && !isspace((unsigned char)s[i])) res[rlen++] = s[i++];
                continue;
            }
            if (neg) v = -v;
            rlen += (size_t)sprintf(res + rlen, "%lld", v * v);   /* дописати квадрат */
            count++;
            i = j;
        } else {
            res[rlen++] = s[i++];                    /* звичайний символ */
        }
        res[rlen] = '\0';
    }
    printf("Результат:     %s\n", res);
    printf("Замінено чисел: %d\n", count);
    return 0;
}
