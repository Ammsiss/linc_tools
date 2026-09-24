#ifndef TRIE_H
#define TRIE_H

#include <stddef.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "common.h"

#define _TRIE_CHILD_MAX 256

typedef enum {
    TRIE_MATCH,
    TRIE_AMBIG,
    TRIE_NONE,
    TRIE_WAIT,
} trie_status;

typedef struct Trie Trie;

struct Trie {
    Trie *children[_TRIE_CHILD_MAX];
    size_t child_n;
    bool term;
    void *data; /* valid if term is true */
};

#define trie_put(tr, s) \
    trie_add(tr, s, NULL, 0)

static inline Trie *trie_create(void) {
    Trie *tr = calloc(1, sizeof(Trie));
    if (!tr)
        _LINC_LIB_FATAL("calloc: out of memory");

    return tr;
}

static inline void trie_destroy(Trie *tr) {
    for (size_t i = 0; i < _TRIE_CHILD_MAX; ++i)
        if (tr->children[i])
            trie_destroy(tr->children[i]);

    if (tr->data)
        free(tr->data);

    free(tr);
}

static inline void trie_add(Trie *tr, const char *s, void *data, size_t sz) {
    assert(tr);
    assert(s);

    if (s[0] == '\0') {
        tr->term = true;

        if (sz > 0) {
            tr->data = malloc(sz);
            if (!tr->data)
                _LINC_LIB_FATAL("malloc: out of memory");

            memcpy(tr->data, data, sz);
        }

        return;
    }

    const uint8_t ch = s[0];

    if (!tr->children[ch]) {
        tr->children[ch] = trie_create();
        ++tr->child_n;
    }

    trie_add(tr->children[ch], &s[1], data, sz);
}

static inline void *trie_lookup(Trie *tr, const char *s, trie_status *stat) {
    assert(tr);
    assert(s);
    assert(stat);

    const uint8_t ch = s[0];

    if (ch == '\0') {
        if (tr->term && tr->child_n == 0) {
            *stat = TRIE_MATCH;
        } else if (tr->term && tr->child_n > 0) {
            *stat = TRIE_AMBIG;
        } else if (!tr->term && tr->child_n > 0) {
            *stat = TRIE_WAIT;
        } else if (!tr->term && tr->child_n == 0)
            *stat = TRIE_NONE;

        return tr->data;
    }

    if (!tr->children[ch]) {
        *stat = TRIE_NONE;
        return NULL;
    }

    return trie_lookup(tr->children[ch], &s[1], stat);
}

#undef _TRIE_CHILD_MAX
#endif
