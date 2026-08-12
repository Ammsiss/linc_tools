#define _GNU_SOURCE

#include <unistd.h>
#include <fcntl.h>

#include "unity_fixture.h"
#include "utils.h"
#include "llog.h"

TEST_GROUP(llog);

/************ Shared utils ************/

#define LBUF_SIZE 8192

static int pfd[2];
static char pbuf[LBUF_SIZE];
static const char *exp_log;

static const char *read_log(void) {
    int num_read = xread(pfd[0], pbuf, LBUF_SIZE);
    pbuf[num_read] = '\0';
    return pbuf;
}

/************ Fixture ************/

TEST_SETUP(llog) {
    xpipe(pfd);
    llog_set_fd(pfd[1]);
}

TEST_TEAR_DOWN(llog) {
    xclose(pfd[0]);
    xclose(pfd[1]);
}

/************ Tests ************/

TEST(llog, set_fd_sets_fd) {
    llog_set_fd(10);
    TEST_ASSERT_EQUAL(10, llog_get_log_fd());
}

TEST(llog, reset_sets_fd_to_sentinal) {
    llog_set_fd(10);
    llog_reset();
    TEST_ASSERT_EQUAL(NO_LOG_FD, llog_get_log_fd());
}

TEST(llog, log_info_header_correct) {
    char exp_log[LBUF_SIZE] = "INFO f:0: m\n";
    llog_log(LLOG_INFO, "f", 0, "m");
    TEST_ASSERT_EQUAL_STRING(exp_log, llog_last_log());
}

TEST(llog, log_error_header_correct) {
    char exp_log[LBUF_SIZE] = "ERR f:0: m\n";
    llog_log(LLOG_ERR, "f", 0, "m");
    TEST_ASSERT_EQUAL_STRING(exp_log, llog_last_log());
}

TEST(llog, log_writes_to_fd) {
    exp_log = "INFO f:0: m\n";
    llog_log(LLOG_INFO, "f", 0, "m");
    TEST_ASSERT_EQUAL_STRING(exp_log, read_log());
}

TEST(llog, log_buf_equal_to_fd_output) {
    char exp_log[LBUF_SIZE] = "ERR main.c:10: Malloc failure\n";
    llog_log(LLOG_ERR, "main.c", 10, "Malloc failure");
    TEST_ASSERT_EQUAL_STRING(exp_log, read_log());
}

/************ Test runner ************/

TEST_GROUP_RUNNER(llog) {
    RUN_TEST_CASE(llog, set_fd_sets_fd);
    RUN_TEST_CASE(llog, reset_sets_fd_to_sentinal);
    RUN_TEST_CASE(llog, log_info_header_correct);
    RUN_TEST_CASE(llog, log_error_header_correct);
    RUN_TEST_CASE(llog, log_writes_to_fd);
    RUN_TEST_CASE(llog, log_buf_equal_to_fd_output);
}
