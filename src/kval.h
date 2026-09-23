#ifndef KVAL
#define KVAL

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "hda.h"

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
    for (size_t i = 0; i < hda_size(kv->pairs); ++i) if ((x = kv->pairs + i))

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

static inline void *lookup_idx(kval *kv, void *k, size_t *idx) {
    for (size_t i = 0; i < hda_size(kv->pairs); ++i) {

        if (memcmp(k, kv->pairs[i].k, kv->k_sz) == 0) {
            if (idx)
                *idx = i;
            return kv->pairs[i].v;
        }
    }

    return NULL;
}

static inline int kv_add_imp(kval *kv, void *k, void *v, k_fn fn) {
    char *canon_k = fn(kv, k);

    if (lookup_idx(kv, canon_k, NULL)) {
        free(canon_k);
        return -1;
    }

    kvp *el = hda_push(kv->pairs, { canon_k, nullptr });

    el->v = malloc(kv->v_sz);
    if (!el->v)
        LIB_FATAL("malloc: out of memory");

    memcpy(el->v, v, kv->v_sz);

    return 0;
}

static inline int kv_set_imp(kval *kv, void *k, void *v, k_fn fn) {
    char *canon_k = fn(kv, k);
    size_t idx;

    if (lookup_idx(kv, canon_k, &idx)) {
        free(kv->pairs[idx].k);
        free(kv->pairs[idx].v);
        hda_delete(kv->pairs, idx, 1);
    }

    kvp *el = hda_push(kv->pairs, { canon_k, nullptr });

    el->v = malloc(kv->v_sz);
    if (!el->v)
        LIB_FATAL("malloc: out of memory");

    memcpy(el->v, v, kv->v_sz);

    return 0;
}

static inline void *kv_lookup_imp(kval *kv, const void *k, k_fn fn) {
    assert(kv);
    assert(k);

    void *canon_k = fn(kv, k);

    void *v = lookup_idx(kv, canon_k, NULL);
    if (!v) {
        free(canon_k);
        return NULL;
    }

    free(canon_k);
    return v;
}

static inline int kv_delete_imp(kval *kv, void *k, k_fn fn) {
    void *canon_k = fn(kv, k);

    size_t idx;
    if (!lookup_idx(kv, canon_k, &idx)) {
        free(canon_k);
        return 0;
    }

    free(kv->pairs[idx].k);
    free(kv->pairs[idx].v);
    hda_delete(kv->pairs, idx, 1);

    free(canon_k);
    return 0;
}

static inline void *str_canon_k(kval *kv, const void *k) {
    assert(kv);
    assert(k);
    assert(strlen(k) < kv->k_sz);

    void *canon_k = calloc(kv->k_sz, 1);
    if (!canon_k)
        LIB_FATAL("calloc: out of memory");

    strcpy(canon_k, k);
    return canon_k;
}

static inline void *num_canon_k(kval *kv, const void *k) {
    assert(kv && k);

    int *canon_k = calloc(kv->k_sz, 1);
    if (!canon_k)
        LIB_FATAL("calloc: out of memory");

    *canon_k = *(int *)k;
    return canon_k;
}

static inline kval *kv_create(size_t k_sz, size_t v_sz) {
    kval *kv = malloc(sizeof(kval));
    if (!kv)
        LIB_FATAL("malloc: out of memory");

    kv->pairs = nullptr;
    kv->k_sz = k_sz;
    kv->v_sz = v_sz;

    return kv;
}

static inline void kv_free(kval *kv) {
    for (size_t i = 0; i < hda_size(kv->pairs); ++i) {
        free(kv->pairs[i].k);
        free(kv->pairs[i].v);
    }

    hda_free(kv->pairs);
    free(kv);
}

#endif
