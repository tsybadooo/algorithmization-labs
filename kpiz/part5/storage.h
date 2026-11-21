/**
 * @file storage.h
 * @brief КПІЗ, частина 5. Робота з файлами: бінарне сховище записів,
 *        імпорт з CSV, текстовий звіт.
 */
#ifndef STORAGE_H
#define STORAGE_H

#include "bus.h"

/** Коди результату файлових операцій. */
typedef enum {
    ST_OK = 0,        /**< успішно                         */
    ST_OPEN_ERR,      /**< не вдалося відкрити файл        */
    ST_READ_ERR,      /**< помилка читання / пошкоджений файл */
    ST_WRITE_ERR,     /**< помилка запису                  */
    ST_NOT_FOUND,     /**< запис не знайдено               */
    ST_NO_MEMORY      /**< нестача пам'яті                 */
} StorageStatus;

/** Текстовий опис коду результату. */
const char *storage_msg(StorageStatus s);

/**
 * Завантажує всі записи з бінарного файла у список (fread).
 * Відсутній файл не є помилкою - список лишається порожнім.
 */
StorageStatus storage_load(const char *path, BusList *l);

/** Повністю перезаписує бінарний файл вмістом списку (fwrite). */
StorageStatus storage_save(const char *path, const BusList *l);

/** Дописує один запис у кінець бінарного файла. */
StorageStatus storage_append(const char *path, const Bus *b);

/**
 * Оновлює запис з тим самим id безпосередньо у файлі: послідовне
 * читання до потрібного запису, ftell/fseek на його початок, fwrite.
 * @param pos_out [out] зміщення запису у файлі, байт (може бути NULL)
 */
StorageStatus storage_update(const char *path, const Bus *b, long *pos_out);

/**
 * Імпортує записи з CSV (перший рядок - заголовок; роздільник - кома).
 * Формат: id,reg_number,model,year,seats,mileage,last_service_km,ДД.ММ.РРРР,status
 * Некоректні рядки та дублікати id пропускаються з повідомленням.
 * @param imported [out] кількість імпортованих записів
 * @param skipped  [out] кількість пропущених рядків
 */
StorageStatus storage_import_csv(const char *path, BusList *l, int *imported, int *skipped);

/**
 * Запит варіанта: автобуси, яким за пробігом потрібне ТО
 * (пробіг після останнього ТО >= interval). Результат виводиться на екран
 * і зберігається у текстовий файл у вигляді таблиці (fprintf).
 * @param found [out] кількість знайдених автобусів
 */
StorageStatus report_service_due(const char *path, const BusList *l, long interval, int *found);

#endif
