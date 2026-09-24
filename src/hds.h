#ifndef LINC_TOOLS_HDS_S
#define LINC_TOOLS_HDS_S

#include <string.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "hda.h"
#include "common.h"

#define hds_append(hds, s) \
    _hds_append_imp(&(hds), s, strlen(s)) \

#define hds_append_n(hds, s, n) \
    _hds_append_imp(&(hds), s, n)

#define hds_append_fmt(hds, fmt, ...) \
    _hds_append_fmt_imp(&(hds), fmt __VA_OPT__(,) __VA_ARGS__)

#define hds_copy(hds, s) \
    _hds_copy_imp(&(hds), s, strlen(s)); \

#define hds_copy_n(hds, s, n) \
    _hds_copy_imp(&(hds), s, n)

#define hds_copy_fmt(hds, fmt, ...) \
    _hds_copy_fmt_imp(&(hds), fmt __VA_OPT__(,) __VA_ARGS__)

static inline size_t hds_len(const char *hds) {
    size_t size = hda_size(hds);
    return (size == 0) ? 0 : size - 1;
}

static inline void _hds_append_imp(char **hds, const char *s, size_t n) {
    size_t size = hda_size(*hds);
    size_t idx = (size == 0) ? 0 : size - 1;

    *hds = _hda_insert_imp(*hds, s, sizeof(char), idx, n);

    if (size == 0)
        hda_push(*hds, '\0');
}

__attribute__ ((__format__(printf, 2, 3)))
static inline void _hds_append_fmt_imp(char **hds, const char *fmt, ...) {
    va_list va;
    char *s;

    va_start(va, fmt);
    int n = vasprintf(&s, fmt, va);
    va_end(va);

    if (n == -1)
        _LINC_LIB_FATAL("vasprintf: io error");

    hds_append_n(*hds, s, n);

    free(s);
}

static inline void _hds_copy_imp(char **hds, const char *s, size_t n) {
    hda_delete(*hds, 0, hda_size(*hds));
    hds_append_n(*hds, s, n);
}

__attribute__ ((__format__(printf, 2, 3)))
static inline void _hds_copy_fmt_imp(char **hds, const char *fmt, ...) {
    va_list va;
    char *s;

    va_start(va, fmt);
    int n = vasprintf(&s, fmt, va);
    va_end(va);

    if (n == -1)
        _LINC_LIB_FATAL("vasprintf: io error");

    hds_copy_n(*hds, s, n);

    free(s);
}

static inline void hds_free(char *hds) {
    hda_free(hds);
}

#endif
