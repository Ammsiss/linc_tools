#include "partty_internals.h"
#include "trie.h"
#include "hds.h"

int resolve_vt_seq(Partty *pt, vt_data *data) {
    size_t n = cp_buf_len(&pt->cp);

    if (n == 0) {
        data->status = VT_RES_NONE;
        return 0;
    }

    char *s = nullptr;
    trie_status stat;
    size_t len = 0;

    data->status = VT_RES_WAIT;

    partty_key saved_key = PARTTY_MAX;
    size_t saved_len = 0;

    for (size_t i = 0; i < n; ++i) {
        uint32_t codepoint = cp_buf_data(&pt->cp)[i].codepoint;

        if (codepoint > 127)
            break;

        hds_append_n(s, (char []){codepoint}, 1);
        partty_key *key = trie_lookup(pt->seqs, s, &stat);

        if (stat == TRIE_MATCH) {
            data->status = VT_RES_OK;
            data->key = *key;
            len = hds_len(s);
            break;

        } else if (stat == TRIE_AMBIG) {
            data->status = VT_RES_WAIT;
            saved_key = *key;
            saved_len = len;
            break;

        } else if (stat == TRIE_NONE) {
            if (saved_key != PARTTY_MAX) {
                data->status = VT_RES_OK;
                data->key = saved_key;
                len = saved_len;
                break;

            } else {
                data->status = VT_RES_NONE;
                break;
            }
        }
    }

    hds_free(s);
    return len;
}
