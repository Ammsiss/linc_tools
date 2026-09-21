#ifndef KVAL
#define KVAL

#include <stddef.h>

#include "headarr.h" // IWYU pragma: export

typedef struct {
    void *k;
    void *v;
} kvp;

typedef struct {
    kvp *pairs;
    size_t k_sz;
    size_t v_sz;
} kval;

typedef void *(* k_fn)(kval *, const void *);

#define kv_foreach(kv, x) \
    for (size_t i = 0; i < hdr_size(kv->pairs); ++i) if ((x = kv->pairs + i))

#define kv_add(kv, k, v) \
    _Generic(k, \
        char *: kv_add_imp(kv, &(typeof(k)){k}, &(typeof(v)){v}, str_canon_k), \
        int: kv_add_imp(kv, &(typeof(k)){k}, &(typeof(v)){v}, num_canon_k) \
    )

#define kv_set(kv, k, v) \
    _Generic(k, \
        char *: kv_set_imp(kv, &(typeof(k)){k}, &(typeof(v)){v}, str_canon_k), \
        int: kv_set_imp(kv, &(typeof(k)){k}, &(typeof(v)){v}, num_canon_k) \
    )

#define kv_lookup(kv, k) \
    _Generic(k, \
        char *: kv_lookup_imp(kv, &(typeof(k)){k}, str_canon_k), \
        int: kv_lookup_imp(kv, &(typeof(k)){k}, num_canon_k) \
    )

#define kv_delete(kv, k) \
    _Generic(k, \
        char *: kv_delete_imp(kv, &(typeof(k)){k}, str_canon_k), \
        int: kv_delete_imp(kv, &(typeof(k)){k}, num_canon_k) \
    )

kval *kv_create(size_t k_sz, size_t v_sz);
void kv_free(kval *kv);

int kv_add_imp(kval *kv, void *k, void *v, k_fn fn);
int kv_set_imp(kval *kv, void *k, void *v, k_fn fn);
void *kv_lookup_imp(kval *kv, const void *k, k_fn fn);
int kv_delete_imp(kval *kv, void *k, k_fn fn);

void *str_canon_k(kval *kv, const void *k);
void *num_canon_k(kval *kv, const void *k);

#endif
