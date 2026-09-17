#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <unistd.h>

#define LIB_FATAL(msg) \
    lib_fatal("" msg, sizeof("" msg) - 1)

__attribute__ ((__noreturn__))
static inline void lib_fatal(const char *msg, int len) {
    write(STDERR_FILENO, msg, len);
    abort();
}

#endif
