/**
 * @file storage.c
 * @brief Реалізація файлових операцій автопарку.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "storage.h"

const char *storage_msg(StorageStatus s)
{
    switch (s) {
        case ST_OK:        return "успішно";
        case ST_OPEN_ERR:  return "не вдалося відкрити файл";
        case ST_READ_ERR:  return "помилка читання або пошкоджений файл";
        case ST_WRITE_ERR: return "помилка запису у файл";
        case ST_NOT_FOUND: return "запис не знайдено";
        case ST_NO_MEMORY: return "нестача пам'яті";
        default:           return "невідома помилка";
    }
}

StorageStatus storage_load(const char *path, BusList *l)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) return ST_OK;                    /* файла ще немає - порожня база */
    Bus b;
    StorageStatus st = ST_OK;
    for (;;) {
        size_t got = fread(&b, sizeof(Bus), 1, f);
        if (got != 1) {
            if (ferror(f)) st = ST_READ_ERR;
            else if (!feof(f)) st = ST_READ_ERR;
            break;                                  /* кінець файла */
        }
        if (!bus_valid(&b, NULL)) { st = ST_READ_ERR; break; }   /* пошкоджений запис */
        if (!list_append(l, &b)) { st = ST_NO_MEMORY; break; }
    }
    /* неповний останній запис: розмір файла не кратний sizeof(Bus) */
    if (st == ST_OK && fseek(f, 0, SEEK_END) == 0 && ftell(f) % (long)sizeof(Bus) != 0) st = ST_READ_ERR;
    fclose(f);
    return st;
}

StorageStatus storage_save(const char *path, const BusList *l)
{
    FILE *f = fopen(path, "wb");
    if (f == NULL) return ST_OPEN_ERR;
    for (const BusNode *p = l->head; p != NULL; p = p->next)
        if (fwrite(&p->data, sizeof(Bus), 1, f) != 1) { fclose(f); return ST_WRITE_ERR; }
    if (fclose(f) != 0) return ST_WRITE_ERR;
    return ST_OK;
}

StorageStatus storage_append(const char *path, const Bus *b)
{
    FILE *f = fopen(path, "ab");
    if (f == NULL) return ST_OPEN_ERR;
    size_t w = fwrite(b, sizeof(Bus), 1, f);
    if (fclose(f) != 0 || w != 1) return ST_WRITE_ERR;
    return ST_OK;
}

StorageStatus storage_update(const char *path, const Bus *b, long *pos_out)
{
    FILE *f = fopen(path, "r+b");
    if (f == NULL) return ST_OPEN_ERR;
    Bus cur;
    StorageStatus st = ST_NOT_FOUND;
    for (;;) {
        long pos = ftell(f);                        /* позиція початку поточного запису */
        if (fread(&cur, sizeof(Bus), 1, f) != 1) {
            if (ferror(f)) st = ST_READ_ERR;
            break;
        }
        if (cur.id == b->id) {
            if (fseek(f, pos, SEEK_SET) != 0) { st = ST_READ_ERR; break; }
            if (fwrite(b, sizeof(Bus), 1, f) != 1) { st = ST_WRITE_ERR; break; }
            if (pos_out != NULL) *pos_out = pos;
            st = ST_OK;
            break;
        }
    }
    if (fclose(f) != 0 && st == ST_OK) st = ST_WRITE_ERR;
    return st;
}

/** Видаляє пробіли на початку і в кінці рядка (на місці). */
static char *trim(char *s)
{
    while (*s == ' ' || *s == '\t') s++;
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == ' ' || s[n - 1] == '\t' || s[n - 1] == '\r' || s[n - 1] == '\n')) s[--n] = '\0';
    return s;
}

/**
 * Розбирає один рядок CSV у структуру (strtok за комами).
 * @return 1 - успішно, 0 - некоректний рядок
 */
static int parse_csv_line(char *line, Bus *b, const char **err)
{
    char *field[9];
    int n = 0;
    for (char *tok = strtok(line, ","); tok != NULL && n < 9; tok = strtok(NULL, ","))
        field[n++] = trim(tok);
    if (n != 9 || strtok(NULL, ",") != NULL) { *err = "очікується 9 полів"; return 0; }

    memset(b, 0, sizeof *b);
    char tail;
    int status;
    if (sscanf(field[0], "%d%c", &b->id, &tail) != 1) { *err = "id не є числом"; return 0; }
    if (strlen(field[1]) >= REG_LEN) { *err = "держномер задовгий"; return 0; }
    strcpy(b->reg_number, field[1]);
    if (strlen(field[2]) >= MODEL_LEN) { *err = "назва моделі задовга"; return 0; }
    strcpy(b->model, field[2]);
    if (sscanf(field[3], "%d%c", &b->year, &tail) != 1) { *err = "рік не є числом"; return 0; }
    if (sscanf(field[4], "%d%c", &b->seats, &tail) != 1) { *err = "місця не є числом"; return 0; }
    if (sscanf(field[5], "%ld%c", &b->mileage, &tail) != 1) { *err = "пробіг не є числом"; return 0; }
    if (sscanf(field[6], "%ld%c", &b->last_service_km, &tail) != 1) { *err = "пробіг ТО не є числом"; return 0; }
    if (!date_parse(field[7], &b->last_service)) { *err = "некоректна дата ТО"; return 0; }
    if (sscanf(field[8], "%d%c", &status, &tail) != 1) { *err = "стан не є числом"; return 0; }
    b->status = (BusStatus)status;
    return bus_valid(b, err);
}

StorageStatus storage_import_csv(const char *path, BusList *l, int *imported, int *skipped)
{
    *imported = *skipped = 0;
    FILE *f = fopen(path, "r");
    if (f == NULL) return ST_OPEN_ERR;
    char line[512];
    int lineno = 0;
    while (fgets(line, sizeof line, f) != NULL) {
        lineno++;
        if (lineno == 1) continue;                  /* заголовок */
        if (trim(line)[0] == '\0') continue;        /* порожній рядок */
        Bus b;
        const char *err = NULL;
        if (!parse_csv_line(line, &b, &err)) {
            printf("  Рядок %d пропущено: %s\n", lineno, err ? err : "помилка формату");
            (*skipped)++;
            continue;
        }
        if (list_find_id(l, b.id) != NULL) {
            printf("  Рядок %d пропущено: id %d уже існує\n", lineno, b.id);
            (*skipped)++;
            continue;
        }
        if (!list_append(l, &b)) { fclose(f); return ST_NO_MEMORY; }
        (*imported)++;
    }
    StorageStatus st = ferror(f) ? ST_READ_ERR : ST_OK;
    fclose(f);
    return st;
}

StorageStatus report_service_due(const char *path, const BusList *l, long interval, int *found)
{
    *found = 0;
    /* відбір у тимчасовий список і сортування за перевищенням (спадання) */
    BusList due;
    list_init(&due);
    for (BusNode *p = l->head; p != NULL; p = p->next)
        if (km_since_service(&p->data) >= interval)
            if (!list_append(&due, &p->data)) { list_free(&due); return ST_NO_MEMORY; }
    list_sort(&due, SORT_SINCE_SERVICE, 0);
    *found = (int)due.size;

    FILE *f = fopen(path, "w");
    if (f == NULL) { list_free(&due); return ST_OPEN_ERR; }
    time_t now = time(NULL);
    char stamp[32];
    strftime(stamp, sizeof stamp, "%d.%m.%Y %H:%M", localtime(&now));

    FILE *outs[2] = {stdout, f};
    for (int k = 0; k < 2; k++) {
        FILE *out = outs[k];
        fprintf(out, "ЗВІТ: автобуси, яким за пробігом потрібне технічне обслуговування\n");
        fprintf(out, "Інтервал ТО: %ld км. Сформовано: %s\n", interval, stamp);
        if (due.size == 0) {
            fprintf(out, "Таких автобусів немає.\n");
            continue;
        }
        bus_print_header(out);
        for (BusNode *p = due.head; p != NULL; p = p->next) bus_print_row(out, &p->data);
        bus_print_footer(out);
        fprintf(out, "Усього: %zu з %zu автобусів. Перевищення інтервалу (км):", due.size, l->size);
        for (BusNode *p = due.head; p != NULL; p = p->next)
            fprintf(out, " %s=%ld", p->data.reg_number, km_since_service(&p->data) - interval);
        fprintf(out, "\n");
    }
    list_free(&due);
    if (fclose(f) != 0) return ST_WRITE_ERR;
    return ST_OK;
}
