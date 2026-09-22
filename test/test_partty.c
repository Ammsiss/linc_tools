#define _GNU_SOURCE

#include <locale.h>
#include <uchar.h>

#include "unity_fixture.h"
#include "partty.h"
#include "partty_internals.h"
#include "unibilium.h"

TEST_GROUP(partty);

/************ Fixture ************/

Partty *pt;
partty_event pev;
unibi_term *term;

void feed_bytes(const char *s) {
    size_t len = strlen(s);
    TEST_ASSERT_EQUAL_INT(len, partty_feed_bytes(pt, s, len));
}

void feed_bytes_n(const char *s, size_t n) {
    TEST_ASSERT_EQUAL_INT(n, partty_feed_bytes(pt, s, n));
}

void feed_sequence(enum unibi_string uc) {
    const char *s = unibi_get_str(term, uc);
    size_t len = strlen(s);
    TEST_ASSERT_EQUAL_INT(len, partty_feed_bytes(pt, s, len));
}

void verify_unicode_event(const char *s) {
    size_t len = strlen(s);
    partty_event pev = partty_resolve_event(pt);

    if (len == 1) {
        TEST_ASSERT_EQUAL_INT(PARTTY_UNICODE, pev.type);
        TEST_ASSERT_EQUAL_UINT32((uint32_t)s[0], pev.codepoint);
        TEST_ASSERT_EQUAL_UINT8(s[0], pev.utf8[0]);
        return;
    }

    uint32_t cp;
    if (mbrtoc32(&cp, s, len, NULL) <= 0)
        TEST_FAIL_MESSAGE("mbrtoc32 decode fail");

    TEST_ASSERT_EQUAL_INT(PARTTY_UNICODE, pev.type);
    TEST_ASSERT_EQUAL_UINT32(cp, pev.codepoint);

    for (size_t i = 0; i < len; ++i)
        TEST_ASSERT_EQUAL_UINT8(s[i], pev.utf8[i]);
}

void verify_key_event(partty_key key) {
    pev = partty_resolve_event(pt);
    TEST_ASSERT_EQUAL_INT(PARTTY_KEY, pev.type);
    TEST_ASSERT_EQUAL_INT(key, pev.key);
}

TEST_SETUP(partty) {
    pt = partty_create();
    TEST_ASSERT_NOT_NULL(pt);

    term = unibi_from_env();
    TEST_ASSERT_NOT_NULL(term);
}

TEST_TEAR_DOWN(partty) {
    partty_destroy(pt);
    unibi_destroy(term);
}

/************ Tests ************/

TEST(partty, feed_bytes) {
    feed_bytes("abc");
    TEST_ASSERT_EQUAL_STRING_LEN("abc", &pt->b.data[pt->b.begin], 3);
}

TEST(partty, resolve_ascii_event) {
    feed_bytes("x");
    verify_unicode_event("x");
}

TEST(partty, resolve_two_ascii_events) {
    feed_bytes("xy");
    verify_unicode_event("x");
    verify_unicode_event("y");
}

TEST(partty, resolve_four_byte_unicode_event) {
    feed_bytes("😀");
    verify_unicode_event("😀");
}

TEST(partty, resolve_two_byte_unicode_event) {
    feed_bytes("é");
    verify_unicode_event("é");
}

TEST(partty, resolve_two_unicode_events) {
    feed_bytes("éΩ");
    verify_unicode_event("é");
    verify_unicode_event("Ω");
}

TEST(partty, partial_unicode_then_feed_rest_and_resolve) {
    char b1[2] = { 0xCE, '\0' };
    char b2[2] = { 0xA9, '\0' };

    feed_bytes(b1);

    pev = partty_resolve_event(pt);
    TEST_ASSERT_EQUAL_INT(PARTTY_AMBIG, pev.type);

    feed_bytes(b2);
    verify_unicode_event("Ω");
}

TEST(partty, arrow_key_escape_sequence) {
    feed_sequence(unibi_key_left);
    verify_key_event(PARTTY_LEFT);
}

TEST(partty, prefix_match_then_no_match) {
    feed_bytes("\x1bx");
    verify_unicode_event("\x1b");
    verify_unicode_event("x");
}

TEST(partty, prefix_match_then_full_match) {
    const char *unibi_str = unibi_get_str(term, unibi_key_left);

    feed_bytes_n(unibi_str, 1);

    pev = partty_resolve_event(pt);
    TEST_ASSERT_EQUAL_INT(PARTTY_AMBIG, pev.type);

    feed_bytes(unibi_str + 1);
    verify_key_event(PARTTY_LEFT);
}

TEST(partty, unicode_and_escape_sequence) {
    feed_bytes("Ω");
    feed_sequence(unibi_key_left);
    feed_bytes("x");

    verify_unicode_event("Ω");
    verify_key_event(PARTTY_LEFT);
    verify_unicode_event("x");
}

/************ Test runner ************/

TEST_GROUP_RUNNER(partty) {
    setlocale(LC_CTYPE, "");

    RUN_TEST_CASE(partty, feed_bytes);
    RUN_TEST_CASE(partty, resolve_ascii_event);
    RUN_TEST_CASE(partty, resolve_two_ascii_events);
    RUN_TEST_CASE(partty, resolve_four_byte_unicode_event);
    RUN_TEST_CASE(partty, resolve_two_byte_unicode_event);
    RUN_TEST_CASE(partty, resolve_two_unicode_events);
    RUN_TEST_CASE(partty, partial_unicode_then_feed_rest_and_resolve);
    RUN_TEST_CASE(partty, arrow_key_escape_sequence);
    RUN_TEST_CASE(partty, prefix_match_then_no_match);
    RUN_TEST_CASE(partty, prefix_match_then_full_match);
    RUN_TEST_CASE(partty, unicode_and_escape_sequence);
}
