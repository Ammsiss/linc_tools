#ifndef COMMON_H
#define COMMON_H

#define LIB_FATAL(msg) \
    lib_fatal("" msg, sizeof("" msg) - 1)

__attribute__ ((__noreturn__))
void lib_fatal(const char *msg, int len);

#endif
