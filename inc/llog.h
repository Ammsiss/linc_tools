#ifndef LLOG_H
#define LLOG_H

#define PFFORMAT(x, y) __attribute__ ((format(printf, (x), (y))))

#define LOG_INFO(fmt, ...) \
    llog_log(LLOG_INFO, __FILE__, __LINE__, fmt __VA_OPT__(,) __VA_ARGS__)

#define LOG_WARN(fmt, ...) \
    llog_log(LLOG_WARN, __FILE__, __LINE__, fmt __VA_OPT__(,) __VA_ARGS__)

#define LOG_ERR(fmt, ...) \
    llog_log(LLOG_ERR, __FILE__, __LINE__, fmt __VA_OPT__(,) __VA_ARGS__)

#define CCL     "\033[m"
#define CGREEN   "\033[2;36m"
#define CRED     "\033[91m"
#define CYELLOW  "\033[33m"

typedef enum {
    LLOG_INFO,
    LLOG_WARN,
    LLOG_ERR,
} llog_lvl;

enum {
    NO_LOG_FD = -1,
};

int llog_get_log_fd(void);
int llog_get_drops(void);
const char *llog_last_log(void);
void llog_set_fd(int log_fd);
void llog_reset(void);
void llog_log(llog_lvl lvl, const char *file, int line, const char *fmt, ...);

#endif
