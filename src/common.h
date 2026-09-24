#ifndef LINC_TOOLS_COMMON_H
#define LINC_TOOLS_COMMON_H

#include <stdlib.h>
#include <unistd.h>

#define _LINC_LIB_FATAL(msg) \
    _linc_lib_fatal("" msg "\n", sizeof("" msg "\n") - 1)

__attribute__ ((__noreturn__))
static inline void _linc_lib_fatal(const char *msg, int len) {
    write(STDERR_FILENO, msg, len);
    abort();
}

#endif
