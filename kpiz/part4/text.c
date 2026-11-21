/**
 * @file text.c
 * @brief Реалізація опрацювання тексту та пошуку анаграм.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "text.h"

/* ------------------------------------------------------------------ */
/*                       Динамічний список рядків                      */
/* ------------------------------------------------------------------ */

void strlist_init(StrList *l)
{
    l->items = NULL;
    l->count = 0;
    l->cap = 0;
}

int strlist_add(StrList *l, const char *s)
{
    if (l->count == l->cap) {
        size_t ncap = l->cap ? l->cap * 2 : 8;
        char **p = (char **)realloc(l->items, ncap * sizeof(char *));
        if (p == NULL) return 0;          /* старий блок лишається дійсним */
        l->items = p;
        l->cap = ncap;
    }
    size_t len = strlen(s);
    char *copy = (char *)malloc(len + 1);
    if (copy == NULL) return 0;
    memcpy(copy, s, len + 1);
    l->items[l->count++] = copy;
    return 1;
}

void strlist_free(StrList *l)
{
    for (size_t i = 0; i < l->count; i++) free(l->items[i]);
    free(l->items);
    strlist_init(l);
}

/* ------------------------------------------------------------------ */
/*                    Власні функції роботи з UTF-8                    */
/* ------------------------------------------------------------------ */

int utf8_decode(const char *s, uint32_t *cp)
{
    const unsigned char *u = (const unsigned char *)s;
    if (u[0] < 0x80) { *cp = u[0]; return 1; }
    if ((u[0] & 0xE0) == 0xC0 && (u[1] & 0xC0) == 0x80) {
        *cp = ((uint32_t)(u[0] & 0x1F) << 6) | (u[1] & 0x3F);
        return 2;
    }
    if ((u[0] & 0xF0) == 0xE0 && (u[1] & 0xC0) == 0x80 && (u[2] & 0xC0) == 0x80) {
        *cp = ((uint32_t)(u[0] & 0x0F) << 12) | ((uint32_t)(u[1] & 0x3F) << 6) | (u[2] & 0x3F);
        return 3;
    }
    if ((u[0] & 0xF8) == 0xF0 && (u[1] & 0xC0) == 0x80 && (u[2] & 0xC0) == 0x80 && (u[3] & 0xC0) == 0x80) {
        *cp = ((uint32_t)(u[0] & 0x07) << 18) | ((uint32_t)(u[1] & 0x3F) << 12) |
              ((uint32_t)(u[2] & 0x3F) << 6) | (u[3] & 0x3F);
        return 4;
    }
    *cp = 0xFFFD;                         /* некоректна послідовність */
    return 1;
}

int utf8_encode(uint32_t cp, char *out)
{
    if (cp < 0x80) { out[0] = (char)cp; return 1; }
    if (cp < 0x800) {
        out[0] = (char)(0xC0 | (cp >> 6));
        out[1] = (char)(0x80 | (cp & 0x3F));
        return 2;
    }
    if (cp < 0x10000) {
        out[0] = (char)(0xE0 | (cp >> 12));
        out[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
        out[2] = (char)(0x80 | (cp & 0x3F));
        return 3;
    }
    out[0] = (char)(0xF0 | (cp >> 18));
    out[1] = (char)(0x80 | ((cp >> 12) & 0x3F));
    out[2] = (char)(0x80 | ((cp >> 6) & 0x3F));
    out[3] = (char)(0x80 | (cp & 0x3F));
    return 4;
}

size_t utf8_strlen(const char *s)
{
    size_t n = 0;
    while (*s != '\0') {
        uint32_t cp;
        s += utf8_decode(s, &cp);
        n++;
    }
    return n;
}

int cp_is_letter(uint32_t cp)
{
    if ((cp >= 'A' && cp <= 'Z') || (cp >= 'a' && cp <= 'z')) return 1;
    if (cp >= 0x0400 && cp <= 0x045F) return 1;          /* основна кирилиця */
    if (cp == 0x0490 || cp == 0x0491) return 1;          /* Ґ ґ */
    return 0;
}

uint32_t cp_to_lower(uint32_t cp)
{
    if (cp >= 'A' && cp <= 'Z') return cp + 32;
    if (cp >= 0x0410 && cp <= 0x042F) return cp + 0x20;  /* А..Я -> а..я */
    if (cp >= 0x0400 && cp <= 0x040F) return cp + 0x50;  /* Ѐ..Џ (Є, І, Ї) */
    if (cp == 0x0490) return 0x0491;                     /* Ґ -> ґ */
    return cp;
}

void word_to_lower(const char *word, char *out)
{
    while (*word != '\0') {
        uint32_t cp;
        word += utf8_decode(word, &cp);
        out += utf8_encode(cp_to_lower(cp), out);
    }
    *out = '\0';
}

size_t make_anagram_key(const char *word, char *out)
{
    uint32_t cps[MAX_LINE + 1];
    size_t n = 0;
    while (*word != '\0' && n < MAX_LINE) {
        uint32_t cp;
        word += utf8_decode(word, &cp);
        if (cp_is_letter(cp)) cps[n++] = cp_to_lower(cp);
    }
    /* сортування вставками кодових точок (слова короткі - O(k^2) достатньо) */
    for (size_t i = 1; i < n; i++) {
        uint32_t key = cps[i];
        size_t j = i;
        while (j > 0 && cps[j - 1] > key) { cps[j] = cps[j - 1]; j--; }
        cps[j] = key;
    }
    for (size_t i = 0; i < n; i++) out += utf8_encode(cps[i], out);
    *out = '\0';
    return n;
}

/** Розмір буферів для одного слова: MAX_LINE символів по 4 байти + '\0'. */
#define WORD_BUF (MAX_LINE * 4 + 1)

int is_anagram(const char *w1, const char *w2)
{
    /* Рефакторинг після TDD-циклу 3: спершу перевіряємо довжину, щоб
       word_to_lower/make_anagram_key ніколи не виходили за межі буферів;
       слова довші за MAX_LINE символів не можуть бути словами тексту. */
    if (strlen(w1) >= WORD_BUF || strlen(w2) >= WORD_BUF) return 0;
    char key[2][WORD_BUF], low[2][WORD_BUF];
    const char *w[2] = {w1, w2};
    for (int i = 0; i < 2; i++) {
        if (make_anagram_key(w[i], key[i]) == 0) return 0;   /* слово без літер */
        word_to_lower(w[i], low[i]);
    }
    return strcmp(key[0], key[1]) == 0 && strcmp(low[0], low[1]) != 0;
}

/* ------------------------------------------------------------------ */
/*                       Розбиття тексту на слова                      */
/* ------------------------------------------------------------------ */

/** Чи починається з позиції s літера або цифра (для обрізання країв слова). */
static int starts_with_alnum(const char *s)
{
    uint32_t cp;
    utf8_decode(s, &cp);
    return cp_is_letter(cp) || (cp < 0x80 && isdigit((int)cp));
}

size_t split_words(const char *line, StrList *words)
{
    static const char DELIMS[] = " \t\r\n.,;:!?()[]{}\"/\\<>=+*&^%$#@~|";
    char buf[MAX_LINE * 4 + 2];
    size_t added = 0;

    strncpy(buf, line, sizeof buf - 1);
    buf[sizeof buf - 1] = '\0';

    for (char *tok = strtok(buf, DELIMS); tok != NULL; tok = strtok(NULL, DELIMS)) {
        /* відкинути неалфавітні символи на початку (напр. «, —, ') */
        while (*tok != '\0' && !starts_with_alnum(tok)) {
            uint32_t cp;
            tok += utf8_decode(tok, &cp);
        }
        /* відкинути неалфавітні символи в кінці: шукаємо кінець останньої літери/цифри */
        char *end = tok, *p = tok;
        while (*p != '\0') {
            uint32_t cp;
            int len = utf8_decode(p, &cp);
            if (cp_is_letter(cp) || (cp < 0x80 && isdigit((int)cp))) end = p + len;
            p += len;
        }
        *end = '\0';
        if (*tok == '\0') continue;                 /* токен без літер (напр. "—") */
        if (!strlist_add(words, tok)) return (size_t)-1;
        added++;
    }
    return added;
}

TextStats text_stats(const StrList *lines, size_t word_count)
{
    TextStats st = {0, word_count, 0};
    int in_sentence = 0;   /* чи є незавершене речення */
    for (size_t i = 0; i < lines->count; i++) {
        const char *s = lines->items[i];
        st.chars += utf8_strlen(s);
        for (const char *p = s; *p != '\0'; p++) {
            if (*p == '.' || *p == '!' || *p == '?') {
                if (in_sentence) st.sentences++;    /* "...", "?!" - одне речення */
                in_sentence = 0;
            } else if (!isspace((unsigned char)*p)) {
                in_sentence = 1;
            }
        }
    }
    if (in_sentence) st.sentences++;                /* останнє речення без крапки */
    return st;
}

/* ------------------------------------------------------------------ */
/*                           Пошук анаграм                             */
/* ------------------------------------------------------------------ */

/** Пара "слово в нижньому регістрі - ключ анаграми". */
typedef struct {
    char *word;
    char *key;
} KeyedWord;

/** Порівняння для qsort: спочатку за ключем, потім за словом. */
static int cmp_keyed(const void *a, const void *b)
{
    const KeyedWord *x = (const KeyedWord *)a, *y = (const KeyedWord *)b;
    int c = strcmp(x->key, y->key);
    return c != 0 ? c : strcmp(x->word, y->word);
}

static void free_keyed(KeyedWord *kw, size_t n)
{
    for (size_t i = 0; i < n; i++) { free(kw[i].word); free(kw[i].key); }
    free(kw);
}

int find_anagram_groups(const StrList *words, StrList *groups)
{
    size_t n = words->count;
    if (n == 0) return 1;
    KeyedWord *kw = (KeyedWord *)calloc(n, sizeof(KeyedWord));
    if (kw == NULL) return 0;

    size_t m = 0;
    for (size_t i = 0; i < n; i++) {
        size_t len = strlen(words->items[i]);
        kw[m].word = (char *)malloc(len * 2 + 1);   /* запас: нижній регістр може бути довшим */
        kw[m].key  = (char *)malloc(len * 2 + 1);
        if (kw[m].word == NULL || kw[m].key == NULL) { free_keyed(kw, m + 1); return 0; }
        word_to_lower(words->items[i], kw[m].word);
        if (make_anagram_key(words->items[i], kw[m].key) < 2) {   /* однолітерні слова пропускаємо */
            free(kw[m].word); free(kw[m].key);
            kw[m].word = kw[m].key = NULL;
            continue;
        }
        m++;
    }
    qsort(kw, m, sizeof(KeyedWord), cmp_keyed);

    /* Проходимо відсортований масив: однакові ключі стоять поруч */
    char line[MAX_LINE * 8];
    size_t i = 0;
    while (i < m) {
        size_t j = i + 1, distinct = 1;
        size_t pos = (size_t)snprintf(line, sizeof line, "%s", kw[i].word);
        while (j < m && strcmp(kw[j].key, kw[i].key) == 0) {
            if (strcmp(kw[j].word, kw[j - 1].word) != 0) {          /* пропуск повторів слова */
                if (pos < sizeof line)
                    pos += (size_t)snprintf(line + pos, sizeof line - pos, " %s", kw[j].word);
                distinct++;
            }
            j++;
        }
        if (distinct >= 2 && !strlist_add(groups, line)) { free_keyed(kw, m); return 0; }
        i = j;
    }
    free_keyed(kw, m);
    return 1;
}

int print_anagram_groups(const StrList *words, size_t *groups_out)
{
    StrList groups;
    strlist_init(&groups);
    if (!find_anagram_groups(words, &groups)) { strlist_free(&groups); return 0; }
    if (groups.count == 0) printf("Груп слів-анаграм не знайдено.\n");
    for (size_t g = 0; g < groups.count; g++)
        printf("  Група %zu: %s\n", g + 1, groups.items[g]);
    if (groups_out != NULL) *groups_out = groups.count;
    strlist_free(&groups);
    return 1;
}
