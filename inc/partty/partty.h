#ifndef PARTTY_H
#define PARTTY_H

#include <unibilium.h>
#include <stdint.h>

typedef struct Partty Partty;

typedef struct partty_event partty_event;

typedef enum {
    PARTTY_START = -1,
    PARTTY_BACKSPACE,
    PARTTY_UP,
    PARTTY_DOWN,
    PARTTY_LEFT,
    PARTTY_RIGHT,

    PARTTY_MAX,
} partty_key;

typedef enum {
    PARTTY_UNICODE,
    PARTTY_KEY,
    PARTTY_AMBIG,
    PARTTY_NONE,
} partty_type;

struct partty_event {
    partty_type type;

    union {
        uint32_t codepoint;
        partty_key key;
    };

    char utf8[5]; /* 4 utf8 bytes + null */
};

Partty *partty_create(void);
void partty_destroy(Partty *pt);

int partty_init(void);
void partty_deinit(void);

int partty_feed_bytes(Partty *pt, const char *s, size_t n);
partty_event partty_resolve_event(Partty *pt);

#endif
