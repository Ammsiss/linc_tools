#include "unity_fixture.h"
#include "linc_tools.h"

/* TODO: add test for n versions of str funcs */

TEST_GROUP(dstr);

/************ Shared utils ************/

static dstr s_str = {0};

static void validate_str(size_t exp_len, size_t exp_size, size_t exp_cap) {
    TEST_ASSERT_EQUAL_size_t(exp_len, s_str.len);
    TEST_ASSERT_EQUAL_size_t(exp_size, s_str.size);
    TEST_ASSERT_EQUAL_size_t(exp_cap, s_str.cap);
}

/************ Fixture ************/

TEST_SETUP(dstr) {
    dstr_init(&s_str);
    validate_str(0, 1, 1);
}

TEST_TEAR_DOWN(dstr) {
    dstr_free(&s_str);
}

/************ Tests ************/

TEST(dstr, is_c_string_after_init) {
    TEST_ASSERT_EQUAL_CHAR('\0', s_str.c_str[0]);
}

TEST(dstr, is_c_string_after_push) {
    dstr_push(&s_str, 'a');
    TEST_ASSERT_EQUAL_STRING_LEN("a", s_str.c_str, 1);
    validate_str(1, 2, 2);

    dstr_push(&s_str, 'b');
    TEST_ASSERT_EQUAL_STRING_LEN("ab", s_str.c_str, 2);
    validate_str(2, 3, 3);

    dstr_push(&s_str, 'c');
    TEST_ASSERT_EQUAL_STRING_LEN("abc", s_str.c_str, 3);
    validate_str(3, 4, 4);
}

TEST(dstr, concat_right_after_init) {
    dstrcat(&s_str, "abc");
    TEST_ASSERT_EQUAL_STRING_LEN("abc", s_str.c_str, 3);
    validate_str(3, 4, 4);
}

TEST(dstr, concat_with_non_empty_string) {
    dstrcpy(&s_str, "abc");
    TEST_ASSERT_EQUAL_STRING_LEN("abc", s_str.c_str, 3);
    validate_str(3, 4, 4);

    dstrcat(&s_str, "def");
    TEST_ASSERT_EQUAL_STRING_LEN("abcdef", s_str.c_str, 6);
    validate_str(6, 7, 7);
}

TEST(dstr, string_formated_print) {
    int x = 1;
    float y = 1.5;
    char *s = "Equation";

    dstr_printf(&s_str, "%s: %d + %.1f = %.1f", s, x, y, x + y);
    validate_str(23, 24, 24);

    TEST_ASSERT_EQUAL_STRING("Equation: 1 + 1.5 = 2.5", s_str.c_str);
}

/************ Test runner ************/

TEST_GROUP_RUNNER(dstr) {
    RUN_TEST_CASE(dstr, is_c_string_after_init);
    RUN_TEST_CASE(dstr, is_c_string_after_push);
    RUN_TEST_CASE(dstr, concat_right_after_init);
    RUN_TEST_CASE(dstr, concat_with_non_empty_string);
    RUN_TEST_CASE(dstr, string_formated_print);
}
