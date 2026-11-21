/**
 * @file memtrack.c
 * @brief Реалізація детектора витоків пам'яті (див. memtrack.h).
 *
 * Таблиця блоків - статичний масив (це службовий модуль налагодження,
 * тому тут допускається внутрішній стан на рівні файла).
 */
#define MEMTRACK_IMPL
#include <stdio.h>
#include "memtrack.h"

#define MT_MAX 65536

typedef struct {
    void       *ptr;
    size_t      size;
    const char *file;
    int         line;
} MtBlock;

static MtBlock blocks[MT_MAX];
static size_t  n_blocks, n_alloc, n_free, n_bad_free, peak_bytes, cur_bytes, total_bytes;
static int     registered;

static void mt_report(void)
{
    fprintf(stderr, "\n==MEMTRACK== ЗВІТ ПРО ВИКОРИСТАННЯ ДИНАМІЧНОЇ ПАМ'ЯТІ\n");
    fprintf(stderr, "==MEMTRACK== виділень: %zu, звільнень: %zu, усього виділено: %zu байт, пік: %zu байт\n",
            n_alloc, n_free, total_bytes, peak_bytes);
    size_t leaked = 0;
    for (size_t i = 0; i < n_blocks; i++)
        if (blocks[i].ptr != NULL) {
            fprintf(stderr, "==MEMTRACK== ВИТІК: %zu байт, виділено у %s:%d\n",
                    blocks[i].size, blocks[i].file, blocks[i].line);
            leaked += blocks[i].size;
        }
    if (n_bad_free) fprintf(stderr, "==MEMTRACK== некоректних free(): %zu\n", n_bad_free);
    if (leaked == 0 && n_bad_free == 0)
        fprintf(stderr, "==MEMTRACK== Усі блоки звільнено. Витоків пам'яті не виявлено.\n");
    else
        fprintf(stderr, "==MEMTRACK== ПІДСУМОК: втрачено %zu байт\n", leaked);
}

static void mt_add(void *p, size_t size, const char *file, int line)
{
    if (!registered) { atexit(mt_report); registered = 1; }
    if (p == NULL) return;
    n_alloc++;
    total_bytes += size;
    cur_bytes += size;
    if (cur_bytes > peak_bytes) peak_bytes = cur_bytes;
    for (size_t i = 0; i < n_blocks; i++)          /* повторно використати вільну комірку */
        if (blocks[i].ptr == NULL) { blocks[i] = (MtBlock){p, size, file, line}; return; }
    if (n_blocks < MT_MAX) blocks[n_blocks++] = (MtBlock){p, size, file, line};
}

/** Вилучає блок з таблиці; повертає 1, якщо покажчик був відомий. */
static int mt_remove(void *p)
{
    for (size_t i = 0; i < n_blocks; i++)
        if (blocks[i].ptr == p) {
            cur_bytes -= blocks[i].size;
            blocks[i].ptr = NULL;
            return 1;
        }
    return 0;
}

void *mt_malloc(size_t size, const char *file, int line)
{
    void *p = malloc(size);
    mt_add(p, size, file, line);
    return p;
}

void *mt_calloc(size_t n, size_t size, const char *file, int line)
{
    void *p = calloc(n, size);
    mt_add(p, n * size, file, line);
    return p;
}

void *mt_realloc(void *old, size_t size, const char *file, int line)
{
    /* Індекс старого блоку шукаємо ДО realloc: після успішного realloc
       старий покажчик недійсний, і порівнювати його вже не можна. */
    size_t idx = MT_MAX;
    if (old != NULL)
        for (size_t i = 0; i < n_blocks; i++)
            if (blocks[i].ptr == old) { idx = i; break; }
    void *p = realloc(old, size);
    if (p != NULL) {
        if (idx != MT_MAX) {
            cur_bytes -= blocks[idx].size;
            blocks[idx].ptr = NULL;
            n_free++;
        }
        mt_add(p, size, file, line);
    }
    return p;
}

void mt_free(void *p, const char *file, int line)
{
    if (p == NULL) return;                          /* free(NULL) допустимий */
    if (!mt_remove(p)) {
        n_bad_free++;
        fprintf(stderr, "==MEMTRACK== ПОМИЛКА: free() невідомого або вже звільненого покажчика у %s:%d\n",
                file, line);
        return;                                     /* не передаємо в free(), щоб уникнути збою */
    }
    n_free++;
    free(p);
}
