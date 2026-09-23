#ifndef HDA_H
#define HDA_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#define MAX(a, b) \
    (((a) > (b)) ? a : b)

#define HDA_BYTE_N (2 * sizeof(size_t))

#define HDA_START(arr) \
    ((size_t *)((char *)(arr) - HDA_BYTE_N))

#define HDA_SIZE(arr) \
    (HDA_START(arr)[0])

#define HDA_CAP(arr) \
    (HDA_START(arr)[1])

#define EL_SZ(hda) \
    sizeof(*(hda))

#define hda_push(hda, ...) \
    ({ \
        hda = hda_grow(hda, EL_SZ(hda), 1); \
        *hda_last(hda) = (typeof(*(hda)))__VA_ARGS__; \
        hda_last(hda); \
    })

#define hda_insert(hda, index, n, ...) \
    hda = hda_insert_imp(hda, (typeof(*hda)[])__VA_ARGS__, EL_SZ(hda), index, n)

#define hda_delete(hda, index, n) \
    hda_delete_imp(hda, EL_SZ(hda), index, n)

#define hda_last(hda) \
    ((typeof(hda))hda_last_imp(hda, EL_SZ(hda)))

static inline size_t *hda_start(const void *hda) {
    return (hda) ? HDA_START(hda) : NULL;
}

static inline size_t hda_size(const void *hda) {
    return (hda) ? HDA_SIZE(hda) : 0;
}

static inline size_t hda_cap(const void *hda) {
    return (hda) ? HDA_CAP(hda) : 0;
}

static inline void hda_free(void *hda) {
    if (!hda)
        return;

    free(HDA_START(hda));
}

static inline void *hda_reserve(void *hda, size_t el_sz, size_t min) {
    size_t cap = hda_cap(hda);

    if (cap >= min)
        return hda;

    cap = MAX(min, MAX(2, cap * 2));

    void *tmp = realloc(hda_start(hda), HDA_BYTE_N + (cap * el_sz));
    if (!tmp)
        LIB_FATAL("realloc: out of memory");

    hda = (char *)tmp + HDA_BYTE_N;
    HDA_CAP(hda) = cap;

    return hda;
}

static inline void *hda_last_imp(void *hda, size_t el_sz) {
    if (hda_size(hda) == 0)
        return NULL;

    char *p = hda;
    return &p[(hda_size(hda) - 1) * el_sz];
}

static inline void *hda_grow(void *hda, size_t el_sz, size_t n) {
    size_t size = hda_size(hda);

    hda = hda_reserve(hda, el_sz, size + n);
    HDA_SIZE(hda) = size + n;

    return hda;
}

static inline void *hda_insert_imp(void *hda, const void *els, size_t el_sz,
        size_t idx, size_t n)
{
    assert(els);
    assert(el_sz > 0);
    assert(idx <= hda_size(hda));

    if (n == 0)
        return hda;

    if (n > SIZE_MAX / el_sz)
        LIB_FATAL("allocation overflow");

    /* to guard against overlapping hda and el on realloc.
     * Can avoid the malloc on every insert by checking for
     * address overlap (uintptr_t) */
    void *el_copy = malloc(el_sz * n);
    if (!el_copy)
        LIB_FATAL("malloc: out of memory");

    memcpy(el_copy, els, el_sz * n);

    size_t size = hda_size(hda);
    hda = hda_grow(hda, el_sz, n);

    char *p = hda;
    memmove(&p[(idx + n) * el_sz], &p[idx * el_sz], (size - idx) * el_sz);
    memcpy(&p[idx * el_sz], el_copy, el_sz * n);

    free(el_copy);
    return hda;
}

static inline void hda_delete_imp(void *hda, size_t el_sz, size_t index, size_t n) {
    assert(el_sz > 0);
    assert(index + n <= hda_size(hda));

    if (n == 0)
        return;

    char *p = hda;
    size_t shift_n = HDA_SIZE(hda) - (index + n);

    memmove(&p[index * el_sz], &p[(index + n) * el_sz], shift_n * el_sz);

    HDA_SIZE(hda) -= n;
}

#endif
