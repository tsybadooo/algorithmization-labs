/* Вправа 4. Решето Ератосфена (з методичних вказівок) */
#include "../common_console.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#define MAXN 1000
int main(void) {
    console_utf8();
    int n;
    printf("Знайти прості числа до N (≤ %d): ", MAXN);
    scanf("%d", &n);
    char is_prime[MAXN + 1];
    memset(is_prime, 1, sizeof(is_prime));
    is_prime[0] = is_prime[1] = 0;
    /* Решето */
    for (int i = 2; (long long)i*i <= n; i++)
        if (is_prime[i])
            for (int j = i*i; j <= n; j += i)
                is_prime[j] = 0;
    /* Виведення */
    int count = 0;
    long long sum = 0;
    printf("Прості числа до %d:\n", n);
    for (int i = 2; i <= n; i++) {
        if (is_prime[i]) {
            printf("%5d", i);
            count++;
            sum += i;
            if (count % 12 == 0) printf("\n");
        }
    }
    printf("\n");
    printf("Кількість: %d\n", count);
    printf("Сума:      %lld\n", sum);
    printf("Найбільше: ");
    for (int i = n; i >= 2; i--)
        if (is_prime[i]) { printf("%d\n", i); break; }
    return 0;
}
