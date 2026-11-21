/* Вправа 2. Рекурсія проти ітерації — числа Фібоначчі (з методичних вказівок) */
#include "../common_console.h"
#include <stdio.h>
long long call_count;  /* лічильник викликів */
/* Рекурсивний — O(2^n) */
long long fib_rec(int n) {
    call_count++;
    if (n <= 1) return n;
    return fib_rec(n-1) + fib_rec(n-2);
}
/* Ітеративний — O(n) */
long long fib_iter(int n) {
    if (n <= 1) return n;
    long long a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        long long c = a + b; a = b; b = c;
    }
    return b;
}
/* Мемоізований — O(n) через кеш */
long long memo[50];
long long fib_memo(int n) {
    call_count++;
    if (n <= 1) return n;
    if (memo[n] != -1) return memo[n];
    memo[n] = fib_memo(n-1) + fib_memo(n-2);
    return memo[n];
}
int main(void) {
    console_utf8();
    printf("%-4s %-12s %-12s %-14s %-12s\n",
           "n", "fib(n)", "рекурс.вик.", "мемо.вик.", "ітер.");
    printf("%s\n", "-------------------------------------------------------");
    for (int n = 0; n <= 20; n++) {
        call_count = 0;
        long long r = fib_rec(n);
        long long rc = call_count;
        for (int i = 0; i < 50; i++) memo[i] = -1;
        call_count = 0;
        long long m = fib_memo(n);
        long long mc = call_count;
        long long it = fib_iter(n);
        if (m != r || it != r) printf("Розбіжність результатів!\n");
        printf("%-4d %-12lld %-14lld %-14lld %-12lld\n",
               n, r, rc, mc, it);
    }
    return 0;
}
