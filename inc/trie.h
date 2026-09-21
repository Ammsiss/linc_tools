#ifndef TRIE_H
#define TRIE_H

#include <stddef.h>

typedef struct Trie Trie;

typedef enum {
    TRIE_MATCH,
    TRIE_AMBIG,
    TRIE_NONE,
    TRIE_WAIT,
} trie_status;

#define trie_put(tr, s) \
    trie_add(tr, s, NULL, 0)

Trie *trie_create(void);
void trie_destroy(Trie *tr);
void trie_add(Trie *tr, const char *s, void *data, size_t sz);
void *trie_lookup(Trie *tr, const char *s, trie_status *stat);

#endif
