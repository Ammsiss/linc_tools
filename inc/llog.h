#ifndef LLOG_H
#define LLOG_H

#define _GNU_SOURCE

#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "dstr.h"
#include "dstr.h"
#include <stddef.h>
#include <unistd.h>

#define LLOG_SITE \
    (llog_site_info){ \
        .file = __FILE__, \
        .func = __func__, \
        .line = __LINE__, \
    }

#define LOG_INFO(fmt, ...) \
    llog_log(llog_default(), LLOG_INFO, &LLOG_SITE, fmt __VA_OPT__(,) __VA_ARGS__)

#define LOG_WARN(fmt, ...) \
    llog_log(llog_default(), LLOG_WARN, &LLOG_SITE, fmt __VA_OPT__(,) __VA_ARGS__)

#define LOG_ERR(fmt, ...) \
    llog_log(llog_default(), LLOG_ERR, &LLOG_SITE, fmt __VA_OPT__(,) __VA_ARGS__)

#define LLOG_DEFAULT(log) \
    llog *llog_default(void) { \
        return log; \
    }

typedef enum {
    LLOG_INFO,
    LLOG_WARN,
    LLOG_ERR,
} llog_lvl;

typedef struct {
    const char *file;
    const char *func;
    int line;
} llog_site_info;

typedef struct {
    int saved_errno;
    const llog_site_info *site;
    llog_lvl log_level;
    pid_t pid;
    pid_t ppid;
    pid_t pgid;
    pid_t tid;
    char *msg;
} llog_info;

typedef void (llog_sink)(const llog_info *);

typedef struct {
    llog_sink *sink;
} llog;

llog *llog_default(void);

static inline void convert_special_chars(char **output_msg) {
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

__attribute__ ((__format__(printf, 4, 5)))
static inline void llog_log(llog *log, llog_lvl lvl, const llog_site_info *site,
        const char *fmt, ...)
{
    assert(log);
    assert(site);
    assert(fmt);

    if (!log->sink)
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

    va_start(va, fmt);
     if (vasprintf(&info.msg, fmt, va) < 0)
         LIB_FATAL("llog: vasprintf: allocation or IO error");
     va_end(va);

    convert_special_chars(&info.msg);
    log->sink(&info);
    free(info.msg);

    errno = saved_errno;
    return;
}

#endif
