#include <check.h>
#include <stdlib.h>

#include "../src/list.h"

START_TEST(list_creates_one)
        struct node *list = init_node();
        add_node(list,1);
        ck_assert_int_eq(count_elems(list), 1);
        free_list(list);
END_TEST

START_TEST(list_nth_values_consistent)
        struct node *list = init_node();
        add_node(list, 1);
        add_node(list, 2);
        add_node(list, 3);
        ck_assert_int_eq(nth_elem(list,1)->data, 1);
        ck_assert_int_eq(nth_elem(list,2)->data, 2);
        ck_assert_int_eq(nth_elem(list,3)->data, 3);
        free_list(list);
END_TEST
        

Suite * list_suite(void)
{
        Suite *s;
        TCase *tc_core;

        s = suite_create("list suite");

        tc_core = tcase_create("Core");

        tcase_add_test(tc_core, list_creates_one);
        tcase_add_test(tc_core, list_nth_values_consistent);

        suite_add_tcase(s, tc_core);

        return s;
}

int main(void)
{
        int number_failed;
        Suite *s;
        SRunner *sr;

        s = list_suite();
        sr = srunner_create(s);

        srunner_run_all(sr, CK_NORMAL);
        number_failed = srunner_ntests_failed(sr);
        srunner_free(sr);
        return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
