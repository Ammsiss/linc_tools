#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hda.h"
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

size_t *hda_start(void *arr) {
    return (arr) ? HDA_START(arr) : NULL;
}

size_t hda_size(void *arr) {
    return (arr) ? HDA_SIZE(arr) : 0;
}

size_t hda_cap(void *arr) {
    return (arr) ? HDA_CAP(arr) : 0;
}

void hda_free(void *arr) {
    if (!arr)
        return;

    free(HDA_START(arr));
}

static void *hda_reserve(void *arr, size_t el_sz, size_t min) {
    size_t cap = hda_cap(arr);

    if (cap >= min)
        return arr;

    cap = MAX(min, MAX(2, cap * 2));

    void *tmp = realloc(hda_start(arr), HDA_BYTE_N + (cap * el_sz));
    if (!tmp)
        LIB_FATAL("realloc: out of memory");

    arr = (char *)tmp + HDA_BYTE_N;
    HDA_CAP(arr) = cap;

    return arr;
}

void *hda_last_imp(void *arr, size_t el_sz) {
    if (hda_size(arr) == 0)
        return NULL;

    char *p = arr;
    return &p[(hda_size(arr) - 1) * el_sz];
}

void *hda_grow(void *arr, size_t el_sz, size_t n) {
    size_t size = hda_size(arr);

    arr = hda_reserve(arr, el_sz, size + n);
    HDA_SIZE(arr) = size + n;

    return arr;
}

void *hda_insert_imp(void *arr, void *el, size_t el_sz, size_t idx, size_t n) {
    assert(el);
    assert(el_sz > 0);
    assert(idx <= hda_size(arr));

    if (n == 0)
        return arr;

    size_t size = hda_size(arr);
    arr = hda_grow(arr, el_sz, n);

    char *p = arr;
    memmove(&p[(idx + n) * el_sz], &p[idx * el_sz], (size - idx) * el_sz);

    char *el_p = el;
    for (size_t i = 0; i < n; ++i)
        memcpy(&p[(idx + i) * el_sz], &el_p[i * el_sz], el_sz);

    return arr;
}

void hda_delete_imp(void *arr, size_t el_sz, size_t index, size_t n) {
    assert(el_sz > 0);
    assert(index + n <= hda_size(arr));

    if (n == 0)
        return;

    char *p = arr;
    size_t shift_n = HDA_SIZE(arr) - (index + n);

    memmove(&p[index * el_sz], &p[(index + n) * el_sz], shift_n * el_sz);

    HDA_SIZE(arr) -= n;
}
