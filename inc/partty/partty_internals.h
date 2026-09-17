#ifndef PARTTY_INTERNALS_H
#define PARTTY_INTERNALS_H

#include <stdint.h>
#include <string.h>

#include "partty.h"
#include "unibilium.h"

#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)
#define streq(a, b) (strcmp(a, b) == 0)

#define PARTTY_BYTE_CAP 1024
#define PARTTY_CP_CAP   64
#define UTF8_BUF_SIZE   5

typedef enum {
    VT_RES_OK,
    VT_RES_WAIT,
    VT_RES_UNKNOWN,
} vt_res_status;

typedef struct {
    vt_res_status status;
    partty_key key;
} vt_data;

typedef enum {
    UTF8_CONTINUE,
    UTF8_LEADER,
    UTF8_ASCII,
    UTF8_INVALID,
} utf8_byte_type;

typedef enum {
    UTF8_DECODE_OK,
    UTF8_DECODE_WAIT,
    UTF8_DECODE_FAIL,
} utf8_decode_status;

typedef struct {
    utf8_decode_status status;
    uint32_t codepoint;
} utf8_decode_info;

typedef struct {
    uint32_t codepoint;
    char utf8[UTF8_BUF_SIZE];
} utf8_data;

typedef struct {
    uint8_t data[PARTTY_BYTE_CAP];
    size_t begin;
    size_t end;
} ByteBuf;

typedef struct {
    utf8_data data[PARTTY_CP_CAP];
    size_t begin;
    size_t end;
} CpBuf;

struct Partty {
    ByteBuf b;
    CpBuf cp;
    unibi_term *term;
};

/* buffer.c */
size_t byte_buf_space(ByteBuf *b);
size_t byte_buf_len(ByteBuf *buf);
uint8_t *byte_buf_data(ByteBuf *b);
void byte_buf_consume(ByteBuf *b, size_t n);
void byte_buf_compact(ByteBuf *b);

size_t cp_buf_space(CpBuf *cp);
size_t cp_buf_len(CpBuf *cp);
void cp_buf_push(CpBuf *cp, utf8_data data);
void cp_buf_consume(CpBuf *cp, size_t n);
void cp_buf_compact(CpBuf *cp);
utf8_data *cp_buf_data(CpBuf *cp);

/* utf8.c */
utf8_byte_type find_utf8_type(uint8_t b, int *seq_len);
void resolve_codepoints(Partty *pt);

/* vt.c */
int resolve_vt_seq(utf8_data *cp, size_t n, vt_data *key);

#endif
