/**
 * @file memtrack.h
 * @brief Простий детектор витоків пам'яті (аналог звіту Valgrind memcheck
 *        для Windows, де Valgrind недоступний).
 *
 * Підключається примусово ключем компілятора:
 *     gcc -DMEMTRACK -include common/memtrack.h ... common/memtrack.c
 * Макроси підміняють malloc/calloc/realloc/free версіями, що запам'ятовують
 * кожен блок (розмір, файл, рядок). Під час завершення програми (atexit)
 * друкується звіт: кількість виділень/звільнень, невивільнені блоки,
 * спроби звільнити невідомий покажчик (подвійне звільнення).
 */
#ifndef MEMTRACK_H
#define MEMTRACK_H

#include <stdlib.h>
#include <string.h>

void *mt_malloc(size_t size, const char *file, int line);
void *mt_calloc(size_t n, size_t size, const char *file, int line);
void *mt_realloc(void *p, size_t size, const char *file, int line);
void  mt_free(void *p, const char *file, int line);

#if defined(MEMTRACK) && !defined(MEMTRACK_IMPL)
#define malloc(s)     mt_malloc((s), __FILE__, __LINE__)
#define calloc(n, s)  mt_calloc((n), (s), __FILE__, __LINE__)
#define realloc(p, s) mt_realloc((p), (s), __FILE__, __LINE__)
#define free(p)       mt_free((p), __FILE__, __LINE__)
#endif

#endif
