#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headarr.h"
#include "common.h"

#define MAX(a, b) \
    (((a) > (b)) ? a : b)

#define HEADARR_BYTE_N (2 * sizeof(size_t))

#define HEADARR_START(arr) \
    ((size_t *)((char *)(arr) - HEADARR_BYTE_N))

#define HEADARR_SIZE(arr) \
    (HEADARR_START(arr)[0])

#define HEADARR_CAP(arr) \
    (HEADARR_START(arr)[1])

size_t *hdr_start(void *arr) {
    return (arr) ? HEADARR_START(arr) : NULL;
}

size_t hdr_size(void *arr) {
    return (arr) ? HEADARR_SIZE(arr) : 0;
}

size_t hdr_cap(void *arr) {
    return (arr) ? HEADARR_CAP(arr) : 0;
}

void hdr_free(void *arr) {
    if (!arr)
        return;

    free(HEADARR_START(arr));
}

static void *hdr_reserve(void *arr, size_t el_sz, size_t min) {
    size_t cap = hdr_cap(arr);

    if (cap >= min)
        return arr;

    cap = MAX(min, MAX(2, cap * 2));

    void *tmp = realloc(hdr_start(arr), HEADARR_BYTE_N + (cap * el_sz));
    if (!tmp)
        LIB_FATAL("realloc: out of memory");

    arr = (char *)tmp + HEADARR_BYTE_N;
    HEADARR_CAP(arr) = cap;

    return arr;
}

void *hdr_last_imp(void *arr, size_t el_sz) {
    if (hdr_size(arr) == 0)
        return NULL;

    char *p = arr;
    return &p[(hdr_size(arr) - 1) * el_sz];
}

void *hdr_grow(void *arr, size_t el_sz, size_t n) {
    size_t size = hdr_size(arr);

    arr = hdr_reserve(arr, el_sz, size + n);
    HEADARR_SIZE(arr) = size + n;

    return arr;
}

void *hdr_insert_imp(void *arr, void *el, size_t el_sz, size_t idx, size_t n) {
    assert(el);
    assert(el_sz > 0);
    assert(idx <= hdr_size(arr));

    if (n == 0)
        return arr;

    size_t size = hdr_size(arr);
    arr = hdr_grow(arr, el_sz, n);

    char *p = arr;
    memmove(&p[(idx + n) * el_sz], &p[idx * el_sz], (size - idx) * el_sz);

    char *el_p = el;
    for (size_t i = 0; i < n; ++i)
        memcpy(&p[(idx + i) * el_sz], &el_p[i * el_sz], el_sz);

    return arr;
}

void hdr_delete_imp(void *arr, size_t el_sz, size_t index, size_t n) {
    assert(el_sz > 0);
    assert(index + n <= hdr_size(arr));

    if (n == 0)
        return;

    char *p = arr;
    size_t shift_n = HEADARR_SIZE(arr) - (index + n);

    memmove(&p[index * el_sz], &p[(index + n) * el_sz], shift_n * el_sz);

    HEADARR_SIZE(arr) -= n;
}
