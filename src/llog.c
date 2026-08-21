#define _GNU_SOURCE

#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

#include "llog.h"
#include "common.h"
#include "dstr.h"

static llog_sink *sink = NULL;

void llog_set_sink(llog_sink *sink_func) {
    sink = sink_func;
}

void llog_reset(void) {
    sink = NULL;
}

static void convert_special_chars(char **output_msg) {
    dstr out;
    dstr_init(&out);

    for (char *c = *output_msg; *c != '\0'; ++c) {
        if (*c == '\n') {
            dstrcat(&out, "\\n");
        } else if (*c == '\t') {
            dstrcat(&out, "\\t");
        } else
            dstr_push(&out, *c);
    }

    free(*output_msg);
    *output_msg = out.c_str;
}

void llog_log(llog_lvl lvl, const llog_site_info *site, const char *fmt, ...) {
    assert(site);
    assert(fmt);

    if (!sink)
        return;

    int saved_errno = errno;

    static llog_info info;
    va_list va;

    info.saved_errno = saved_errno;
    info.site = site;
    info.log_level = lvl;
    info.pid = getpid();
    info.ppid = getppid();
    info.pgid = getpgrp();
    info.tid = gettid();

    va_start(va, fmt);
     if (vasprintf(&info.msg, fmt, va) < 0)
         LIB_FATAL("llog: vasprintf: allocation or IO error");
     va_end(va);

    convert_special_chars(&info.msg);
    sink(&info);
    free(info.msg);

    errno = saved_errno;
    return;
}
