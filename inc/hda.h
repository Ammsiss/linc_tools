#ifndef HDA_H
#define HDA_H

#include <assert.h>
#include <stddef.h>

#define EL_SZ(arr) \
    sizeof(*(arr))

#define hda_push(arr, ...) \
    ({ \
            arr = hda_grow(arr, EL_SZ(arr), 1); \
            *hda_last(arr) = (typeof(*(arr)))__VA_ARGS__; \
            hda_last(arr); \
    })

#define hda_insert(arr, index, n, ...) \
    arr = hda_insert_imp(arr, (typeof(*arr)[])__VA_ARGS__, EL_SZ(arr), index, n)

#define hda_delete(arr, index, n) \
    hda_delete_imp(arr, EL_SZ(arr), index, n)

#define hda_last(arr) \
    ((typeof(arr))hda_last_imp(arr, EL_SZ(arr)))

size_t hda_size(void *arr);
size_t hda_cap(void *arr);

void hda_free(void *arr);
void *hda_grow(void *arr, size_t el_sz, size_t n);
void *hda_insert_imp(void *arr, void *el, size_t el_sz, size_t idx, size_t n);
void hda_delete_imp(void *arr_p, size_t el_sz, size_t idx, size_t n);
void *hda_last_imp(void *arr, size_t el_sz);

#endif
