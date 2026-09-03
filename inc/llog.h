#ifndef LLOG_H
#define LLOG_H

#include <stddef.h>
#include <unistd.h>

#define LLOG_SITE \
    (llog_site_info){ \
        .file = __FILE__, \
        .func = __func__, \
        .line = __LINE__, \
    }

#define LOG_INFO(fmt, ...) \
    llog_log(LLOG_INFO, &LLOG_SITE, fmt __VA_OPT__(,) __VA_ARGS__)

#define LOG_WARN(fmt, ...) \
    llog_log(LLOG_WARN, &LLOG_SITE, fmt __VA_OPT__(,) __VA_ARGS__)

#define LOG_ERR(fmt, ...) \
    llog_log(LLOG_ERR, &LLOG_SITE, fmt __VA_OPT__(,) __VA_ARGS__)

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

#define LLOG_SINK(name) \
    void name(const llog_info *info [[maybe_unused]])

void llog_set_sink(llog_sink *sink);
void llog_reset(void);

__attribute__ ((__format__(printf, 3, 4)))
void llog_log(llog_lvl lvl, const llog_site_info *site, const char *fmt, ...);

#endif
