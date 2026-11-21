/* common_console.h - налаштування консолі Windows на UTF-8,
   щоб коректно виводився український текст. */
#ifndef COMMON_CONSOLE_H
#define COMMON_CONSOLE_H
#ifdef _WIN32
#include <windows.h>
static void console_utf8(void) { SetConsoleOutputCP(CP_UTF8); SetConsoleCP(CP_UTF8); }
#else
static void console_utf8(void) { }
#endif
#endif
