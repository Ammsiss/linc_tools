#define _GNU_SOURCE

#include "unity_fixture.h"
#include "trie.h"

TEST_GROUP(trie);

/************ Fixture ************/

TEST_SETUP(trie) {
}

TEST_TEAR_DOWN(trie) {
}

/************ Tests ************/

TEST(trie, init_test) {
    TEST_FAIL();
}

/************ Test runner ************/

TEST_GROUP_RUNNER(trie) {
    RUN_TEST_CASE(trie, init_test);
}
