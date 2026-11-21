/**
 * @file main.c
 * @brief КПІЗ, частина 5 (варіант 19). «Автопарк»: облік автобусів,
 *        запит - автобуси, яким за пробігом потрібне ТО.
 *
 * Записи зберігаються в бінарному файлі buses.dat, в оперативній пам'яті -
 * в однозв'язному списку. Консольне меню українською мовою.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bus.h"
#include "storage.h"
#include "../common/console.h"

#define DATA_FILE   "buses.dat"
#define REPORT_FILE "report.txt"
#define CSV_FILE    "buses.csv"

/**
 * Читає рядок з клавіатури (fgets) без символу '\n'.
 * @return 1 - успішно, 0 - кінець вхідного потоку
 */
static int read_line(const char *prompt, char *buf, size_t size)
{
    printf("%s", prompt);
    if (fgets(buf, (int)size, stdin) == NULL) return 0;
    size_t n = strcspn(buf, "\r\n");
    if (buf[n] == '\0' && n == size - 1) flush_line();   /* рядок задовгий - відкинути решту */
    buf[n] = '\0';
    return 1;
}

/**
 * Читає ціле в межах [lo; hi] з повторним запитом.
 * Порожній ввід при наявності def_value повертає значення за замовчуванням.
 * @return 1 - успішно, 0 - кінець вводу
 */
static int read_long(const char *prompt, long lo, long hi, long *value, const long *def_value)
{
    char buf[64], tail;
    for (;;) {
        if (!read_line(prompt, buf, sizeof buf)) return 0;
        if (buf[0] == '\0' && def_value != NULL) { *value = *def_value; return 1; }
        if (sscanf(buf, "%ld %c", value, &tail) == 1 && *value >= lo && *value <= hi) return 1;
        printf("  Помилка: потрібне ціле число від %ld до %ld.\n", lo, hi);
    }
}

/** Читає рядок довжиною 1..maxlen-1 символів; def - значення для порожнього вводу. */
static int read_text(const char *prompt, char *out, size_t maxlen, const char *def)
{
    char buf[128];
    for (;;) {
        if (!read_line(prompt, buf, sizeof buf)) return 0;
        if (buf[0] == '\0' && def != NULL) { strcpy(out, def); return 1; }
        if (buf[0] != '\0' && strlen(buf) < maxlen && strchr(buf, ',') == NULL) { strcpy(out, buf); return 1; }
        printf("  Помилка: рядок має бути непорожнім, без коми, до %zu байт.\n", maxlen - 1);
    }
}

/** Читає дату ДД.ММ.РРРР; def - значення для порожнього вводу (може бути NULL). */
static int read_date(const char *prompt, Date *d, const Date *def)
{
    char buf[64];
    for (;;) {
        if (!read_line(prompt, buf, sizeof buf)) return 0;
        if (buf[0] == '\0' && def != NULL) { *d = *def; return 1; }
        if (date_parse(buf, d)) return 1;
        printf("  Помилка: потрібна існуюча дата у форматі ДД.ММ.РРРР (1990..2100).\n");
    }
}

/**
 * Введення (або редагування, якщо old != NULL) полів запису.
 * Під час редагування порожній ввід залишає старе значення.
 * @return 1 - запис введено коректно, 0 - скасовано / кінець вводу
 */
static int input_bus(Bus *b, const Bus *old)
{
    char prompt[128];
    long v, def;
    if (old != NULL) printf("(Enter - залишити поточне значення)\n");

    snprintf(prompt, sizeof prompt, "Держномер%s%s%s: ", old ? " [" : "", old ? old->reg_number : "", old ? "]" : "");
    for (;;) {   /* BUG-05-01: довжину держномера перевіряємо одразу, а не після введення всіх полів */
        if (!read_text(prompt, b->reg_number, REG_LEN, old ? old->reg_number : NULL)) return 0;
        if (strlen(b->reg_number) >= 4) break;
        printf("  Помилка: держномер має містити від 4 до %d символів.\n", REG_LEN - 1);
    }
    snprintf(prompt, sizeof prompt, "Модель%s%s%s: ", old ? " [" : "", old ? old->model : "", old ? "]" : "");
    if (!read_text(prompt, b->model, MODEL_LEN, old ? old->model : NULL)) return 0;

    def = old ? old->year : 0;
    snprintf(prompt, sizeof prompt, "Рік випуску (1990..2100)%s: ", old ? " [поточне]" : "");
    if (!read_long(prompt, 1990, 2100, &v, old ? &def : NULL)) return 0;
    b->year = (int)v;

    def = old ? old->seats : 0;
    snprintf(prompt, sizeof prompt, "Кількість місць (1..300)%s: ", old ? " [поточне]" : "");
    if (!read_long(prompt, 1, 300, &v, old ? &def : NULL)) return 0;
    b->seats = (int)v;

    def = old ? old->mileage : 0;
    snprintf(prompt, sizeof prompt, "Поточний пробіг, км%s: ", old ? " [поточне]" : "");
    if (!read_long(prompt, 0, 5000000L, &v, old ? &def : NULL)) return 0;
    b->mileage = v;

    for (;;) {
        def = old ? old->last_service_km : 0;
        snprintf(prompt, sizeof prompt, "Пробіг на момент останнього ТО, км%s: ", old ? " [поточне]" : "");
        if (!read_long(prompt, 0, 5000000L, &v, old ? &def : NULL)) return 0;
        if (v <= b->mileage) break;
        printf("  Помилка: пробіг на ТО не може перевищувати поточний пробіг (%ld).\n", b->mileage);
    }
    b->last_service_km = v;

    for (;;) {
        if (!read_date("Дата останнього ТО (ДД.ММ.РРРР): ", &b->last_service, old ? &old->last_service : NULL)) return 0;
        if (b->last_service.year >= b->year) break;
        printf("  Помилка: дата ТО не може бути раніше року випуску.\n");
    }

    def = old ? (long)old->status : 0;
    if (!read_long("Стан (0 - на лінії, 1 - у ремонті, 2 - у резерві): ", 0, 2, &v, old ? &def : NULL)) return 0;
    b->status = (BusStatus)v;

    const char *err = NULL;
    if (!bus_valid(b, &err)) { printf("  Запис некоректний: %s\n", err); return 0; }
    return 1;
}

/** Обробляє код результату файлової операції; повертає 1 при успіху. */
static int check(StorageStatus st, const char *action)
{
    if (st == ST_OK) return 1;
    printf("  Помилка (%s): %s\n", action, storage_msg(st));
    return 0;
}

/** Пункт меню «Пошук за держномером» (точний збіг без урахування регістру). */
static void search_by_reg(const BusList *l)
{
    char key[REG_LEN + 8];
    if (!read_text("Держномер для пошуку: ", key, sizeof key, NULL)) return;
    int found = 0;
    for (BusNode *p = l->head; p != NULL; p = p->next) {
        const char *a = p->data.reg_number, *b = key;
        while (*a && *b && toupper((unsigned char)*a) == toupper((unsigned char)*b)) { a++; b++; }
        if (*a == '\0' && *b == '\0') {
            if (!found) bus_print_header(stdout);
            bus_print_row(stdout, &p->data);
            found++;
        }
    }
    if (found) bus_print_footer(stdout); else printf("Автобус з номером %s не знайдено.\n", key);
}

/** Пункт меню «Пошук за моделлю» (входження підрядка, strstr). */
static void search_by_model(const BusList *l)
{
    char key[MODEL_LEN];
    if (!read_text("Модель або її частина: ", key, sizeof key, NULL)) return;
    int found = 0;
    for (BusNode *p = l->head; p != NULL; p = p->next)
        if (strstr(p->data.model, key) != NULL) {
            if (!found) bus_print_header(stdout);
            bus_print_row(stdout, &p->data);
            found++;
        }
    if (found) { bus_print_footer(stdout); printf("Знайдено: %d\n", found); }
    else printf("Автобусів моделі «%s» не знайдено.\n", key);
}

int main(void)
{
    console_utf8();
    BusList list;
    list_init(&list);
    if (!check(storage_load(DATA_FILE, &list), "завантаження " DATA_FILE)) {
        printf("  Робота продовжується з даними, які вдалося прочитати (%zu записів).\n", list.size);
    }
    printf("КПІЗ. Частина 5. Варіант 19 - «Автопарк»\nЗавантажено записів: %zu\n", list.size);

    long choice = -1;
    while (choice != 0) {
        printf("\nМЕНЮ\n"
               " 1 - імпорт початкових даних з %s\n"
               " 2 - перегляд усіх записів\n"
               " 3 - додавання запису\n"
               " 4 - редагування запису (у файлі, fseek)\n"
               " 5 - вилучення запису\n"
               " 6 - пошук за держномером\n"
               " 7 - пошук за моделлю\n"
               " 8 - сортування за вибраним полем\n"
               " 9 - запит: автобуси, яким потрібне ТО (звіт у %s)\n"
               " 0 - вихід\n", CSV_FILE, REPORT_FILE);
        if (!read_long("Ваш вибір: ", 0, 9, &choice, NULL)) break;

        switch (choice) {
            case 1: {
                int imp, skip;
                if (check(storage_import_csv(CSV_FILE, &list, &imp, &skip), "імпорт CSV")) {
                    printf("Імпортовано: %d, пропущено: %d\n", imp, skip);
                    check(storage_save(DATA_FILE, &list), "збереження");
                }
                break;
            }
            case 2:
                if (list.size == 0) printf("Записи відсутні.\n");
                else { list_print(stdout, &list); printf("Усього записів: %zu\n", list.size); }
                break;
            case 3: {
                Bus b;
                memset(&b, 0, sizeof b);
                b.id = list_max_id(&list) + 1;
                printf("Новий запис, інвентарний номер %d\n", b.id);
                if (!input_bus(&b, NULL)) { printf("Додавання скасовано.\n"); break; }
                if (!list_append(&list, &b)) { printf("Помилка: нестача пам'яті\n"); break; }
                if (check(storage_append(DATA_FILE, &b), "запис у файл")) printf("Запис додано.\n");
                break;
            }
            case 4: {
                long id;
                if (!read_long("Інвентарний номер для редагування: ", 1, 1000000, &id, NULL)) break;
                BusNode *node = list_find_id(&list, (int)id);
                if (node == NULL) { printf("Запис з id %ld не знайдено.\n", id); break; }
                Bus edited = node->data;
                if (!input_bus(&edited, &node->data)) { printf("Редагування скасовано.\n"); break; }
                long pos = -1;
                if (check(storage_update(DATA_FILE, &edited, &pos), "оновлення у файлі")) {
                    node->data = edited;
                    printf("Запис оновлено у файлі (зміщення %ld байт, запис №%ld).\n", pos, pos / (long)sizeof(Bus) + 1);
                }
                break;
            }
            case 5: {
                long id;
                if (!read_long("Інвентарний номер для вилучення: ", 1, 1000000, &id, NULL)) break;
                if (!list_remove_id(&list, (int)id)) { printf("Запис з id %ld не знайдено.\n", id); break; }
                if (check(storage_save(DATA_FILE, &list), "збереження")) printf("Запис вилучено.\n");
                break;
            }
            case 6: search_by_reg(&list); break;
            case 7: search_by_model(&list); break;
            case 8: {
                long field, asc;
                if (!read_long("Поле: 1-id 2-держномер 3-модель 4-рік 5-пробіг 6-пробіг після ТО 7-дата ТО: ",
                               1, 7, &field, NULL)) break;
                if (!read_long("Порядок: 1 - за зростанням, 0 - за спаданням: ", 0, 1, &asc, NULL)) break;
                list_sort(&list, (SortField)field, (int)asc);
                list_print(stdout, &list);
                check(storage_save(DATA_FILE, &list), "збереження");
                break;
            }
            case 9: {
                long interval, def = DEFAULT_SERVICE_INTERVAL;
                if (!read_long("Інтервал ТО, км (Enter - 15000): ", 1000, 100000, &interval, &def)) break;
                int found;
                if (check(report_service_due(REPORT_FILE, &list, interval, &found), "звіт"))
                    printf("Звіт збережено у файл %s (знайдено: %d)\n", REPORT_FILE, found);
                break;
            }
            case 0:
                printf("Завершення роботи.\n");
                break;
            default:
                break;
        }
    }
    list_free(&list);
    return 0;
}
