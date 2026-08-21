#define _GNU_SOURCE

#include <unistd.h>
#include <fcntl.h>

#include "unity_fixture.h"
#include "llog.h"

TEST_GROUP(llog);

/************ Shared utils ************/

static char log_buf[8192];

LLOG_SINK(info_sink) {
    TEST_ASSERT_EQUAL_INT(LLOG_INFO, info->log_level);
    snprintf(log_buf, 8192, "INFO %s", info->msg);
}

LLOG_SINK(err_sink) {
    TEST_ASSERT_EQUAL_INT(LLOG_ERR, info->log_level);
    snprintf(log_buf, 8192, "ERROR %s", info->msg);
}

LLOG_SINK(warn_sink) {
    TEST_ASSERT_EQUAL_INT(LLOG_WARN, info->log_level);
    snprintf(log_buf, 8192, "WARN %s", info->msg);
}

/************ Fixture ************/

TEST_SETUP(llog) {
    log_buf[0] = '\0';
}

TEST_TEAR_DOWN(llog) {
    llog_reset();
}

/************ Tests ************/

TEST(llog, simple_info_log) {
    llog_set_sink(info_sink);

    LOG_INFO("hi");
    TEST_ASSERT_EQUAL_STRING("INFO hi", log_buf);
}

TEST(llog, simple_err_log) {
    llog_set_sink(err_sink);

    LOG_ERR("hi");
    TEST_ASSERT_EQUAL_STRING("ERROR hi", log_buf);
}

TEST(llog, simple_warn_log) {
    llog_set_sink(warn_sink);

    LOG_WARN("hi");
    TEST_ASSERT_EQUAL_STRING("WARN hi", log_buf);
}

TEST(llog, special_chars_are_normalized) {
    llog_set_sink(info_sink);

    LOG_INFO("\n\t");
    TEST_ASSERT_EQUAL_STRING("INFO \\n\\t", log_buf);
}

/************ Test runner ************/

TEST_GROUP_RUNNER(llog) {
    RUN_TEST_CASE(llog, simple_info_log);
    RUN_TEST_CASE(llog, simple_err_log);
    RUN_TEST_CASE(llog, simple_info_log);
    RUN_TEST_CASE(llog, special_chars_are_normalized);
}
