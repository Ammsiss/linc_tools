#ifndef UTILS_H
#define UTILS_H

/* Utils shared among test groups */

#include <stdio.h>

#define ERR_STR_SIZE 10000

static char err_str[ERR_STR_SIZE];

static inline char *get_err_str(const char *msg) {
    snprintf(err_str, ERR_STR_SIZE, "%s: %m", msg);
    return err_str;
}

#define xpipe(pfd) \
    ({ \
        int rv = pipe2(pfd, O_NONBLOCK); \
        if (rv == -1) \
            TEST_FAIL_MESSAGE(get_err_str("pipe")); \
        rv; \
    })

#define xclose(fd) \
    ({ \
        int rv = close(fd); \
        if (rv == -1) \
            TEST_FAIL_MESSAGE(get_err_str("close")); \
        rv; \
    })

#define xread(fd, buf, count) \
    ({ \
        int rv = read(fd, buf, count); \
        if (rv == -1) \
            TEST_FAIL_MESSAGE(get_err_str("read")); \
        rv; \
    })

#endif
