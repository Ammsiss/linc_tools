#ifndef LINC_TOOLS_HDA_H
#define LINC_TOOLS_HDA_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#define _HDA_MAX(a, b) \
    (((a) > (b)) ? a : b)

#define _HDA_BYTE_N (2 * sizeof(size_t))

#define _HDA_START(arr) \
    ((size_t *)((char *)(arr) - _HDA_BYTE_N))

#define _HDA_SIZE(arr) \
    (_HDA_START(arr)[0])

#define _HDA_CAP(arr) \
    (_HDA_START(arr)[1])

#define _HDA_EL_SZ(hda) \
    sizeof(*(hda))

#define hda_push(hda, ...) \
    ({ \
        hda = _hda_grow(hda, _HDA_EL_SZ(hda), 1); \
        *hda_last(hda) = (typeof(*(hda)))__VA_ARGS__; \
        hda_last(hda); \
    })

#define hda_insert(hda, index, n, ...) \
    (hda = _hda_insert_imp( \
            hda, \
            (typeof(*hda)[])__VA_ARGS__, \
            _HDA_EL_SZ(hda), \
            index, \
            n \
    ))

#define hda_delete(hda, index, n) \
    _hda_delete_imp(hda, _HDA_EL_SZ(hda), index, n)

#define hda_last(hda) \
    ((typeof(hda))_hda_last_imp(hda, _HDA_EL_SZ(hda)))

#define hda_free(hda) \
    do { \
        _hda_free_imp(hda); \
        hda = nullptr; \
    } while (false)

static inline size_t *_hda_start(const void *hda) {
    return (hda) ? _HDA_START(hda) : NULL;
}

static inline size_t hda_size(const void *hda) {
    return (hda) ? _HDA_SIZE(hda) : 0;
}

static inline size_t hda_cap(const void *hda) {
    return (hda) ? _HDA_CAP(hda) : 0;
}

static inline void _hda_free_imp(void *hda) {
    if (!hda)
        return;

    free(_HDA_START(hda));
}

static inline void *_hda_reserve(void *hda, size_t el_sz, size_t min) {
    size_t cap = hda_cap(hda);

    if (cap >= min)
        return hda;

    cap = _HDA_MAX(min, _HDA_MAX(2, cap * 2));

    void *tmp = realloc(_hda_start(hda), _HDA_BYTE_N + (cap * el_sz));
    if (!tmp)
        _LINC_LIB_FATAL("realloc: out of memory");

    hda = (char *)tmp + _HDA_BYTE_N;
    _HDA_CAP(hda) = cap;

    return hda;
}

static inline void *_hda_last_imp(void *hda, size_t el_sz) {
    if (hda_size(hda) == 0)
        return NULL;

    char *p = hda;
    return &p[(hda_size(hda) - 1) * el_sz];
}

static inline void *_hda_grow(void *hda, size_t el_sz, size_t n) {
    size_t size = hda_size(hda);

    hda = _hda_reserve(hda, el_sz, size + n);
    _HDA_SIZE(hda) = size + n;

    return hda;
}

static inline void *_hda_insert_imp(void *hda, const void *els,
        size_t el_sz, size_t idx, size_t n)
{
    assert(els);
    assert(el_sz > 0);
    assert(idx <= hda_size(hda));

    if (n == 0)
        return hda;

    if (n > SIZE_MAX / el_sz)
        _LINC_LIB_FATAL("allocation overflow");

    /* to guard against overlapping hda and el on realloc.
     * Can avoid the malloc on every insert by checking for
     * address overlap (uintptr_t) */
    void *el_copy = malloc(el_sz * n);
    if (!el_copy)
        _LINC_LIB_FATAL("malloc: out of memory");

    memcpy(el_copy, els, el_sz * n);

    size_t size = hda_size(hda);
    hda = _hda_grow(hda, el_sz, n);

    char *p = hda;
    memmove(&p[(idx + n) * el_sz], &p[idx * el_sz], (size - idx) * el_sz);
    memcpy(&p[idx * el_sz], el_copy, el_sz * n);

    free(el_copy);
    return hda;
}

static inline void _hda_delete_imp(void *hda, size_t el_sz, size_t index,
        size_t n)
{
    assert(el_sz > 0);
    assert(index + n <= hda_size(hda));

    if (n == 0)
        return;

    char *p = hda;
    size_t shift_n = _HDA_SIZE(hda) - (index + n);

    memmove(&p[index * el_sz], &p[(index + n) * el_sz], shift_n * el_sz);

    _HDA_SIZE(hda) -= n;
}

#undef _HDA_MAX
#undef _HDA_BYTE_N
#undef _HDA_START
#undef _HDA_SIZE
#undef _HDA_CAP

#endif
