#include "unity_fixture.h"
#include "linc_tools.h"

TEST_GROUP(trie);

/************ Fixture ************/

Trie *tr;
trie_status trie_stat;
int *data;

TEST_SETUP(trie) {
    tr = trie_create();
}

TEST_TEAR_DOWN(trie) {
    trie_destroy(tr);
}

/************ Tests ************/

TEST(trie, simple_exact_match) {
    trie_add(tr, "abc", &(int){1}, sizeof(int));
    data = trie_lookup(tr, "abc", &trie_stat);

    TEST_ASSERT_EQUAL_INT(TRIE_MATCH, trie_stat);
    TEST_ASSERT_EQUAL_INT(1, *data);
}

TEST(trie, lookup_with_no_match) {
    trie_add(tr, "abc", &(int){1}, sizeof(int));
    data = trie_lookup(tr, "def", &trie_stat);

    TEST_ASSERT_EQUAL_INT(TRIE_NONE, trie_stat);
    TEST_ASSERT_NULL(data);
}

TEST(trie, lookup_with_wait_match) {
    trie_add(tr, "abcdef", &(int){1}, sizeof(int));
    data = trie_lookup(tr, "abc", &trie_stat);

    TEST_ASSERT_EQUAL_INT(TRIE_WAIT, trie_stat);
    TEST_ASSERT_NULL(data);
}

TEST(trie, lookup_with_ambiguous_match) {
    trie_add(tr, "abc", &(int){1}, sizeof(int));
    trie_add(tr, "abcdef", &(int){2}, sizeof(int));
    data = trie_lookup(tr, "abc", &trie_stat);

    TEST_ASSERT_EQUAL_INT(TRIE_AMBIG, trie_stat);
    TEST_ASSERT_EQUAL_INT(1, *data);
}

TEST(trie, add_and_lookup_empty_string) {
    trie_add(tr, "", &(int){1}, sizeof(int));
    data = trie_lookup(tr, "", &trie_stat);

    TEST_ASSERT_EQUAL_INT(TRIE_MATCH, trie_stat);
    TEST_ASSERT_EQUAL_INT(1, *data);

    trie_add(tr, "x", &(int){2}, sizeof(int));
    data = trie_lookup(tr, "x", &trie_stat);

    TEST_ASSERT_EQUAL_INT(TRIE_MATCH, trie_stat);
    TEST_ASSERT_EQUAL_INT(2, *data);
}

TEST(trie, lookup_empty_string_on_empty_trie) {
    trie_lookup(tr, "", &trie_stat);

    TEST_ASSERT_EQUAL_INT(TRIE_NONE, trie_stat);
}

/************ Test runner ************/

TEST_GROUP_RUNNER(trie) {
    RUN_TEST_CASE(trie, simple_exact_match);
    RUN_TEST_CASE(trie, lookup_with_no_match);
    RUN_TEST_CASE(trie, lookup_with_wait_match);
    RUN_TEST_CASE(trie, lookup_with_ambiguous_match);
    RUN_TEST_CASE(trie, add_and_lookup_empty_string);
    RUN_TEST_CASE(trie, lookup_empty_string_on_empty_trie);
}
