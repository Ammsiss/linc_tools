#ifndef SPLITTY_H
#define SPLITTY_H

#include <stddef.h>
#include <stdint.h>

#define SP_MOVED 1
#define SP_RESIZED 2
#define SP_NODE_CHILD_MAX 100

typedef struct sp_node sp_node;
typedef struct sp_rect sp_rect;
typedef int (sp_init_fn)(sp_node *node);
typedef void (sp_resize_fn)(sp_node *node, uint32_t flags);
typedef void (sp_free_fn)(sp_node *node);

typedef enum {
    SP_SPLIT_LEFT,
    SP_SPLIT_RIGHT,
    SP_SPLIT_GAP,
    SP_SPLIT_CHILD_MAX,
} sp_split_child;

typedef enum {
    SP_QUAD_TL,
    SP_QUAD_TR,
    SP_QUAD_BL,
    SP_QUAD_BR,

    SP_QUAD_GL,
    SP_QUAD_GR,
    SP_QUAD_GT,
    SP_QUAD_GB,
    SP_QUAD_GJ,
    SP_QUAD_CHILD_MAX,
} sp_quad_child;

typedef enum {
    SP_SPLIT,
    SP_QUAD,
    SP_LEAF,
} sp_node_type;

typedef enum {
    SP_HSPLIT,
    SP_VSPLIT,
} sp_axis;

typedef struct {
    float ratio;
    sp_axis axis;
    int split_gap;
    sp_node *left;
    sp_node *right;
    sp_node *gap;
} sp_split_config;

typedef struct {
    float ratio_y;
    float ratio_x;
    int quad_gap;
    sp_node *tl, *tr, *bl, *br;
    sp_node *gl, *gr, *gt, *gb, *gj;
} sp_quad_config;

struct sp_vec {
    int y;
    int x;
};

struct sp_rect {
    int y;
    int x;
    int rows;
    int cols;
};

struct sp_node {
    sp_rect rect;
    sp_node_type type;

    union {
        /* container */
        struct {
            size_t child_n;
            sp_node *children[SP_NODE_CHILD_MAX];

            union {
                /* split */
                struct {
                    float ratio;
                    sp_axis axis;
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
            sp_init_fn *init_data;
            sp_resize_fn *resize;
            sp_free_fn *free_data;
            void *user_data;
        };
    };
};

sp_node *sp_new_quad(sp_quad_config cfg);
sp_node *sp_new_split(sp_split_config cfg);
sp_node *sp_new_leaf(sp_init_fn *init, sp_resize_fn *resize, sp_free_fn *free);

void sp_refresh(sp_node *node, sp_rect rect);
void sp_free(sp_node *node);

#endif
