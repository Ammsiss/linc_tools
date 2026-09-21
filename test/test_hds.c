#define _GNU_SOURCE

#include "unity_fixture.h"
#include "hds.h"
#include "hda.h"

TEST_GROUP(hds);

/************ Fixture ************/

char *hds;

TEST_SETUP(hds) {
    hds = nullptr;
}

TEST_TEAR_DOWN(hds) {
    hds_free(hds);
}

/************ Tests ************/

/* use lower-level hda_size function to assert malloc size */

TEST(hds, append_strings) {
    hds_append(hds, "hello, ");
    TEST_ASSERT_EQUAL_STRING("hello, ", hds);
    TEST_ASSERT_EQUAL_size_t(sizeof("hello, "), hda_size(hds));

    hds_append(hds, "world!");
    TEST_ASSERT_EQUAL_STRING("hello, world!", hds);
    TEST_ASSERT_EQUAL_size_t(sizeof("hello, world!"), hda_size(hds));
}

TEST(hds, copy_and_append) {
    hds_copy(hds, "hello, ");
    TEST_ASSERT_EQUAL_STRING("hello, ", hds);
    TEST_ASSERT_EQUAL_size_t(sizeof("hello, "), hda_size(hds));

    hds_append(hds, "wonderful ");
    TEST_ASSERT_EQUAL_STRING("hello, wonderful ", hds);
    TEST_ASSERT_EQUAL_size_t(sizeof("hello, wonderful "), hda_size(hds));

    hds_append(hds, "world!");
    TEST_ASSERT_EQUAL_STRING("hello, wonderful world!", hds);
    TEST_ASSERT_EQUAL_size_t(sizeof("hello, wonderful world!"), hda_size(hds));

    hds_copy(hds, "");
    TEST_ASSERT_EQUAL_STRING("", hds);
    TEST_ASSERT_EQUAL_size_t(sizeof(""), hda_size(hds));
}

TEST(hds, append_n_chars) {
    hds_append_n(hds, "abcdef", 3);
    TEST_ASSERT_EQUAL_STRING("abc", hds);
    TEST_ASSERT_EQUAL_size_t(sizeof("abc"), hda_size(hds));

    hds_append_n(hds, "defghi", 3);
    TEST_ASSERT_EQUAL_STRING("abcdef", hds);
    TEST_ASSERT_EQUAL_size_t(sizeof("abcdef"), hda_size(hds));
}

TEST(hds, copy_n_chars) {
    hds_copy_n(hds, "abcdef", 3);
    TEST_ASSERT_EQUAL_STRING("abc", hds);
    TEST_ASSERT_EQUAL_size_t(sizeof("abc"), hda_size(hds));

    hds_copy_n(hds, "defghi", 3);
    TEST_ASSERT_EQUAL_STRING("def", hds);
    TEST_ASSERT_EQUAL_size_t(sizeof("def"), hda_size(hds));
}

TEST(hds, append_fmt) {
    int x = 3;
    const char *s = "hi";

    hds_append_fmt(hds, "int: %d string: %s", x, s);
    TEST_ASSERT_EQUAL_STRING("int: 3 string: hi", hds);
    TEST_ASSERT_EQUAL_size_t(sizeof("int: 3 string: hi"), hda_size(hds));
}

TEST(hds, copy_fmt) {
    hds_copy(hds, "x");

    int x = 3;
    const char *s = "hi";

    hds_copy_fmt(hds, "int: %d string: %s", x, s);
    TEST_ASSERT_EQUAL_STRING("int: 3 string: hi", hds);
    TEST_ASSERT_EQUAL_size_t(sizeof("int: 3 string: hi"), hda_size(hds));
}

TEST(hds, append_to_empty_string_that_was_non_empty) {
    hds_append(hds, "abc");
    hds_copy(hds, "");
    hds_append(hds, "def");

    TEST_ASSERT_EQUAL_STRING("def", hds);
    TEST_ASSERT_EQUAL_size_t(sizeof("def"), hda_size(hds));
}

TEST(hds, self_append) {
    hds_append(hds, "abc");
    hds_append(hds, hds);

    TEST_ASSERT_EQUAL_STRING("abcabc", hds);
    TEST_ASSERT_EQUAL_size_t(sizeof("abcabc"), hda_size(hds));
}

/************ Test runner ************/

TEST_GROUP_RUNNER(hds) {
    RUN_TEST_CASE(hds, append_strings);
    RUN_TEST_CASE(hds, copy_and_append);
    RUN_TEST_CASE(hds, append_n_chars);
    RUN_TEST_CASE(hds, copy_n_chars);
    RUN_TEST_CASE(hds, append_fmt);
    RUN_TEST_CASE(hds, copy_fmt);
    RUN_TEST_CASE(hds, append_to_empty_string_that_was_non_empty);
    RUN_TEST_CASE(hds, self_append);
}
