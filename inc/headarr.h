#ifndef HEADARR_H
#define HEADARR_H

#include <assert.h>
#include <stddef.h>

#define EL_SZ(arr) \
    sizeof(*(arr))

#define hdr_push(arr, ...) \
    ({ \
            arr = hdr_grow(arr, EL_SZ(arr), 1); \
            *hdr_last(arr) = (typeof(*(arr)))__VA_ARGS__; \
            hdr_last(arr); \
    })

#define hdr_insert(arr, index, n, ...) \
    arr = hdr_insert_imp(arr, (typeof(*arr)[])__VA_ARGS__, EL_SZ(arr), index, n)

#define hdr_delete(arr, index, n) \
    hdr_delete_imp(arr, EL_SZ(arr), index, n)

#define hdr_last(arr) \
    ((typeof(arr))hdr_last_imp(arr, EL_SZ(arr)))

size_t hdr_size(void *arr);
size_t hdr_cap(void *arr);

void hdr_free(void *arr);
void *hdr_grow(void *arr, size_t el_sz, size_t n);
void *hdr_insert_imp(void *arr, void *el, size_t el_sz, size_t idx, size_t n);
void hdr_delete_imp(void *arr_p, size_t el_sz, size_t idx, size_t n);
void *hdr_last_imp(void *arr, size_t el_sz);

#endif
