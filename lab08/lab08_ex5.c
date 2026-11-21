/* Вправа 5. Таблиця температур та функція форматування (з методичних вказівок) */
#include "../common_console.h"
#include <stdio.h>
double c_to_f(double c) { return c * 9.0/5.0 + 32.0; }
double c_to_k(double c) { return c + 273.15; }
double f_to_c(double f) { return (f - 32.0) * 5.0/9.0; }
const char* temp_description(double c) {
    if (c < -20)  return "екстремальний мороз";
    if (c < 0)    return "морозно";
    if (c < 10)   return "холодно";
    if (c < 20)   return "прохолодно";
    if (c < 25)   return "комфортно";
    if (c < 35)   return "тепло";
    return "спека";
}
int main(void) {
    console_utf8();
    printf("%-10s %-12s %-12s  %s\n",
           "Цельсій", "Фаренгейт", "Кельвін", "Опис");
    printf("%s\n", "----------------------------------------------------");
    for (int c = -40; c <= 100; c += 10) {
        printf("%-10d %-12.1f %-12.2f  %s\n",
               c, c_to_f(c), c_to_k(c), temp_description(c));
    }
    return 0;
}
