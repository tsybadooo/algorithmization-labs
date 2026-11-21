/**
 * @file test_tdd.c
 * @brief КПІЗ, частина 6. Тести функції is_anagram, яку розроблено за
 *        методикою TDD. Макрос CYCLE задає, скільки циклів
 *        «червоний - зелений - рефакторинг» уже пройдено (1..3).
 */
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../part4/text.h"

#ifndef CYCLE
#define CYCLE 3
#endif

int main(void)
{
    /* Цикл 1: слова з однаковим набором літер (з різним регістром) - анаграми */
    assert(is_anagram("Кабан", "банка") == 1);
    assert(is_anagram("рамку", "мурку") == 0);
    printf("cycle 1 OK\n");
#if CYCLE >= 2
    /* Цикл 2: те саме слово (без урахування регістру) анаграмою не вважається */
    assert(is_anagram("кабан", "КАБАН") == 0);
    printf("cycle 2 OK\n");
#endif
#if CYCLE >= 3
    /* Цикл 3: порожні та надто довгі рядки не спричиняють переповнення буфера */
    char longw[1200];
    memset(longw, 'a', sizeof longw - 1);
    longw[sizeof longw - 1] = '\0';
    assert(is_anagram("", "") == 0);
    assert(is_anagram(longw, longw) == 0);
    printf("cycle 3 OK\n");
#endif
    return 0;
}
