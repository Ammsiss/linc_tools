#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "llog.h"
#include "dstr.h"
#include "common.h"

#define BUF_SIZE 8192

static int lfd = NO_LOG_FD;
static int drops = 0;
static char last_log[BUF_SIZE];

int llog_get_log_fd(void) {
    return lfd;
}

int llog_get_drops(void) {
    return drops;
}

const char *llog_last_log(void) {
    return last_log;
}

void llog_set_fd(int log_fd) {
    lfd = log_fd;
}

void llog_reset(void) {
    lfd = NO_LOG_FD;
    drops = 0;
}

static const char *lvlstr(llog_lvl level) {
    switch (level) {
    case LLOG_INFO: return CGREEN "INFO" CCL;
    case LLOG_WARN: return CYELLOW "WARN" CCL;
    case LLOG_ERR: return CRED "ERR" CCL;
    default: return "";
    }
}

static void convert_newlines(dstr *output_msg) {
    dstr out;
    dstr_init(&out);

    for (char *c = output_msg->c_str; *c != '\0'; ++c) {
        if (*c == '\n') {
            dstrcat(&out, "\\n");
        } else
            dstr_push(&out, *c);
    }

    dstr_push(&out, '\n');

    dstr_free(output_msg);
    dstr_init(output_msg);
    dstrcpy(output_msg, out.c_str);

    dstr_free(&out);
}

void llog_log(llog_lvl lvl, const char *file, int line, const char *fmt, ...) {
    dstr output_msg;
    int saved_errno = errno;
    va_list va;

    dstr_init(&output_msg);
    dstr_printf(&output_msg, "%s %s:%d: ", lvlstr(lvl), file, line);

    char user_msg[BUF_SIZE];

    va_start(va, fmt);
     if (vsnprintf(user_msg, BUF_SIZE, fmt, va) < 0)
         LIB_FATAL("llog: vsnprintf: output error");
    va_end(va);

    dstrcat(&output_msg, user_msg);
    convert_newlines(&output_msg);

    if (write(lfd, output_msg.c_str, output_msg.len) != (int) output_msg.len)
        goto fail;

    strncpy(last_log, output_msg.c_str, output_msg.size);

    dstr_free(&output_msg);
    errno = saved_errno;
    return;

fail:
    dstr_free(&output_msg);
    ++drops;
    errno = saved_errno;
    return;
}
