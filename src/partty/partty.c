#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unibilium.h>

#include "partty.h"
#include "partty_internals.h"

static struct {
    const char *name;
    const char *cap_str;
} partty_keys[PARTTY_MAX] = {
    [PARTTY_BACKSPACE] = { "backspace", NULL },
    [PARTTY_UP]        = { "up",        NULL },
    [PARTTY_DOWN]      = { "down",      NULL },
    [PARTTY_LEFT]      = { "left",      NULL },
    [PARTTY_RIGHT]     = { "right",     NULL },
};

Partty *partty_create(void) {
    const char *lang = getenv("LANG");
    if (!lang)
        return NULL;

    if (!(lang = strchr(lang, '.')) || !++lang || !streq("UTF-8", lang))
        return NULL;

    Partty *pt = malloc(sizeof(Partty));
    if (!pt)
        return NULL;

    *pt = (Partty){0};

    pt->term = unibi_from_env();
    if (!pt->term) {
        free(pt);
        return NULL;
    }

    for (int uc = unibi_string_begin_ + 1; uc < unibi_string_end_; ++uc) {
        for (size_t i = 0; i < PARTTY_MAX; ++i) {
            if (streq(unibi_name_str(uc), partty_keys[i].name))
                partty_keys->cap_str = unibi_get_str(pt->term, uc);
        }
    }

    return pt;
}

void partty_destroy(Partty *pt) {
    for (size_t i = 0; i < PARTTY_MAX; ++i)
        partty_keys[i].cap_str = NULL;

    unibi_destroy(pt->term);
    free(pt);
}

int partty_feed_bytes(Partty *pt, const char *s, size_t n) {
    assert(pt && s);

    if (byte_buf_space(&pt->b) < n)
        byte_buf_compact(&pt->b);

    n = MIN(byte_buf_space(&pt->b), n);

    memcpy(&pt->b.data[pt->b.end], s, n);
    pt->b.end += n;

    return n;
}

partty_event partty_resolve_event(Partty *pt) {
    assert(pt);

    resolve_codepoints(pt);

    if (byte_buf_len(&pt->b) == 0 && cp_buf_len(&pt->cp) == 0)
        return (partty_event){ .type = PARTTY_NONE };

    if (cp_buf_len(&pt->cp) == 0)
        return (partty_event){ .type = PARTTY_AMBIG };

    vt_data vt_data;
    partty_event pev;
    utf8_data utf8_data = *cp_buf_data(&pt->cp);

    if (utf8_data.codepoint == '\x1b') {
        int n = resolve_vt_seq(&utf8_data, cp_buf_len(&pt->cp), &vt_data);

        switch (vt_data.status) {
        case VT_RES_OK:
            pev.type = PARTTY_KEY;
            pev.key = vt_data.key;
            cp_buf_consume(&pt->cp, n);
            break;

        case VT_RES_WAIT:
            pev.type = PARTTY_AMBIG;
            break;

        case VT_RES_UNKNOWN:
            pev.type = PARTTY_NONE;
            break;
        }

    } else {
        pev.type = PARTTY_UNICODE;
        pev.codepoint = utf8_data.codepoint;
        memcpy(pev.utf8, utf8_data.utf8, UTF8_BUF_SIZE);

        cp_buf_consume(&pt->cp, 1);
    }

    return pev;
}
