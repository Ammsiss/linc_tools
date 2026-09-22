#ifndef PARTTY_H
#define PARTTY_H

#include <unibilium.h>
#include <stdint.h>

typedef struct Partty Partty;
typedef struct partty_event partty_event;

typedef enum {
    PARTTY_BACKSPACE,
    PARTTY_UP,
    PARTTY_DOWN,
    PARTTY_LEFT,
    PARTTY_RIGHT,
    PARTTY_CATAB,
    PARTTY_CLEAR,
    PARTTY_CTAB,
    PARTTY_DC,
    PARTTY_DL,
    PARTTY_EIC,
    PARTTY_EOL,
    PARTTY_EOS,
    PARTTY_F0,
    PARTTY_F1,
    PARTTY_F10,
    PARTTY_F2,
    PARTTY_F3,
    PARTTY_F4,
    PARTTY_F5,
    PARTTY_F6,
    PARTTY_F7,
    PARTTY_F8,
    PARTTY_F9,
    PARTTY_HOME,
    PARTTY_IC,
    PARTTY_IL,
    PARTTY_LL,
    PARTTY_NPAGE,
    PARTTY_PPAGE,
    PARTTY_SF,
    PARTTY_SR,
    PARTTY_STAB,
    PARTTY_A1,
    PARTTY_A3,
    PARTTY_B2,
    PARTTY_C1,
    PARTTY_C3,
    PARTTY_BTAB,
    PARTTY_BEG,
    PARTTY_END,
    PARTTY_ENTER,
    PARTTY_SBEG,
    PARTTY_SDC,
    PARTTY_SEOL,
    PARTTY_SHOME,
    PARTTY_SIC,
    PARTTY_SLEFT,
    PARTTY_SNEXT,
    PARTTY_SPREVIOUS,
    PARTTY_SRIGHT,
    PARTTY_F11,
    PARTTY_F12,
    PARTTY_F13,
    PARTTY_F14,
    PARTTY_F15,
    PARTTY_F16,
    PARTTY_F17,
    PARTTY_F18,
    PARTTY_F19,
    PARTTY_F20,
    PARTTY_F21,
    PARTTY_F22,
    PARTTY_F23,
    PARTTY_F24,
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
