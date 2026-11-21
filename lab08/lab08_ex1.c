/* Вправа 1. Математичні функції — прості числа, НСД, факторіал (з методичних вказівок) */
#include "../common_console.h"
#include <stdio.h>
#include <math.h>
/* Прототипи */
int  is_prime(int n);
int  gcd(int a, int b);
int  lcm(int a, int b);
long long factorial(int n);
int  sum_digits(int n);
int  count_digits(int n);
int is_prime(int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (int i = 3; (long long)i*i <= n; i += 2)
        if (n % i == 0) return 0;
    return 1;
}
int gcd(int a, int b) {
    return b == 0 ? a : gcd(b, a % b);
}
int lcm(int a, int b) {
    return a / gcd(a, b) * b;  /* ділення ПЕРЕД множенням — уникаємо переповнення */
}
long long factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}
int sum_digits(int n) {
    int s = 0;
    if (n < 0) n = -n;
    if (n == 0) return 0;
    while (n > 0) { s += n % 10; n /= 10; }
    return s;
}
int count_digits(int n) {
    if (n == 0) return 1;
    int c = 0;
    if (n < 0) n = -n;
    while (n > 0) { c++; n /= 10; }
    return c;
}
int main(void) {
    console_utf8();
    /* Прості числа */
    printf("Прості від 2 до 50: ");
    for (int i = 2; i <= 50; i++)
        if (is_prime(i)) printf("%d ", i);
    printf("\n");
    /* НСД і НСК */
    int a = 48, b = 36;
    printf("НСД(%d,%d)=%d  НСК=%d\n", a, b, gcd(a,b), lcm(a,b));
    /* Факторіали */
    printf("\nТаблиця факторіалів:\n");
    for (int n = 0; n <= 12; n++)
        printf("  %2d! = %lld\n", n, factorial(n));
    /* Цифри числа */
    int num = 123456;
    printf("\nЧисло: %d\n", num);
    printf("Кількість цифр: %d\n", count_digits(num));
    printf("Сума цифр:      %d\n", sum_digits(num));
    return 0;
}
