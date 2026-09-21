#include "unity_fixture.h"
#include "kval.h"

TEST_GROUP(kval);

/************ Fixture ************/

kval *kv;

TEST_SETUP(kval) {
}

TEST_TEAR_DOWN(kval) {
    kv_free(kv);
}

/************ Tests ************/

TEST(kval, use_str_key_value_pair) {
    kv = kv_create(100, sizeof(int));
    kv_add(kv, "abc", 11);

    int *val = kv_lookup(kv, "abc");

    TEST_ASSERT_NOT_NULL(val);
    TEST_ASSERT_EQUAL_INT(*val, 11);

    kv_delete(kv, "abc");

    val = kv_lookup(kv, "abc");
    TEST_ASSERT_NULL(val);
}

TEST(kval, use_int_key_value_pair) {
    kv = kv_create(sizeof(int), 100);
    kv_add(kv, 11, "abc");

    char *val = kv_lookup(kv, 11);
    TEST_ASSERT_NOT_NULL(val);
    TEST_ASSERT_EQUAL_STRING(val, "abc");

    kv_delete(kv, 11);

    val = kv_lookup(kv, 11);
    TEST_ASSERT_NULL(val);
}

TEST(kval, iteration) {
    kv = kv_create(sizeof(int), sizeof(int));

    kv_add(kv, 1, 2);
    kv_add(kv, 2, 4);
    kv_add(kv, 3, 6);

    int sum = 0;

    kvp *pair;
    kv_foreach(kv, pair)
        sum += *(int *)pair->v;

    TEST_ASSERT_EQUAL_INT(12, sum);
}

TEST(kval, add_key_that_already_exists) {
    kv = kv_create(sizeof(int), sizeof(int));

    TEST_ASSERT_EQUAL_INT(0, kv_add(kv, 1, 2));
    TEST_ASSERT_EQUAL_INT(-1, kv_add(kv, 1, 4));
}

TEST(kval, lookup_non_existant_key) {
    kv = kv_create(sizeof(int), sizeof(int));

    TEST_ASSERT_NULL(kv_lookup(kv, 1));
}

TEST(kval, delete_non_existant_key) {
    kv = kv_create(sizeof(int), sizeof(int));
    kv_add(kv, 1, 1);

    TEST_ASSERT_EQUAL_size_t(1, hdr_size(kv->pairs));
    TEST_ASSERT_EQUAL_INT(0, kv_delete(kv, 2));
    TEST_ASSERT_EQUAL_size_t(1, hdr_size(kv->pairs));
}

TEST(kval, set_existing_key_to_new_value) {
    kv = kv_create(sizeof(int), sizeof(int));
    kv_add(kv, 1, 1);
    kv_set(kv, 1, 2);

    TEST_ASSERT_EQUAL_size_t(1, hdr_size(kv->pairs));
    TEST_ASSERT_EQUAL_INT(2, *(int *)kv_lookup(kv, 1));
}

/************ Test runner ************/

TEST_GROUP_RUNNER(kval) {
    RUN_TEST_CASE(kval, use_str_key_value_pair);
    RUN_TEST_CASE(kval, use_int_key_value_pair);
    RUN_TEST_CASE(kval, iteration);
    RUN_TEST_CASE(kval, add_key_that_already_exists);
    RUN_TEST_CASE(kval, lookup_non_existant_key);
    RUN_TEST_CASE(kval, delete_non_existant_key);
    RUN_TEST_CASE(kval, set_existing_key_to_new_value);
}
