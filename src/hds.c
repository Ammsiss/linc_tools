#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "hds.h"
#include "hda.h"
#include "common.h"

void hds_append_imp(char **hds, const char *s, size_t n) {
    size_t size = hda_size(*hds);
    size_t idx = (size == 0) ? 0 : size - 1;

    *hds = hda_insert_imp(*hds, s, sizeof(char), idx, n);

    if (size == 0)
        hda_push(*hds, '\0');
}

void hds_append_fmt_imp(char **hds, const char *fmt, ...) {
    va_list va;
    char *s;

    va_start(va, fmt);
    int n = vasprintf(&s, fmt, va);
    va_end(va);

    if (n == -1)
        LIB_FATAL("vasprintf: io error");

    hds_append_n(*hds, s, n);

    free(s);
}

void hds_copy_imp(char **hds, const char *s, size_t n) {
    hda_delete(*hds, 0, hda_size(*hds));
    hds_append_n(*hds, s, n);
}

void hds_copy_fmt_imp(char **hds, const char *fmt, ...) {
    va_list va;
    char *s;

    va_start(va, fmt);
    int n = vasprintf(&s, fmt, va);
    va_end(va);

    if (n == -1)
        LIB_FATAL("vasprintf: io error");

    hds_copy_n(*hds, s, n);

    free(s);
}

void hds_free(char *hds) {
    hda_free(hds);
}
