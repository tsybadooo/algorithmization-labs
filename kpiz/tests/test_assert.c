/**
 * @file test_assert.c
 * @brief КПІЗ, частина 6. Модульні тести на основі макросу assert для
 *        функцій частин 2, 3 і 4 (варіант 19).
 *
 * Ідентифікатори TC-xx-yy відповідають тест-кейсам пояснювальної записки.
 * Після кожного успішного тесту друкується його ідентифікатор.
 */
#undef NDEBUG
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "../part2/func.h"
#include "../part3/matrix.h"
#include "../part3/sort.h"
#include "../part3/search.h"
#include "../part4/text.h"
#include "../common/console.h"

#define EPS 1e-6
#define PASS(id, name) printf("  [OK] %-9s %s\n", id, name)

/* ------------------------- частина 2: y(x) ------------------------- */
static void test_calc_y(void)
{
    const double a = 1.0, PI = KPIZ_PI;
    int ok;

    /* класи еквівалентності: по одному представнику з кожної гілки */
    assert(fabs(calc_y(-5.0, a, &ok) - fabs(sin(-5.0))) < EPS && ok == 1);
    PASS("TC-02-01", "клас x < -pi (x = -5)");
    assert(fabs(calc_y(0.0, a, &ok) - 1.0) < EPS && ok == 1);
    PASS("TC-02-02", "клас -pi <= x <= pi (x = 0)");
    assert(fabs(calc_y(5.0, a, &ok) - 1.0 / sqrt(5.0 - PI)) < EPS && ok == 1);
    PASS("TC-02-03", "клас x > pi (x = 5)");

    /* аналіз граничних значень навколо x = -pi та x = pi */
    assert(branch_of(-PI - 1e-9) == 1 && fabs(calc_y(-PI - 1e-9, a, &ok) - fabs(sin(-PI - 1e-9))) < EPS);
    PASS("TC-02-04", "границя: x = -pi - 1e-9 -> гілка 1");
    assert(branch_of(-PI) == 2 && fabs(calc_y(-PI, a, &ok) - (-PI + a)) < EPS);
    PASS("TC-02-05", "границя: x = -pi -> гілка 2, y = a - pi");
    assert(branch_of(PI) == 2 && fabs(calc_y(PI, a, &ok) - (PI + a)) < EPS && ok == 1);
    PASS("TC-02-06", "границя: x = pi -> гілка 2, y = pi + a");
    assert(branch_of(PI + 1e-6) == 3 && ok == 1);
    calc_y(PI + 1e-6, a, &ok);
    assert(ok == 1 && fabs(calc_y(PI + 1e-6, a, &ok) - 1000.0) < 1e-2);
    PASS("TC-02-07", "границя: x = pi + 1e-6 -> гілка 3, y ~ 1000");

    /* параметр a впливає лише на другу гілку (таблиця рішень) */
    assert(fabs(calc_y(1.0, 2.5, &ok) - 3.5) < EPS);
    assert(fabs(calc_y(-4.0, 2.5, &ok) - calc_y(-4.0, 1.0, &ok)) < EPS);
    PASS("TC-02-08", "таблиця рішень: вплив параметра a");

    /* кількість точок табулювання з урахуванням похибки округлення */
    assert(points_count(0.0, 1.0, 0.1) == 11);
    assert(points_count(-4.0, 4.5, 0.5) == 18);
    assert(points_count(1.0, 1.0, 0.5) == 1);
    assert(points_count(0.0, 1.0, 0.0) == 0 && points_count(1.0, 0.0, 0.1) == 0);
    PASS("TC-02-09", "points_count: 0.1-крок, вироджений відрізок, h = 0");
}

/* --------------------- частина 3: матриця і сортування --------------------- */

/** Створює матрицю з плоского масиву значень. */
static void make(Matrix *m, size_t r, size_t c, const int *vals)
{
    assert(matrix_create(m, r, c));
    for (size_t i = 0; i < r; i++)
        for (size_t j = 0; j < c; j++) m->data[i][j] = vals[i * c + j];
}

static void test_local_minima(void)
{
    Matrix m;
    Cell cells[16];

    const int v1[] = {7};
    make(&m, 1, 1, v1);
    assert(count_local_minima(&m, cells, 16) == 1);
    matrix_free(&m);
    PASS("TC-03-01", "матриця 1x1: елемент без сусідів - мінімум");

    const int v2[] = {5, 5, 5, 5};
    make(&m, 2, 2, v2);
    assert(count_local_minima(&m, cells, 16) == 0);
    matrix_free(&m);
    PASS("TC-03-02", "усі елементи рівні - мінімумів немає (строга нерівність)");

    const int v3[] = {5, 8, 2, 9, 4,
                      7, 1, 6, 3, 8,
                      9, 4, 8, 7, 0,
                      2, 6, 5, 9, 3};
    make(&m, 4, 5, v3);
    size_t n = count_local_minima(&m, cells, 16);
    assert(n == 3);
    assert(cells[0].row == 1 && cells[0].col == 1);
    assert(cells[1].row == 2 && cells[1].col == 4);
    assert(cells[2].row == 3 && cells[2].col == 0);
    matrix_free(&m);
    PASS("TC-03-03", "типова матриця 4x5: 3 мінімуми, у т.ч. у кутах");

    const int v4[] = {3, 1, 4, 1, 5};
    make(&m, 1, 5, v4);
    assert(count_local_minima(&m, NULL, 0) == 2);
    matrix_free(&m);
    PASS("TC-03-04", "один рядок 1x5: сусіди лише ліворуч/праворуч");

    const int v5[] = {2, 9, 9, 2};
    make(&m, 4, 1, v5);
    assert(count_local_minima(&m, cells, 1) == 2);    /* масив на 1 елемент - без переповнення */
    matrix_free(&m);
    PASS("TC-03-05", "один стовпець 4x1, буфер координат менший за результат");

    assert(matrix_create(&m, 0, 5) == 0 && m.data == NULL);
    assert(count_local_minima(&m, cells, 16) == 0);
    assert(count_local_minima(NULL, cells, 16) == 0);
    PASS("TC-03-06", "некоректний розмір 0x5 / NULL - відмова, 0 мінімумів");
}

static void test_sorts(void)
{
    sort_fn algs[2] = {bubble_sort, quick_sort};
    for (int a = 0; a < 2; a++) {
        int empty[1] = {42};
        SortStats st = {0, 0};
        algs[a](empty, 0, &st);
        assert(st.comparisons == 0 && empty[0] == 42);

        int one[1] = {5};
        algs[a](one, 1, &st);
        assert(one[0] == 5);

        int arr[] = {9, -3, 7, 0, 7, -50, 50, 1};
        algs[a](arr, 8, &st);
        const int exp[] = {-50, -3, 0, 1, 7, 7, 9, 50};
        assert(memcmp(arr, exp, sizeof exp) == 0);

        int sorted[] = {1, 2, 3, 4, 5};
        SortStats s2 = {0, 0};
        algs[a](sorted, 5, &s2);
        assert(is_sorted(sorted, 5));
        if (a == 0) assert(s2.comparisons == 4 && s2.swaps == 0);  /* рання зупинка бульбашки */
    }
    PASS("TC-03-07", "сортування: 0, 1, дублікати, від'ємні, вже впорядкований");

    int rev[10];
    for (int i = 0; i < 10; i++) rev[i] = 10 - i;
    SortStats st = {0, 0};
    bubble_sort(rev, 10, &st);
    assert(is_sorted(rev, 10) && st.comparisons == 45 && st.swaps == 45);
    PASS("TC-03-08", "бульбашка, гірший випадок n=10: 45 порівнянь, 45 обмінів");
}

static void test_search(void)
{
    const int a[] = {-5, 0, 3, 3, 8, 12, 40};
    unsigned long c;
    assert(linear_search(a, 7, 8, &c) == 4 && c == 5);
    assert(linear_search(a, 7, 100, &c) == -1 && c == 7);
    assert(binary_search(a, 7, 8, &c) == 4 && c <= 3);
    assert(binary_search(a, 7, -5, &c) == 0);
    assert(binary_search(a, 7, 40, &c) == 6);
    assert(binary_search(a, 7, 4, &c) == -1);
    assert(binary_search(a, 0, 4, &c) == -1 && c == 0);
    long i3 = binary_search(a, 7, 3, &c);
    assert(i3 == 2 || i3 == 3);
    PASS("TC-03-09", "пошук: перший/останній/відсутній/порожній масив");
}

/* ------------------------- частина 4: текст ------------------------- */
static void test_text(void)
{
    char key1[64], key2[64], low[64];

    assert(utf8_strlen("") == 0 && utf8_strlen("abc") == 3 && utf8_strlen("Їжак") == 4);
    PASS("TC-04-01", "utf8_strlen: порожній, ASCII, кирилиця");

    word_to_lower("ҐЄІЇ Abc", low);
    assert(strcmp(low, "ґєії abc") == 0);
    PASS("TC-04-02", "word_to_lower: українські літери Ґ Є І Ї");

    make_anagram_key("Кабан", key1);
    make_anagram_key("БАНКА", key2);
    assert(strcmp(key1, key2) == 0);
    make_anagram_key("рамку", key1);
    make_anagram_key("мурку", key2);
    assert(strcmp(key1, key2) != 0);
    make_anagram_key("м'ята", key1);
    make_anagram_key("тяма", key2);
    assert(strcmp(key1, key2) == 0);
    PASS("TC-04-03", "ключ анаграми: регістр, апостроф, не-анаграми");

    StrList w, g;
    strlist_init(&w);
    strlist_init(&g);
    assert(split_words("Кабан, «банка» — клоун; колун!  ", &w) == 4);
    assert(strcmp(w.items[1], "банка") == 0);
    assert(find_anagram_groups(&w, &g) == 1 && g.count == 2);
    assert(strcmp(g.items[0], "банка кабан") == 0);
    assert(strcmp(g.items[1], "клоун колун") == 0);
    strlist_free(&w);
    strlist_free(&g);
    PASS("TC-04-04", "split_words + групи: лапки, тире, дві групи");

    assert(split_words("Ліс ліс ЛІС сил", &w) == 4);
    assert(find_anagram_groups(&w, &g) == 1 && g.count == 0);   /* однакові слова - не група */
    strlist_free(&w);
    strlist_free(&g);
    PASS("TC-04-05", "повтори одного слова не утворюють групу");

    StrList lines;
    strlist_init(&lines);
    strlist_add(&lines, "Перше речення. Друге?! Третє...");
    strlist_add(&lines, "Четверте без крапки");
    TextStats st = text_stats(&lines, 6);
    assert(st.sentences == 4 && st.words == 6 && st.chars == utf8_strlen(lines.items[0]) + utf8_strlen(lines.items[1]));
    strlist_free(&lines);
    PASS("TC-04-06", "статистика: \"?!\" і \"...\" - один кінець речення");
}

int main(void)
{
    console_utf8();
    printf("Модульні тести (assert), КПІЗ варіант 19\n");
    printf("Частина 2:\n"); test_calc_y();
    printf("Частина 3:\n"); test_local_minima(); test_sorts(); test_search();
    printf("Частина 4:\n"); test_text();
    printf("Усі тести пройдено успішно.\n");
    return 0;
}
