#ifndef HDS_S
#define HDS_S

#include <string.h>
#include <stddef.h>

#define hds_append(hds, s) \
    hds_append_imp(&(hds), s, strlen(s)) \

#define hds_append_n(hds, s, n) \
    hds_append_imp(&(hds), s, n)

#define hds_append_fmt(hds, fmt, ...) \
    hds_append_fmt_imp(&(hds), fmt __VA_OPT__(,) __VA_ARGS__)

#define hds_copy(hds, s) \
    hds_copy_imp(&(hds), s, strlen(s)); \

#define hds_copy_n(hds, s, n) \
    hds_copy_imp(&(hds), s, n)

#define hds_copy_fmt(hds, fmt, ...) \
    hds_copy_fmt_imp(&(hds), fmt __VA_OPT__(,) __VA_ARGS__)

void hds_append_imp(char **hds, const char *s, size_t n);
__attribute__ ((__format__(printf, 2, 3)))
void hds_append_fmt_imp(char **hds, const char *fmt, ...);

void hds_copy_imp(char **hds, const char *s, size_t n);
__attribute__ ((__format__(printf, 2, 3)))
void hds_copy_fmt_imp(char **hds, const char *fmt, ...);

void hds_free(char *hds);

#endif
