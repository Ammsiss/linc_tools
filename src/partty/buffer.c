#include "partty_internals.h"
#include <assert.h>

/* ByteBuf */

size_t byte_buf_space(ByteBuf *b) {
    assert(b);

    return PARTTY_BYTE_CAP - b->end;
}

size_t byte_buf_len(ByteBuf *b) {
    assert(b);

    return b->end - b->begin;
}

void byte_buf_clear(ByteBuf *b) {
    assert(b);

    b->begin = 0;
    b->end = 0;
}

uint8_t *byte_buf_data(ByteBuf *b) {
    return b->data + b->begin;
}

void byte_buf_compact(ByteBuf *b) {
    assert(b);

    if (b->begin == 0)
        return;

    size_t n = byte_buf_len(b);
    memmove(&b->data, &b->data[b->begin], n);

    b->begin = 0;
    b->end = n;
}

void byte_buf_consume(ByteBuf *b, size_t n) {
    assert(b);
    assert(n <= byte_buf_space(b));

    b->begin += n;

    if (b->begin == b->end)
        byte_buf_clear(b);
}

/* CpBuf */

size_t cp_buf_space(CpBuf *cp) {
    return PARTTY_BYTE_CAP - cp->end;
}

size_t cp_buf_len(CpBuf *cp) {
    return cp->end - cp->begin;
}

void cp_buf_clear(CpBuf *cp) {
    assert(cp);

    cp->begin = 0;
    cp->end = 0;
}

void cp_buf_compact(CpBuf *cp) {
    if (cp->begin == 0)
        return;

    size_t n = cp_buf_len(cp);
    memmove(&cp->data, &cp->data[cp->begin], n);

    cp->begin = 0;
    cp->end = n;
}

void cp_buf_push(CpBuf *cp, utf8_data data) {
    cp_buf_compact(cp);

    if (cp_buf_space(cp) == 0)
        return;

    cp->data[cp->end++] = data;
}

void cp_buf_consume(CpBuf *cp, size_t n) {
    assert(cp);
    assert(n <= cp_buf_space(cp));

    cp->begin += n;

    if (cp->begin == cp->end)
        cp_buf_clear(cp);
}

utf8_data *cp_buf_data(CpBuf *cp) {
    return cp->data + cp->begin;
}
