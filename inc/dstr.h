#ifndef DSTR_H
#define DSTR_H

#include <stddef.h>

#define DSTR_PFFORMAT(x, y) __attribute__ ((format(printf, (x), (y))))

struct dstr {
    char *c_str;
    size_t len;
    size_t size;
    size_t cap;
};

typedef struct dstr dstr;

void dstr_init(dstr *str);
void dstr_free(dstr *str);
void dstr_reserve(dstr *str, size_t min);
void dstr_push(dstr *str, char c);
void dstr_clear(dstr *str);
void dstrcpy(dstr *str, char *c);
void dstrncpy(dstr *dst, char *src, size_t n);
void dstrcat(dstr *dst, const char *src);
void dstrncat(dstr *dst, const char *src, size_t n);

DSTR_PFFORMAT(2, 3)
void dstr_printf(dstr *dst, char *fmt, ...);

#endif
