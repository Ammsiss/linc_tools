#include "unity_fixture.h"
#include "darr.h"

TEST_GROUP(darr);

/************ Shared utils ************/

static da_int arr;

static void validate_array(da_int *arr, size_t size, size_t cap) {
    TEST_ASSERT_NOT_NULL(arr);
    TEST_ASSERT_EQUAL_size_t(size, arr->size);
    TEST_ASSERT_EQUAL_size_t(cap, arr->cap);
}

/************ Fixture ************/

TEST_SETUP(darr) {
    int rv = da_init(&arr);
    TEST_ASSERT_EQUAL_INT(0, rv);
    validate_array(&arr, 0, 0);
}

TEST_TEAR_DOWN(darr) {
    da_free(&arr);
}

/************ Tests ************/

TEST(darr, reserve_min_lte_cap_is_no_op) {
    int rv = da_reserve(&arr, 0);
    TEST_ASSERT_EQUAL_INT(0, rv);

    validate_array(&arr, 0, 0);
}

TEST(darr, reserve_empty_array) {
    int rv = da_reserve(&arr, 10);
    TEST_ASSERT_EQUAL_INT(0, rv);
    validate_array(&arr, 0, 10);
}

TEST(darr, reserve_non_empty_array) {
    /* initial allocation ... realloc(NULL, size) */
    int rv = da_reserve(&arr, 10);
    TEST_ASSERT_EQUAL_INT(0, rv);
    validate_array(&arr, 0, 10);
    /* actual realloc call ... realloc (data, size) */
    rv = da_reserve(&arr, 20);
    TEST_ASSERT_EQUAL_INT(0, rv);
    validate_array(&arr, 0, 20);
}

TEST(darr, reserve_realloc_preserves_data) {
    int *p = da_push(&arr);
    TEST_ASSERT_NOT_NULL(p);
    validate_array(&arr, 1, 1);

    arr.data[0] = 100;

    p = da_push(&arr);
    TEST_ASSERT_NOT_NULL(p);
    validate_array(&arr, 2, 2);

    TEST_ASSERT_EQUAL_INT(100, arr.data[0]);
}

TEST(darr, push_returns_new_element) {
    int *p = da_push(&arr);
    TEST_ASSERT_EQUAL_PTR(&arr.data[0], p);
    validate_array(&arr, 1, 1);

    /* push should zero out new element */
    TEST_ASSERT_EQUAL_INT(0, *p);
}

TEST(darr, delete_only_element) {
    da_push(&arr);
    validate_array(&arr, 1, 1);

    da_delete(&arr, 0);
    validate_array(&arr, 0, 1);
}

TEST(darr, delete_first_element) {
    int *p1 = da_push(&arr);
    validate_array(&arr, 1, 1);
    *p1 = 1;

    int *p2 = da_push(&arr);
    validate_array(&arr, 2, 2);
    *p2 = 2;

    da_delete(&arr, 0);
    validate_array(&arr, 1, 2);

    TEST_ASSERT_EQUAL_INT(arr.data[0], 2);
}

TEST(darr, delete_last_element) {
    int *p1 = da_push(&arr);
    validate_array(&arr, 1, 1);
    *p1 = 1;

    int *p2 = da_push(&arr);
    validate_array(&arr, 2, 2);
    *p2 = 2;

    da_delete(&arr, 1);
    validate_array(&arr, 1, 2);

    TEST_ASSERT_EQUAL_INT(arr.data[0], 1);
}

TEST(darr, delete_middle_element) {
    int *p1 = da_push(&arr);
    validate_array(&arr, 1, 1);
    *p1 = 1;

    int *p2 = da_push(&arr);
    validate_array(&arr, 2, 2);
    *p2 = 2;

    int *p3 = da_push(&arr);
    validate_array(&arr, 3, 3);
    *p3 = 3;

    da_delete(&arr, 1);
    validate_array(&arr, 2, 3);

    TEST_ASSERT_EQUAL_INT(arr.data[0], 1);
    TEST_ASSERT_EQUAL_INT(arr.data[1], 3);
}

/************ Test runner ************/

TEST_GROUP_RUNNER(darr) {
    RUN_TEST_CASE(darr, reserve_min_lte_cap_is_no_op);
    RUN_TEST_CASE(darr, reserve_empty_array);
    RUN_TEST_CASE(darr, reserve_non_empty_array);
    RUN_TEST_CASE(darr, reserve_realloc_preserves_data);
    RUN_TEST_CASE(darr, push_returns_new_element);
    RUN_TEST_CASE(darr, delete_only_element);
    RUN_TEST_CASE(darr, delete_first_element);
    RUN_TEST_CASE(darr, delete_last_element);
    RUN_TEST_CASE(darr, delete_middle_element);
}
