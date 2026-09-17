#include <assert.h>

#include "partty.h"
#include "partty_internals.h"

static inline bool utf8_continue(uint8_t b) {
    return b & 0x80 && !(b & 0x40);
}

static inline bool utf8_ascii(uint8_t b) {
    return !(b & 0x80);
}

utf8_byte_type find_utf8_type(uint8_t b, int *seq_len) {
    /* These are always overlong */
    if (b == 0xC0 || b == 0xC1)
        return UTF8_INVALID;

    if (utf8_continue(b))
        return UTF8_CONTINUE;

    if (utf8_ascii(b))
        return UTF8_ASCII;

    int out = 2; /* including leader */
    uint8_t bit = 0x20;

    while (out != 4 && bit & b && (bit = bit >> 1))
        ++out;

    /* max sequence is 4 bytes */
    if (b & (0x80 >> out))
        return UTF8_INVALID;

    *seq_len = out;
    return UTF8_LEADER;
}

static uint32_t utf8_decode_bytes(uint8_t *bytes, size_t n) {
    assert(n > 1);

    /* WARNING*** allows overlong sequences */

    uint32_t cp = 0;
    uint8_t leader_b = bytes[0];

    uint8_t mask = 0x01;
    for (size_t i = 0; i < 8 - (n + 1); ++i) {
        cp |= leader_b & mask;
        mask *= 2;
    }

    for (size_t i = 1; i < n; ++i) {
        uint8_t b = bytes[i];

        cp <<= 6; /* all continuation bytes have 6 data bits */
        mask = 0x01;
        for (size_t i = 0; i < 6; ++i) {
            cp |= b & mask;
            mask *= 2;
        }
    }

    printf("decoded %d\n", cp);

    if (cp > 0x10FFFF)
        return (uint32_t)-1;

    if (cp >= 0xD800 && cp <= 0xDFFF)
        return (uint32_t)-1;

    return cp;
}

static int utf8_decode(uint8_t *bytes, size_t n, utf8_decode_info *info) {
    assert(bytes && info);

    int seq_len = 0;
    size_t used = 0;
    bool sequence_pending = false;

    info->status = UTF8_DECODE_WAIT;

    for (size_t i = 0; i < n; ++i, ++used) {
        utf8_byte_type t = find_utf8_type(bytes[i], &seq_len);

        if (t == UTF8_INVALID) {
            ++used;
            goto fail;
        }

        if (t == UTF8_ASCII) {
            if (sequence_pending)
                goto fail;

            info->codepoint = bytes[i];
            info->status = UTF8_DECODE_OK;
            ++used;
            break;

        } else if (t == UTF8_CONTINUE) {
            if (!sequence_pending) {
                ++used;
                goto fail;
            }

            if (--seq_len != 0) {
                printf("leader byte len: %d\n", seq_len);
                continue;
            }

            sequence_pending = false;

            uint32_t cp = utf8_decode_bytes(bytes, ++used);
            if (cp == (uint32_t)-1) {
                printf("utf8_decode_bytes failed\n");
                goto fail;
            }

            info->codepoint = cp;
            info->status = UTF8_DECODE_OK;
            break;

        } else if (t == UTF8_LEADER) {
            if (sequence_pending)
                goto fail;

                    /* the leader is part of the sequence */
            seq_len = seq_len - 1;
            sequence_pending = true;

            printf("leader byte len: %d\n", seq_len);
        }
    }

    return used;

fail:
    info->status = UTF8_DECODE_FAIL;
    return used;
}

void resolve_codepoints(Partty *pt) {
    utf8_decode_info info = {0};
    uint8_t *data;
    size_t len;
    size_t n;

    while (true) {
        if (cp_buf_space(&pt->cp) == 0)
            return;

        data = byte_buf_data(&pt->b);
        len = byte_buf_len(&pt->b);

        printf("\nbegin: %zu   end: %zu\n", pt->b.begin, pt->b.end);
        printf("data len: %zu\n", len);

        n = utf8_decode(data, len, &info);

        printf("bytes used: %zu\n", n);

        if (info.status == UTF8_DECODE_OK) {
            printf("decoded code point with %zu bytes!\n", n);

            utf8_data utf8_data;
            utf8_data.codepoint = info.codepoint;
            memcpy(utf8_data.utf8, data, n);

            cp_buf_push(&pt->cp, utf8_data);
            byte_buf_consume(&pt->b, n);

        } else if (info.status == UTF8_DECODE_FAIL) {
            printf("Decoding failed!\n");
            byte_buf_consume(&pt->b, n);

        } else if (info.status == UTF8_DECODE_WAIT) {
            printf("Need more bytes!\n");
            break;
        }
    }
}
