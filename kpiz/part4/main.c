/**
 * @file main.c
 * @brief КПІЗ, частина 4 (варіант 19). Введення тексту, розбиття на слова,
 *        пошук груп слів-анаграм, статистика тексту. Консольне меню.
 */
#include <stdio.h>
#include <string.h>
#include "text.h"
#include "../common/console.h"

/**
 * Читає рядки з потоку fgets до порожнього рядка або EOF.
 * Рядки довші за MAX_LINE символів обрізаються з попередженням.
 * @return кількість прочитаних рядків або (size_t)-1 при нестачі пам'яті
 */
static size_t read_lines(FILE *in, StrList *lines, int interactive)
{
    char buf[MAX_LINE * 4 + 2];     /* до 4 байтів на символ UTF-8 */
    size_t n = 0;
    if (interactive)
        printf("Введіть текст (кілька рядків, до %d символів кожен).\n"
               "Порожній рядок - завершення введення:\n", MAX_LINE);
    while (fgets(buf, sizeof buf, in) != NULL) {
        size_t len = strlen(buf);
        int complete = (len > 0 && buf[len - 1] == '\n');
        buf[strcspn(buf, "\r\n")] = '\0';
        if (!complete && !feof(in)) {                 /* рядок не вмістився - відкинути решту */
            int ch;
            while ((ch = fgetc(in)) != '\n' && ch != EOF) { }
            printf("  Попередження: рядок %zu задовгий і був обрізаний.\n", n + 1);
        }
        if (utf8_strlen(buf) > MAX_LINE) {
            printf("  Попередження: рядок %zu довший за %d символів - обрізано.\n", n + 1, MAX_LINE);
            /* обрізаємо по межі символу */
            size_t chars = 0;
            char *p = buf;
            while (*p != '\0' && chars < MAX_LINE) { uint32_t cp; p += utf8_decode(p, &cp); chars++; }
            *p = '\0';
        }
        if (interactive && buf[0] == '\0') break;
        if (!strlist_add(lines, buf)) return (size_t)-1;
        n++;
    }
    return n;
}

/** Розбиває всі рядки тексту на слова. 0 - нестача пам'яті. */
static int build_words(const StrList *lines, StrList *words)
{
    strlist_free(words);
    for (size_t i = 0; i < lines->count; i++)
        if (split_words(lines->items[i], words) == (size_t)-1) return 0;
    return 1;
}

/* Сигнатуру main визначено стандартом C, тому argv не робимо const. */
// cppcheck-suppress constParameter
int main(int argc, char *argv[])
{
    console_utf8();
    StrList lines, words;
    strlist_init(&lines);
    strlist_init(&words);

    printf("КПІЗ. Частина 4. Варіант 19 - групи слів-анаграм\n");
    if (argc > 1) {                                    /* текст з файла, переданого аргументом */
        FILE *f = fopen(argv[1], "r");
        if (f == NULL) { printf("Помилка: не вдалося відкрити файл %s\n", argv[1]); return 1; }
        if (read_lines(f, &lines, 0) == (size_t)-1 || !build_words(&lines, &words)) {
            printf("Помилка: нестача пам'яті\n");
            fclose(f); strlist_free(&lines); strlist_free(&words);
            return 1;
        }
        fclose(f);
        printf("Завантажено рядків: %zu, слів: %zu\n", lines.count, words.count);
    }

    int choice = -1;
    while (choice != 0) {
        printf("\nМЕНЮ\n 1 - ввести текст з клавіатури\n 2 - вивести текст і слова\n"
               " 3 - знайти групи слів-анаграм\n 4 - статистика тексту\n 0 - вихід\nВаш вибір: ");
        if (scanf("%d", &choice) != 1) {
            if (feof(stdin)) break;
            flush_line();
            printf("  Помилка: введіть номер пункту.\n");
            choice = -1;
            continue;
        }
        flush_line();
        switch (choice) {
            case 1:
                strlist_free(&lines);
                if (read_lines(stdin, &lines, 1) == (size_t)-1 || !build_words(&lines, &words)) {
                    printf("Помилка: нестача пам'яті\n");
                    choice = 0;
                    break;
                }
                printf("Прочитано рядків: %zu, слів: %zu\n", lines.count, words.count);
                break;
            case 2:
                if (lines.count == 0) { printf("Текст ще не введено.\n"); break; }
                printf("Текст:\n");
                for (size_t i = 0; i < lines.count; i++) printf("  %s\n", lines.items[i]);
                printf("Слова (%zu):", words.count);
                for (size_t i = 0; i < words.count; i++) printf(" [%s]", words.items[i]);
                printf("\n");
                break;
            case 3: {
                if (words.count == 0) { printf("Текст ще не введено.\n"); break; }
                size_t g = 0;
                if (!print_anagram_groups(&words, &g)) printf("Помилка: нестача пам'яті\n");
                else printf("Знайдено груп: %zu\n", g);
                break;
            }
            case 4: {
                TextStats st = text_stats(&lines, words.count);
                printf("Символів: %zu\nСлів: %zu\nРечень: %zu\n", st.chars, st.words, st.sentences);
                break;
            }
            case 0:
                printf("Завершення роботи.\n");
                break;
            default:
                printf("  Немає такого пункту меню.\n");
        }
    }
    strlist_free(&lines);
    strlist_free(&words);
    return 0;
}
