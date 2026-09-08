#ifndef TERMSPLIT_H
#define TERMSPLIT_H

#include <stddef.h>
#include <stdint.h>

#define TS_MOVED 1
#define TS_RESIZED 2
#define TS_MAX_CHILD 100

typedef struct ts_node ts_node;
typedef struct ts_rect ts_rect;
typedef void (ts_resize_fn)(ts_node *node, uint32_t flags);
typedef void (ts_init_fn)(ts_node *node);
typedef void (ts_free_fn)(ts_node *node);

typedef enum {
    TS_SPLIT_LEFT,
    TS_SPLIT_RIGHT,
    TS_SPLIT_GAP,
    TS_SPLIT_CHILD_MAX,
} ts_split_child;

typedef enum {
    TS_QUAD_TL,
    TS_QUAD_TR,
    TS_QUAD_BL,
    TS_QUAD_BR,

    TS_QUAD_GL,
    TS_QUAD_GR,
    TS_QUAD_GT,
    TS_QUAD_GB,
    TS_QUAD_GJ,
    TS_QUAD_MAX,
} ts_quad_child;

typedef enum {
    TS_SPLIT,
    TS_QUAD,
    TS_LEAF,
} ts_node_type;

typedef enum {
    TS_HSPLIT,
    TS_VSPLIT,
} ts_axis;

typedef struct {
    float ratio;
    ts_axis axis;
    int split_gap;
    ts_node *left;
    ts_node *right;
    ts_node *gap;
} ts_split_config;

struct ts_vec {
    int y;
    int x;
};

struct ts_rect {
    int y;
    int x;
    int rows;
    int cols;
};

struct ts_node {
    ts_rect rect;
    ts_node_type type;

    union {
        /* container */
        struct {
            size_t child_n;
            ts_node *children[TS_MAX_CHILD];

            union {
                /* split */
                struct {
                    float ratio;
                    ts_axis axis;
                    int split_gap;
                };

                /* quad */
                struct {
                    float ratio_x;
                    float ratio_y;
                    int quad_gap;
                };
            };
        };

        /* leaf */
        struct {
            bool is_data_init;
            ts_init_fn *init_data;
            ts_resize_fn *resize;
            ts_free_fn *free_data;
            void *user_data;
        };
    };
};

ts_node *ts_new_split(ts_split_config cfg);

ts_node *ts_new_leaf(ts_init_fn *init, ts_resize_fn *resize, ts_free_fn *free);
void ts_refresh(ts_node *node, ts_rect rect);
void ts_free(ts_node *node);

#endif
