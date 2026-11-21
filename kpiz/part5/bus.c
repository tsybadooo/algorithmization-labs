/**
 * @file bus.c
 * @brief Реалізація операцій із записами автопарку та однозв'язним списком.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bus.h"

/* ---------------- перевірка та форматування ---------------- */

int is_leap(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

int date_valid(Date d)
{
    static const int dm[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (d.year < 1990 || d.year > 2100 || d.month < 1 || d.month > 12) return 0;
    int maxd = dm[d.month - 1] + (d.month == 2 && is_leap(d.year));
    return d.day >= 1 && d.day <= maxd;
}

int date_cmp(Date a, Date b)
{
    if (a.year != b.year) return a.year - b.year;
    if (a.month != b.month) return a.month - b.month;
    return a.day - b.day;
}

int date_parse(const char *s, Date *d)
{
    char tail;
    if (sscanf(s, "%d.%d.%d%c", &d->day, &d->month, &d->year, &tail) != 3) return 0;
    return date_valid(*d);
}

const char *status_name(BusStatus s)
{
    switch (s) {
        case BUS_ACTIVE:  return "на лінії";
        case BUS_REPAIR:  return "у ремонті";
        case BUS_RESERVE: return "у резерві";
        default:          return "невідомо";
    }
}

int bus_valid(const Bus *b, const char **err)
{
    const char *e = NULL;
    if (b->id <= 0) e = "інвентарний номер має бути додатним";
    else if (strlen(b->reg_number) < 4) e = "держномер занадто короткий";
    else if (b->model[0] == '\0') e = "модель не вказано";
    else if (b->year < 1990 || b->year > 2100) e = "рік випуску поза межами 1990..2100";
    else if (b->seats < 1 || b->seats > 300) e = "кількість місць поза межами 1..300";
    else if (b->mileage < 0 || b->mileage > 5000000L) e = "пробіг поза межами 0..5000000 км";
    else if (b->last_service_km < 0 || b->last_service_km > b->mileage) e = "пробіг на ТО більший за поточний";
    else if (!date_valid(b->last_service)) e = "некоректна дата ТО";
    else if (b->last_service.year < b->year) e = "дата ТО раніша за рік випуску";
    else if (b->status < BUS_ACTIVE || b->status > BUS_RESERVE) e = "некоректний стан";
    if (err != NULL) *err = e;
    return e == NULL;
}

long km_since_service(const Bus *b)
{
    return b->mileage - b->last_service_km;
}

/** Друкує рядок UTF-8, доповнений пробілами до width символів (не байтів). */
static void print_padded(FILE *out, const char *s, int width)
{
    int chars = 0;
    for (const char *p = s; *p; p++)
        if (((unsigned char)*p & 0xC0) != 0x80) chars++;
    fprintf(out, "%s%*s", s, width > chars ? width - chars : 0, "");
}

void bus_print_header(FILE *out)
{
    fprintf(out, "+------+------------+----------------------+------+------+----------+----------+------------+------------+\n");
    fprintf(out, "|  ID  | Держномер  | Модель               | Рік  | Місць|Пробіг, км|Після ТО  | Дата ТО    | Стан       |\n");
    fprintf(out, "+------+------------+----------------------+------+------+----------+----------+------------+------------+\n");
}

void bus_print_row(FILE *out, const Bus *b)
{
    fprintf(out, "| %4d | ", b->id);
    print_padded(out, b->reg_number, 10);
    fprintf(out, " | ");
    print_padded(out, b->model, 20);
    fprintf(out, " | %4d | %4d | %8ld | %8ld | %02d.%02d.%04d | ",
            b->year, b->seats, b->mileage, km_since_service(b),
            b->last_service.day, b->last_service.month, b->last_service.year);
    print_padded(out, status_name(b->status), 10);
    fprintf(out, " |\n");
}

void bus_print_footer(FILE *out)
{
    fprintf(out, "+------+------------+----------------------+------+------+----------+----------+------------+------------+\n");
}

/* ---------------- однозв'язний список ---------------- */

void list_init(BusList *l)
{
    l->head = NULL;
    l->size = 0;
}

int list_append(BusList *l, const Bus *b)
{
    BusNode *node = (BusNode *)malloc(sizeof(BusNode));
    if (node == NULL) return 0;
    node->data = *b;
    node->next = NULL;
    if (l->head == NULL) {
        l->head = node;
    } else {
        BusNode *p = l->head;
        while (p->next != NULL) p = p->next;
        p->next = node;
    }
    l->size++;
    return 1;
}

BusNode *list_find_id(const BusList *l, int id)
{
    for (BusNode *p = l->head; p != NULL; p = p->next)
        if (p->data.id == id) return p;
    return NULL;
}

int list_remove_id(BusList *l, int id)
{
    BusNode **pp = &l->head;              /* покажчик на поле, що вказує на поточний вузол */
    while (*pp != NULL) {
        if ((*pp)->data.id == id) {
            BusNode *victim = *pp;
            *pp = victim->next;
            free(victim);
            l->size--;
            return 1;
        }
        pp = &(*pp)->next;
    }
    return 0;
}

void list_free(BusList *l)
{
    BusNode *p = l->head;
    while (p != NULL) {
        BusNode *next = p->next;
        free(p);
        p = next;
    }
    list_init(l);
}

void list_print(FILE *out, const BusList *l)
{
    bus_print_header(out);
    for (BusNode *p = l->head; p != NULL; p = p->next) bus_print_row(out, &p->data);
    bus_print_footer(out);
}

int list_max_id(const BusList *l)
{
    int mx = 0;
    for (BusNode *p = l->head; p != NULL; p = p->next)
        if (p->data.id > mx) mx = p->data.id;
    return mx;
}

/** Порівняння двох записів за полем. */
static int bus_cmp(const Bus *a, const Bus *b, SortField f)
{
    switch (f) {
        case SORT_ID:            return (a->id > b->id) - (a->id < b->id);
        case SORT_REG:           return strcmp(a->reg_number, b->reg_number);
        case SORT_MODEL:         return strcmp(a->model, b->model);
        case SORT_YEAR:          return (a->year > b->year) - (a->year < b->year);
        case SORT_MILEAGE:       return (a->mileage > b->mileage) - (a->mileage < b->mileage);
        case SORT_SINCE_SERVICE: {
            long x = km_since_service(a), y = km_since_service(b);
            return (x > y) - (x < y);
        }
        case SORT_DATE:          return date_cmp(a->last_service, b->last_service);
        default:                 return 0;
    }
}

/** Злиття двох відсортованих ланцюжків вузлів (стабільне). */
static BusNode *merge_nodes(BusNode *a, BusNode *b, SortField f, int asc)
{
    BusNode dummy;
    BusNode *tail = &dummy;
    dummy.next = NULL;
    while (a != NULL && b != NULL) {
        int c = bus_cmp(&a->data, &b->data, f);
        if (!asc) c = -c;
        if (c <= 0) { tail->next = a; a = a->next; }
        else        { tail->next = b; b = b->next; }
        tail = tail->next;
    }
    tail->next = (a != NULL) ? a : b;
    return dummy.next;
}

/** Рекурсивне сортування злиттям ланцюжка вузлів, O(n log n). */
static BusNode *merge_sort_nodes(BusNode *head, SortField f, int asc)
{
    if (head == NULL || head->next == NULL) return head;
    BusNode *slow = head, *fast = head->next;    /* пошук середини: "черепаха і заєць" */
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    BusNode *second = slow->next;
    slow->next = NULL;
    return merge_nodes(merge_sort_nodes(head, f, asc), merge_sort_nodes(second, f, asc), f, asc);
}

void list_sort(BusList *l, SortField field, int ascending)
{
    l->head = merge_sort_nodes(l->head, field, ascending);
}
