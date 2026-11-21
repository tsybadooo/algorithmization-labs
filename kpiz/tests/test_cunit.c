/**
 * @file test_cunit.c
 * @brief КПІЗ, частина 6. Модульні тести функцій частин 3-4
 *        з використанням фреймворку CUnit (режим Basic, детальний вивід).
 */
#include <string.h>
#include <CUnit/Basic.h>
#include "../part3/matrix.h"
#include "../part3/sort.h"
#include "../part3/search.h"
#include "../part4/text.h"
#include "../common/console.h"

/* ----------------------------- частина 3 ----------------------------- */

static void test_matrix_create_free(void)
{
    Matrix m;
    CU_ASSERT_TRUE(matrix_create(&m, 3, 4));
    CU_ASSERT_PTR_NOT_NULL(m.data);
    CU_ASSERT_EQUAL(m.rows, 3);
    CU_ASSERT_EQUAL(m.cols, 4);
    CU_ASSERT_EQUAL(m.data[2][3], 0);           /* calloc - нулі */
    matrix_free(&m);
    CU_ASSERT_PTR_NULL(m.data);
    matrix_free(&m);                            /* повторне звільнення безпечне */
    CU_ASSERT_FALSE(matrix_create(&m, 3, 0));
}

static void test_is_local_min_corners_and_center(void)
{
    Matrix m;
    const int v[] = {1, 5, 0,
                     5, 2, 5,
                     3, 5, 4};
    matrix_create(&m, 3, 3);
    for (size_t i = 0; i < 9; i++) m.data[i / 3][i % 3] = v[i];
    CU_ASSERT_TRUE(is_local_min(&m, 0, 0));     /* кут, 3 сусіди більші */
    CU_ASSERT_TRUE(is_local_min(&m, 0, 2));     /* інший кут */
    CU_ASSERT_FALSE(is_local_min(&m, 1, 1));    /* центр: сусід 0 і 1 менші */
    CU_ASSERT_FALSE(is_local_min(&m, 2, 2));    /* 4 > 2 по діагоналі */
    CU_ASSERT_FALSE(is_local_min(&m, 2, 0));    /* 3: діагональний сусід 2 менший */
    CU_ASSERT_EQUAL(count_local_minima(&m, NULL, 0), 2);   /* лише 1 і 0 */
    matrix_free(&m);
}

static void test_last_column_roundtrip(void)
{
    Matrix m;
    int col[3];
    matrix_create(&m, 3, 2);
    m.data[0][1] = 9; m.data[1][1] = -1; m.data[2][1] = 4;
    CU_ASSERT_TRUE(matrix_get_last_col(&m, col));
    quick_sort(col, 3, NULL);
    matrix_set_last_col(&m, col);
    CU_ASSERT_EQUAL(m.data[0][1], -1);
    CU_ASSERT_EQUAL(m.data[1][1], 4);
    CU_ASSERT_EQUAL(m.data[2][1], 9);
    matrix_free(&m);
}

static void test_sort_function_pointer(void)
{
    sort_fn algs[] = {bubble_sort, quick_sort};
    for (int a = 0; a < 2; a++) {
        int arr[] = {3, 3, -1, 10, 0, -7, 3};
        SortStats st = {0, 0};
        algs[a](arr, 7, &st);
        CU_ASSERT_TRUE(is_sorted(arr, 7));
        CU_ASSERT_TRUE(st.comparisons > 0);
    }
}

static void test_quick_vs_bubble_counts(void)
{
    enum { N = 1000 };
    static int a[N], b[N];
    unsigned int seed = 12345u;
    for (int i = 0; i < N; i++) {                /* детермінований псевдовипадковий масив */
        seed = seed * 1103515245u + 12345u;
        a[i] = b[i] = (int)((seed >> 16) % 101) - 50;
    }
    SortStats sb = {0, 0}, sq = {0, 0};
    bubble_sort(a, N, &sb);
    quick_sort(b, N, &sq);
    CU_ASSERT_TRUE(is_sorted(a, N) && is_sorted(b, N));
    CU_ASSERT_TRUE(memcmp(a, b, sizeof a) == 0);
    CU_ASSERT_TRUE(sb.comparisons <= (unsigned long long)N * (N - 1) / 2);
    CU_ASSERT_TRUE(sq.comparisons * 10 < sb.comparisons);   /* O(n log n) << O(n^2) */
}

static void test_binary_search_all_positions(void)
{
    int a[50];
    unsigned long c;
    for (int i = 0; i < 50; i++) a[i] = i * 2;
    for (int i = 0; i < 50; i++) {
        CU_ASSERT_EQUAL(binary_search(a, 50, i * 2, &c), i);
        CU_ASSERT_TRUE(c <= 6);                  /* ceil(log2(51)) = 6 */
        CU_ASSERT_EQUAL(binary_search(a, 50, i * 2 + 1, &c), -1);
    }
}

/* ----------------------------- частина 4 ----------------------------- */

static void test_utf8_roundtrip(void)
{
    const char *s = "Ґанок їжака";
    char out[64], *p = out;
    for (const char *q = s; *q != '\0'; ) {
        uint32_t cp;
        q += utf8_decode(q, &cp);
        p += utf8_encode(cp, p);
    }
    *p = '\0';
    CU_ASSERT_STRING_EQUAL(out, s);
    CU_ASSERT_EQUAL(utf8_strlen(s), 11);
}

static void test_anagram_groups_mixed(void)
{
    StrList w, g;
    strlist_init(&w);
    strlist_init(&g);
    split_words("Listen silent ENLIST, tinsel; кіт тік; сон ніс", &w);
    CU_ASSERT_EQUAL(w.count, 8);
    CU_ASSERT_TRUE(find_anagram_groups(&w, &g));
    CU_ASSERT_EQUAL(g.count, 2);
    if (g.count == 2) {
        CU_ASSERT_STRING_EQUAL(g.items[0], "enlist listen silent tinsel");
        CU_ASSERT_STRING_EQUAL(g.items[1], "кіт тік");
    }
    strlist_free(&w);
    strlist_free(&g);
    CU_ASSERT_PTR_NULL(w.items);
}

static void test_split_no_words(void)
{
    StrList w;
    strlist_init(&w);
    CU_ASSERT_EQUAL(split_words("  ... — !!! ,,, ", &w), 0);
    CU_ASSERT_EQUAL(w.count, 0);
    strlist_free(&w);
}

static void test_is_anagram_tdd(void)
{
    CU_ASSERT_TRUE(is_anagram("Кабан", "банка"));
    CU_ASSERT_FALSE(is_anagram("кабан", "кабан"));     /* те саме слово - не анаграма */
    CU_ASSERT_FALSE(is_anagram("рамку", "мурку"));
    CU_ASSERT_FALSE(is_anagram("", ""));
}

int main(void)
{
    console_utf8();
    if (CU_initialize_registry() != CUE_SUCCESS) return (int)CU_get_error();

    CU_pSuite s3 = CU_add_suite("Частина 3: матриця, сортування, пошук", NULL, NULL);
    CU_pSuite s4 = CU_add_suite("Частина 4: опрацювання тексту", NULL, NULL);
    if (s3 == NULL || s4 == NULL) { CU_cleanup_registry(); return (int)CU_get_error(); }

    CU_add_test(s3, "створення/звільнення матриці", test_matrix_create_free);
    CU_add_test(s3, "локальні мінімуми: кути та центр", test_is_local_min_corners_and_center);
    CU_add_test(s3, "останній стовпець: читання/запис", test_last_column_roundtrip);
    CU_add_test(s3, "сортування через покажчик на функцію", test_sort_function_pointer);
    CU_add_test(s3, "кількість порівнянь: швидке << бульбашка", test_quick_vs_bubble_counts);
    CU_add_test(s3, "бінарний пошук усіх позицій", test_binary_search_all_positions);
    CU_add_test(s4, "UTF-8: декодування/кодування", test_utf8_roundtrip);
    CU_add_test(s4, "групи анаграм: латиниця і кирилиця", test_anagram_groups_mixed);
    CU_add_test(s4, "рядок без слів", test_split_no_words);
    CU_add_test(s4, "is_anagram (розроблено за TDD)", test_is_anagram_tdd);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    unsigned int failed = CU_get_number_of_failures();
    CU_cleanup_registry();
    return failed == 0 ? 0 : 1;
}
