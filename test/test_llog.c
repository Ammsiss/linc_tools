#include <unistd.h>
#include <fcntl.h>

#include "unity_fixture.h"
#include "linc_tools.h"

TEST_GROUP(llog);

/************ Shared utils ************/

static char log_buf[8192];

void info_sink(const llog_info *info) {
    TEST_ASSERT_EQUAL_INT(LLOG_INFO, info->log_level);
    snprintf(log_buf, 8192, "INFO %s", info->msg);
}

void err_sink(const llog_info *info) {
    TEST_ASSERT_EQUAL_INT(LLOG_ERR, info->log_level);
    snprintf(log_buf, 8192, "ERROR %s", info->msg);
}

void warn_sink(const llog_info *info) {
    TEST_ASSERT_EQUAL_INT(LLOG_WARN, info->log_level);
    snprintf(log_buf, 8192, "WARN %s", info->msg);
}

static llog info_log = { .sink = info_sink };
static llog err_log = { .sink = err_sink };
static llog warn_log = { .sink = warn_sink };

/************ Fixture ************/

TEST_SETUP(llog) {
    log_buf[0] = '\0';
}

TEST_TEAR_DOWN(llog) {
}

/************ Tests ************/

TEST(llog, simple_info_log) {
    llog_log(&info_log, LLOG_INFO, &LLOG_SITE, "hi");
    TEST_ASSERT_EQUAL_STRING("INFO hi", log_buf);
}

TEST(llog, simple_err_log) {
    llog_log(&err_log, LLOG_ERR, &LLOG_SITE, "hi");
    TEST_ASSERT_EQUAL_STRING("ERROR hi", log_buf);
}

TEST(llog, simple_warn_log) {
    llog_log(&warn_log, LLOG_WARN, &LLOG_SITE, "hi");
    TEST_ASSERT_EQUAL_STRING("WARN hi", log_buf);
}

TEST(llog, special_chars_are_normalized) {
    llog_log(&info_log, LLOG_INFO, &LLOG_SITE, "\n\t");
    TEST_ASSERT_EQUAL_STRING("INFO \\n\\t", log_buf);
}

/************ Test runner ************/

TEST_GROUP_RUNNER(llog) {
    RUN_TEST_CASE(llog, simple_info_log);
    RUN_TEST_CASE(llog, simple_err_log);
    RUN_TEST_CASE(llog, simple_info_log);
    RUN_TEST_CASE(llog, special_chars_are_normalized);
}
