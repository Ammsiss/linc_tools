#define _GNU_SOURCE

#include "unity_fixture.h"
#include "partty.h"
#include "partty_internals.h"

TEST_GROUP(partty);

/************ Fixture ************/

Partty *pt;

TEST_SETUP(partty) {
    pt = partty_create();
    TEST_ASSERT_NOT_NULL(pt);
}

TEST_TEAR_DOWN(partty) {
    partty_destroy(pt);
}

/************ Tests ************/

TEST(partty, feed_bytes) {
    TEST_ASSERT_EQUAL_INT(3, partty_feed_bytes(pt, "abc", 3));
    TEST_ASSERT_EQUAL_STRING_LEN("abc", &pt->b.data[pt->b.begin], 3);
}

TEST(partty, resolve_ascii_event) {
    TEST_ASSERT_EQUAL_INT(1, partty_feed_bytes(pt, "x", 1));

    partty_event pev = partty_resolve_event(pt);
    TEST_ASSERT_EQUAL_INT(PARTTY_UNICODE, pev.type);
    TEST_ASSERT_EQUAL_UINT32('x', pev.codepoint);
    TEST_ASSERT_EQUAL_UINT8('x', pev.utf8[0]);
}

TEST(partty, resolve_two_ascii_events) {
    TEST_ASSERT_EQUAL_INT(2, partty_feed_bytes(pt, "xy", 2));

    partty_event pev = partty_resolve_event(pt);
    TEST_ASSERT_EQUAL_INT(PARTTY_UNICODE, pev.type);
    TEST_ASSERT_EQUAL_UINT32('x', pev.codepoint);
    TEST_ASSERT_EQUAL_UINT8('x', pev.utf8[0]);

    pev = partty_resolve_event(pt);
    TEST_ASSERT_EQUAL_INT(PARTTY_UNICODE, pev.type);
    TEST_ASSERT_EQUAL_UINT32('y', pev.codepoint);
    TEST_ASSERT_EQUAL_UINT8('y', pev.utf8[0]);
}

TEST(partty, resolve_four_byte_unicode_event) {
    TEST_ASSERT_EQUAL_INT(4, partty_feed_bytes(pt, "😀", 4));

    partty_event pev = partty_resolve_event(pt);
    TEST_ASSERT_EQUAL_INT(PARTTY_UNICODE, pev.type);
    TEST_ASSERT_EQUAL_UINT32(0x1F600, pev.codepoint);
    TEST_ASSERT_EQUAL_UINT8(0xF0, pev.utf8[0]);
    TEST_ASSERT_EQUAL_UINT8(0x9F, pev.utf8[1]);
    TEST_ASSERT_EQUAL_UINT8(0x98, pev.utf8[2]);
    TEST_ASSERT_EQUAL_UINT8(0x80, pev.utf8[3]);
}

TEST(partty, resolve_two_byte_unicode_event) {
    TEST_ASSERT_EQUAL_INT(2, partty_feed_bytes(pt, "é", 2));

    partty_event pev = partty_resolve_event(pt);
    TEST_ASSERT_EQUAL_INT(PARTTY_UNICODE, pev.type);
    TEST_ASSERT_EQUAL_UINT32(0x00E9, pev.codepoint);
    TEST_ASSERT_EQUAL_UINT8(0xC3, pev.utf8[0]);
    TEST_ASSERT_EQUAL_UINT8(0xA9, pev.utf8[1]);
}

TEST(partty, resolve_two_unicode_events) {
    TEST_ASSERT_EQUAL_INT(4, partty_feed_bytes(pt, "éΩ", 4));

    partty_event pev = partty_resolve_event(pt);
    TEST_ASSERT_EQUAL_INT(PARTTY_UNICODE, pev.type);
    TEST_ASSERT_EQUAL_UINT32(0x00E9, pev.codepoint);
    TEST_ASSERT_EQUAL_UINT8(0xC3, pev.utf8[0]);
    TEST_ASSERT_EQUAL_UINT8(0xA9, pev.utf8[1]);

    pev = partty_resolve_event(pt);
    TEST_ASSERT_EQUAL_INT(PARTTY_UNICODE, pev.type);
    TEST_ASSERT_EQUAL_UINT32(0x03A9, pev.codepoint);
    TEST_ASSERT_EQUAL_UINT8(0xCE, pev.utf8[0]);
    TEST_ASSERT_EQUAL_UINT8(0xA9, pev.utf8[1]);
}

TEST(partty, partial_unicode_then_feed_rest_and_resolve) {
    char b1[2] = { 0xCE, '\0' };
    char b2[2] = { 0xA9, '\0' };

    TEST_ASSERT_EQUAL_INT(1, partty_feed_bytes(pt, b1, 1));

    partty_event pev = partty_resolve_event(pt);
    TEST_ASSERT_EQUAL_INT(PARTTY_AMBIG, pev.type);

    TEST_ASSERT_EQUAL_INT(1, partty_feed_bytes(pt, b2, 1));

    pev = partty_resolve_event(pt);
    TEST_ASSERT_EQUAL_INT(PARTTY_UNICODE, pev.type);
    TEST_ASSERT_EQUAL_UINT32(0x03A9, pev.codepoint);
    TEST_ASSERT_EQUAL_UINT8(0xCE, pev.utf8[0]);
    TEST_ASSERT_EQUAL_UINT8(0xA9, pev.utf8[1]);
}

/************ Test runner ************/

TEST_GROUP_RUNNER(partty) {
    RUN_TEST_CASE(partty, feed_bytes);
    RUN_TEST_CASE(partty, resolve_ascii_event);
    RUN_TEST_CASE(partty, resolve_two_ascii_events);
    RUN_TEST_CASE(partty, resolve_four_byte_unicode_event);
    RUN_TEST_CASE(partty, resolve_two_byte_unicode_event);
    RUN_TEST_CASE(partty, resolve_two_unicode_events);
    RUN_TEST_CASE(partty, partial_unicode_then_feed_rest_and_resolve);
}
