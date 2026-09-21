#ifndef HDA_H
#define HDA_H

#include <assert.h>
#include <stddef.h>

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

size_t hda_size(const void *hda);
size_t hda_cap(const void *hda);

void hda_free(void *hda);
void *hda_grow(void *hda, size_t el_sz, size_t n);
void *hda_insert_imp(void *hda, const void *els, size_t el_sz, size_t idx, size_t n);
void hda_delete_imp(void *hda, size_t el_sz, size_t idx, size_t n);
void *hda_last_imp(void *hda, size_t el_sz);

#endif
