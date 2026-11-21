/*
 * Лабораторна робота №8. Індивідуальне завдання №19 (таблиця 3.1).
 * 1) rotate_digits(n, k) - циклічно зсунути цифри числа на k позицій
 *    (k > 0 - вліво, k < 0 - вправо). Приклад: 12345, k=2 -> 34512.
 * 2) sum_of_proper_divisors(n) - сума власних дільників числа n
 *    (усіх дільників, менших за n). Приклад: 28 -> 1+2+4+7+14 = 28.
 * Для зсуву написано ітеративну і рекурсивну версії та порівняно їх.
 */
#include <stdio.h>
#include "../common_console.h"

/* ---------- прототипи ---------- */
int  count_digits(long n);
long ipow10(int p);
long rotate_digits(long n, int k);
long rotate_left_once(long n, int len);
long rotate_digits_rec(long n, int k, int len);
long sum_of_proper_divisors(long n);
void print_divisors(long n);

int main(void)
{
    console_utf8();
    long n;
    int k;
    printf("Введіть натуральне число n та зсув k: ");
    if (scanf("%ld %d", &n, &k) != 2 || n <= 0) {
        printf("Помилка: n має бути натуральним, k - цілим\n");
        return 1;
    }
    int len = count_digits(n);
    int kk = ((k % len) + len) % len;           /* нормалізований зсув вліво */

    long r1 = rotate_digits(n, k);
    long r2 = rotate_digits_rec(n, kk, len);
    printf("Кількість цифр: %d\n", len);
    printf("rotate_digits(%ld, %d)           = %ld\n", n, k, r1);
    printf("рекурсивна версія (вліво на %d)   = %ld  -> %s\n", kk, r2,
           r1 == r2 ? "збігається" : "РОЗБІЖНІСТЬ");

    long s = sum_of_proper_divisors(n);
    printf("Власні дільники %ld: ", n);
    print_divisors(n);
    printf("sum_of_proper_divisors(%ld) = %ld", n, s);
    if (s == n)      printf("  (досконале число)\n");
    else if (s > n)  printf("  (надлишкове число)\n");
    else             printf("  (недостатнє число)\n");
    return 0;
}

/* Призначення: кількість десяткових цифр числа.
   Параметри: n - невід'ємне ціле. Повертає: кількість цифр (для 0 - 1). */
int count_digits(long n)
{
    int c = 1;
    while (n >= 10) { n /= 10; c++; }
    return c;
}

/* Повертає 10 у степені p (p >= 0) без використання pow(). */
long ipow10(int p)
{
    long r = 1;
    while (p-- > 0) r *= 10;
    return r;
}

/* Призначення: циклічний зсув цифр (ітеративно, за формулою).
   Параметри: n - натуральне число, k - зсув (k>0 вліво, k<0 вправо).
   Повертає: число після зсуву. Ведучі нулі, що з'являються
   (напр. 1002 -> 0021), за правилами запису числа відкидаються. */
long rotate_digits(long n, int k)
{
    int len = count_digits(n);
    k = ((k % len) + len) % len;          /* зсув вправо = зсув вліво на len-k */
    long p = ipow10(len - k);
    long head = n / p;                    /* перші k цифр      */
    long tail = n % p;                    /* останні len-k цифр */
    return tail * ipow10(k) + head;
}

/* Зсув на 1 позицію вліво з урахуванням початкової довжини len. */
long rotate_left_once(long n, int len)
{
    long p = ipow10(len - 1);
    return (n % p) * 10 + n / p;
}

/* Рекурсивна версія: зсув на k = зсув на 1 + зсув на (k-1).
   База рекурсії: k == 0. Глибина рекурсії - k (k < len). */
long rotate_digits_rec(long n, int k, int len)
{
    if (k == 0) return n;
    return rotate_digits_rec(rotate_left_once(n, len), k - 1, len);
}

/* Призначення: сума власних дільників числа (дільники < n).
   Перебір до sqrt(n): дільники йдуть парами i та n/i. */
long sum_of_proper_divisors(long n)
{
    if (n == 1) return 0;
    long s = 1;
    for (long i = 2; i * i <= n; i++)
        if (n % i == 0) {
            s += i;
            if (i != n / i) s += n / i;
        }
    return s;
}

/* Виводить усі власні дільники числа у порядку зростання. */
void print_divisors(long n)
{
    int any = 0;
    for (long i = 1; i < n; i++)
        if (n % i == 0) { printf("%s%ld", any ? " " : "", i); any = 1; }
    if (!any) printf("(немає)");
    printf("\n");
}
