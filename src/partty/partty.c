#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unibilium.h>

#include "partty.h"
#include "partty_internals.h"

static struct {
    const char *name;
} partty_keys[PARTTY_MAX] = {
    [PARTTY_BACKSPACE] = { "key_backspace" },
    [PARTTY_UP]        = { "key_up" },
    [PARTTY_DOWN]      = { "key_down" },
    [PARTTY_LEFT]      = { "key_left" },
    [PARTTY_RIGHT]     = { "key_right" },
    [PARTTY_CATAB]     = { "key_catab" },
    [PARTTY_CLEAR]     = { "key_clear" },
    [PARTTY_CTAB]      = { "key_ctab" },
    [PARTTY_DC]        = { "key_dc" },
    [PARTTY_DL]        = { "key_dl" },
    [PARTTY_EIC]       = { "key_eic" },
    [PARTTY_EOL]       = { "key_eol" },
    [PARTTY_EOS]       = { "key_eos" },
    [PARTTY_HOME]      = { "key_home" },
    [PARTTY_IC]        = { "key_ic" },
    [PARTTY_IL]        = { "key_il" },
    [PARTTY_LL]        = { "key_ll" },
    [PARTTY_NPAGE]     = { "key_npage" },
    [PARTTY_PPAGE]     = { "key_ppage" },
    [PARTTY_SF]        = { "key_sf" },
    [PARTTY_SR]        = { "key_sr" },
    [PARTTY_STAB]      = { "key_stab" },
    [PARTTY_A1]        = { "key_a1" },
    [PARTTY_A3]        = { "key_a3" },
    [PARTTY_B2]        = { "key_b2" },
    [PARTTY_C1]        = { "key_c1" },
    [PARTTY_C3]        = { "key_c3" },
    [PARTTY_BTAB]      = { "key_btab" },
    [PARTTY_BEG]       = { "key_beg" },
    [PARTTY_END]       = { "key_end" },
    [PARTTY_ENTER]     = { "key_enter" },
    [PARTTY_SBEG]      = { "key_sbeg" },
    [PARTTY_SDC]       = { "key_sdc" },
    [PARTTY_SEOL]      = { "key_seol" },
    [PARTTY_SHOME]     = { "key_shome" },
    [PARTTY_SIC]       = { "key_sic" },
    [PARTTY_SLEFT]     = { "key_sleft" },
    [PARTTY_SNEXT]     = { "key_snext" },
    [PARTTY_SPREVIOUS] = { "key_sprevious" },
    [PARTTY_SRIGHT]    = { "key_sright" },
    [PARTTY_F0]        = { "key_f0" },
    [PARTTY_F1]        = { "key_f1" },
    [PARTTY_F2]        = { "key_f2" },
    [PARTTY_F3]        = { "key_f3" },
    [PARTTY_F4]        = { "key_f4" },
    [PARTTY_F5]        = { "key_f5" },
    [PARTTY_F6]        = { "key_f6" },
    [PARTTY_F7]        = { "key_f7" },
    [PARTTY_F8]        = { "key_f8" },
    [PARTTY_F9]        = { "key_f9" },
    [PARTTY_F10]       = { "key_f10" },
    [PARTTY_F11]       = { "key_f11" },
    [PARTTY_F12]       = { "key_f12" },
    [PARTTY_F13]       = { "key_f13" },
    [PARTTY_F14]       = { "key_f14" },
    [PARTTY_F15]       = { "key_f15" },
    [PARTTY_F16]       = { "key_f16" },
    [PARTTY_F17]       = { "key_f17" },
    [PARTTY_F18]       = { "key_f18" },
    [PARTTY_F19]       = { "key_f19" },
    [PARTTY_F20]       = { "key_f20" },
    [PARTTY_F21]       = { "key_f21" },
    [PARTTY_F22]       = { "key_f22" },
    [PARTTY_F23]       = { "key_f23" },
    [PARTTY_F24]       = { "key_f24" },
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

    pt->seqs = trie_create();

    for (int uc = unibi_string_begin_ + 1; uc < unibi_string_end_; ++uc) {
        for (size_t partty_code = 0; partty_code < PARTTY_MAX; ++partty_code) {

            if (streq(unibi_name_str(uc), partty_keys[partty_code].name)) {
                const char *unibi_str = unibi_get_str(pt->term, uc);
                if (unibi_str)
                    trie_add(pt->seqs, unibi_str, &(int){partty_code}, sizeof(int));
            }
        }
    }

    return pt;
}

void partty_destroy(Partty *pt) {
    trie_destroy(pt->seqs);
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
        int n = resolve_vt_seq(pt, &vt_data);

        switch (vt_data.status) {
        case VT_RES_OK:
            pev.type = PARTTY_KEY;
            pev.key = vt_data.key;
            cp_buf_consume(&pt->cp, n);
            break;

        case VT_RES_WAIT:
            pev.type = PARTTY_AMBIG;
            break;

        case VT_RES_NONE:
            pev.type = PARTTY_UNICODE;
            pev.codepoint = utf8_data.codepoint;
            memcpy(pev.utf8, utf8_data.utf8, UTF8_BUF_SIZE);
            cp_buf_consume(&pt->cp, 1);

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
