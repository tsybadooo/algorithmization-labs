/**
 * @file console.h
 * @brief Налаштування консолі на UTF-8 (Windows) для українського тексту.
 */
#ifndef KPIZ_CONSOLE_H
#define KPIZ_CONSOLE_H

#ifdef _WIN32
#include <windows.h>
/** Перемикає кодову сторінку консолі на UTF-8 (введення та виведення). */
static inline void console_utf8(void)
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}
#else
static inline void console_utf8(void) { }
#endif

#include <stdio.h>
/** Очищує залишок рядка у stdin після scanf (до '\n' або EOF). */
static inline void flush_line(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) { }
}

#endif
