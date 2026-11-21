/**
 * @file bus.h
 * @brief КПІЗ, частина 5 (варіант 19). Предметна область «Автопарк».
 * Опис структур даних і операцій з однозв'язним списком автобусів.
 */
#ifndef BUS_H
#define BUS_H

#include <stdio.h>

#define REG_LEN    12   /**< довжина буфера держномера (напр. "AC1234BX")  */
#define MODEL_LEN  32   /**< довжина буфера назви моделі                   */
#define DEFAULT_SERVICE_INTERVAL 15000L  /**< типовий інтервал ТО, км       */

/** Календарна дата (вкладена структура). */
typedef struct {
    int day;
    int month;
    int year;
} Date;

/** Стан автобуса (перелічення). */
typedef enum {
    BUS_ACTIVE = 0,   /**< на лінії            */
    BUS_REPAIR = 1,   /**< у ремонті           */
    BUS_RESERVE = 2   /**< у резерві           */
} BusStatus;

/** Запис про автобус (9 полів). */
typedef struct {
    int       id;                     /**< інвентарний номер (унікальний) */
    char      reg_number[REG_LEN];    /**< державний номер                */
    char      model[MODEL_LEN];       /**< марка/модель                   */
    int       year;                   /**< рік випуску                    */
    int       seats;                  /**< кількість місць                */
    long      mileage;                /**< поточний пробіг, км            */
    long      last_service_km;        /**< пробіг на момент останнього ТО */
    Date      last_service;           /**< дата останнього ТО             */
    BusStatus status;                 /**< стан                           */
} Bus;

/** Вузол однозв'язного списку. */
typedef struct BusNode {
    Bus             data;
    struct BusNode *next;
} BusNode;

/** Однозв'язний список автобусів. */
typedef struct {
    BusNode *head;
    size_t   size;
} BusList;

/** Поля, за якими можливе сортування. */
typedef enum {
    SORT_ID = 1, SORT_REG, SORT_MODEL, SORT_YEAR, SORT_MILEAGE, SORT_SINCE_SERVICE, SORT_DATE
} SortField;

/* ---------------- перевірка та форматування ---------------- */

/** Чи є рік високосним. */
int is_leap(int year);

/** Перевіряє коректність дати (1990..2100). @return 1 - коректна */
int date_valid(Date d);

/** Порівнює дати: <0, якщо a раніше b; 0 - рівні; >0 - пізніше. */
int date_cmp(Date a, Date b);

/** Розбирає дату формату ДД.ММ.РРРР. @return 1 - успішно */
int date_parse(const char *s, Date *d);

/** Назва стану українською. */
const char *status_name(BusStatus s);

/**
 * Перевіряє коректність запису (поля в допустимих межах).
 * @param err [out] повідомлення про помилку (може бути NULL)
 * @return 1 - запис коректний
 */
int bus_valid(const Bus *b, const char **err);

/** Пробіг після останнього ТО, км. */
long km_since_service(const Bus *b);

/** Друкує заголовок таблиці у потік. */
void bus_print_header(FILE *out);

/** Друкує один запис рядком таблиці у потік. */
void bus_print_row(FILE *out, const Bus *b);

/** Друкує нижню межу таблиці. */
void bus_print_footer(FILE *out);

/* ---------------- однозв'язний список ---------------- */

/** Ініціалізує порожній список. */
void list_init(BusList *l);

/** Додає запис у кінець списку. @return 1 - успішно, 0 - нестача пам'яті */
int list_append(BusList *l, const Bus *b);

/** Шукає вузол за інвентарним номером. @return вузол або NULL */
BusNode *list_find_id(const BusList *l, int id);

/** Вилучає вузол з даним id. @return 1 - вилучено, 0 - не знайдено */
int list_remove_id(BusList *l, int id);

/** Звільняє всі вузли списку. */
void list_free(BusList *l);

/** Друкує всі записи списку таблицею. */
void list_print(FILE *out, const BusList *l);

/**
 * Сортує список за вказаним полем (стабільне сортування злиттям вузлів).
 * @param ascending 1 - за зростанням, 0 - за спаданням
 */
void list_sort(BusList *l, SortField field, int ascending);

/** Найбільший id у списку (0 для порожнього). */
int list_max_id(const BusList *l);

#endif
