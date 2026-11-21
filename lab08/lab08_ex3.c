/* Вправа 3. Ханойські вежі та рекурсивний відлік (з методичних вказівок) */
#include "../common_console.h"
#include <stdio.h>
int total_moves;
void hanoi(int disks, char from, char to, char via) {
    if (disks == 1) {
        printf("  Диск 1: %c -> %c\n", from, to);
        total_moves++;
        return;
    }
    hanoi(disks-1, from, via, to);   /* перемістити n-1 на допоміжний */
    printf("  Диск %d: %c -> %c\n", disks, from, to);
    total_moves++;
    hanoi(disks-1, via, to, from);   /* перемістити n-1 з допоміжного */
}
int main(void) {
    console_utf8();
    printf("=== Ханойські вежі ===\n");
    for (int n = 1; n <= 4; n++) {
        total_moves = 0;
        printf("\n%d дисків: (A->C через B)\n", n);
        hanoi(n, 'A', 'C', 'B');
        printf("  Ходів: %d  (2^%d-1=%d)\n",
               total_moves, n, (1<<n)-1);
    }
    /* Рекурсивний зворотній відлік */
    printf("\n=== Рекурсивний відлік від 5 до 0 ===\n");
    /* Функція countdown через лямбду-ідею у С — вкладений виклик */
    void countdown(int n) {
        if (n < 0) return;
        printf("%d ", n);
        countdown(n - 1);
    }
    countdown(5);
    printf("\n");
    return 0;
}
