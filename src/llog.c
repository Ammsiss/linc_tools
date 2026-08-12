#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

#include "llog.h"

#define BUF_SIZE 8192

static int lfd = NO_LOG_FD;
static char lbuf[BUF_SIZE] = "Nothing logged";
static int drops = 0;

PFFORMAT(1, 2)
static void lfd_printf(const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);

    int len = vsnprintf(lbuf, BUF_SIZE, fmt, va);

    va_end(va);

    if (len < 0 || len >= BUF_SIZE || write(lfd, lbuf, len) != len)
        ++drops;
}

static const char *lvlstr(llog_lvl level) {
    switch (level) {
    case LLOG_INFO: return "INFO";
    case LLOG_WARN: return "WARN";
    case LLOG_ERR: return "ERR";
    default: return "";
    }
}

int llog_get_log_fd(void) {
    return lfd;
}

const char *llog_last_log(void) {
    return lbuf;
}

void llog_set_fd(int log_fd) {
    lfd = log_fd;
}

void llog_reset(void) {
    lfd_printf("LLog: drops: %d\n", drops);

    lfd = NO_LOG_FD;
    drops = 0;
}

void llog_log(llog_lvl lvl, const char *file, int line, const char *fmt, ...) {
    int saved_errno = errno;
    int len = 0;
    va_list va;

    int n = snprintf(lbuf, BUF_SIZE, "%s %s:%d: ", lvlstr(lvl), file, line);

    if (n < 0 || n >= BUF_SIZE)
        goto fail;

    len += n;

    va_start(va, fmt);
    n = vsnprintf(lbuf + len, BUF_SIZE - n, fmt, va);
    va_end(va);

    if (n < 0 || n >= BUF_SIZE - len - 1)
        goto fail;

    len += n;

    lbuf[len++] = '\n';
    lbuf[len] = '\0';

    if (write(lfd, lbuf, len) != len)
        goto fail;

    return;

fail:
    ++drops;
    errno = saved_errno;
}
