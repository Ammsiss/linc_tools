#ifndef LINC_TOOLS_HASH_H
#define LINC_TOOLS_HASH_H

#include <stddef.h>
#include <stdint.h>

static inline uint32_t hash_str(const char *s, size_t n) {
    uint32_t val = 0;

    for (size_t i = 0; i < n; ++i)
        val = val * 31 + (uint8_t)s[i];

    return val;
}

#endif
