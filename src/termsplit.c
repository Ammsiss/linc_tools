#include <assert.h>
#include <stdlib.h>

#include "termsplit.h"
#include "xfuncs.h"
#include "common.h"

static void calc_quad(ts_node *node, ts_rect rects[TS_NODE_CHILD_MAX]) {
    int gap = node->quad_gap;

    if (gap > node->rect.rows)
        gap = node->rect.rows;

    if (gap > node->rect.cols)
        gap = node->rect.cols;

    int avail_rows = node->rect.rows - gap;
    int avail_cols = node->rect.cols - gap;

    int top_side_rows = avail_rows * node->ratio_y;
    int top_side_y = node->rect.y;

    int bottom_side_rows = avail_rows - top_side_rows;
    int bottom_side_y = top_side_y + top_side_rows + gap;

    int left_side_cols = avail_cols * node->ratio_x;
    int left_side_x = node->rect.x;

    int right_side_cols = avail_cols - left_side_cols;
    int right_side_x = left_side_x + left_side_cols + gap;

    rects[TS_QUAD_TL].rows = top_side_rows;
    rects[TS_QUAD_TL].cols = left_side_cols;
    rects[TS_QUAD_TL].y = top_side_y;
    rects[TS_QUAD_TL].x = left_side_x;

    rects[TS_QUAD_TR].rows = top_side_rows;
    rects[TS_QUAD_TR].cols = right_side_cols;
    rects[TS_QUAD_TR].y = top_side_y;
    rects[TS_QUAD_TR].x = right_side_x;

    rects[TS_QUAD_BL].rows = bottom_side_rows;
    rects[TS_QUAD_BL].cols = left_side_cols;
    rects[TS_QUAD_BL].y = bottom_side_y;
    rects[TS_QUAD_BL].x = left_side_x;

    rects[TS_QUAD_BR].rows = bottom_side_rows;
    rects[TS_QUAD_BR].cols = right_side_cols;
    rects[TS_QUAD_BR].y = bottom_side_y;
    rects[TS_QUAD_BR].x = right_side_x;

    int y_gap_y = top_side_y + top_side_rows;
    int x_gap_x = left_side_x + left_side_cols;

    rects[TS_QUAD_GL].rows = gap;
    rects[TS_QUAD_GL].cols = left_side_cols;
    rects[TS_QUAD_GL].y = y_gap_y;
    rects[TS_QUAD_GL].x = left_side_x;

    rects[TS_QUAD_GR].rows = gap;
    rects[TS_QUAD_GR].cols = right_side_cols;
    rects[TS_QUAD_GR].y = y_gap_y;
    rects[TS_QUAD_GR].x = right_side_x;

    rects[TS_QUAD_GT].rows = top_side_rows;
    rects[TS_QUAD_GT].cols = gap;
    rects[TS_QUAD_GT].y = top_side_y;
    rects[TS_QUAD_GT].x = x_gap_x;

    rects[TS_QUAD_GB].rows = bottom_side_rows;
    rects[TS_QUAD_GB].cols = gap;
    rects[TS_QUAD_GB].y = bottom_side_y;
    rects[TS_QUAD_GB].x = x_gap_x;

    rects[TS_QUAD_GJ].rows = gap;
    rects[TS_QUAD_GJ].cols = gap;
    rects[TS_QUAD_GJ].y = y_gap_y;
    rects[TS_QUAD_GJ].x = x_gap_x;
}

static void calc_split(ts_node *node, ts_rect rects[TS_NODE_CHILD_MAX]) {
    int gap = node->split_gap;

    rects[TS_SPLIT_LEFT] = node->rect;
    rects[TS_SPLIT_RIGHT] = node->rect;
    rects[TS_SPLIT_GAP] = node->rect;

    if (node->axis == TS_HSPLIT) {
        if (node->split_gap > node->rect.rows)
            gap = node->rect.rows;

        int avail = node->rect.rows - gap;

        rects[TS_SPLIT_LEFT].rows = avail * node->ratio;

        rects[TS_SPLIT_GAP].rows = gap;
        rects[TS_SPLIT_GAP].y += rects[TS_SPLIT_LEFT].rows;

        rects[TS_SPLIT_RIGHT].rows = avail - rects[TS_SPLIT_LEFT].rows;
        rects[TS_SPLIT_RIGHT].y += rects[TS_SPLIT_LEFT].rows + gap;
    }

    if (node->axis == TS_VSPLIT) {
        if (node->split_gap > node->rect.cols)
            gap = node->rect.cols;

        int avail = node->rect.cols - node->split_gap;

        rects[TS_SPLIT_LEFT].cols = avail * node->ratio;

        rects[TS_SPLIT_GAP].cols = gap;
        rects[TS_SPLIT_GAP].x += rects[TS_SPLIT_LEFT].cols;

        rects[TS_SPLIT_RIGHT].cols = avail - rects[TS_SPLIT_LEFT].cols;
        rects[TS_SPLIT_RIGHT].x += rects[TS_SPLIT_LEFT].cols + node->split_gap;
    }
}

ts_node *ts_new_quad(ts_quad_config cfg) {
    assert(cfg.ratio_x > 0 && cfg.ratio_x <= 1.0F);
    assert(cfg.ratio_y > 0 && cfg.ratio_y <= 1.0F);
    assert(cfg.quad_gap >= 0);
    assert(cfg.tl && cfg.tr && cfg.bl && cfg.br);
    assert(cfg.gl && cfg.gr && cfg.gt && cfg.gb && cfg.gj);

    ts_node *quad = xmalloc(sizeof(ts_node));

    *quad = (ts_node){
        .type = TS_QUAD,
        .ratio_x = cfg.ratio_x,
        .ratio_y = cfg.ratio_y,
        .quad_gap = cfg.quad_gap,
        .child_n = TS_QUAD_CHILD_MAX,
    };

    quad->children[TS_QUAD_TL] = cfg.tl;
    quad->children[TS_QUAD_TR] = cfg.tr;
    quad->children[TS_QUAD_BL] = cfg.bl;
    quad->children[TS_QUAD_BR] = cfg.br;
    quad->children[TS_QUAD_GL] = cfg.gl;
    quad->children[TS_QUAD_GR] = cfg.gr;
    quad->children[TS_QUAD_GT] = cfg.gt;
    quad->children[TS_QUAD_GB] = cfg.gb;
    quad->children[TS_QUAD_GJ] = cfg.gj;

    return quad;
}

ts_node *ts_new_split(ts_split_config cfg) {
    assert(cfg.ratio > 0 && cfg.ratio <= 1.0F);
    assert(cfg.left && cfg.right);
    assert(cfg.split_gap >= 0);

    ts_node *split = xmalloc(sizeof(ts_node));

    *split = (ts_node){
        .type = TS_SPLIT,
        .ratio = cfg.ratio,
        .axis = cfg.axis,
        .split_gap = cfg.split_gap,
        .child_n = TS_SPLIT_CHILD_MAX,
    };

    split->children[TS_SPLIT_LEFT] = cfg.left;
    split->children[TS_SPLIT_RIGHT] = cfg.right;
    split->children[TS_SPLIT_GAP] = cfg.gap;

    return split;
}

ts_node *ts_new_leaf(ts_init_fn *fn1, ts_resize_fn *fn2, ts_free_fn *fn3) {
    assert(fn1 && fn2 && fn3);

    ts_node *leaf = xmalloc(sizeof(ts_node));

    *leaf = (ts_node){
        .type = TS_LEAF,
        .is_data_init = false,
        .init_data = fn1,
        .resize = fn2,
        .free_data = fn3,
    };

    return leaf;
}

void ts_refresh(ts_node *node, ts_rect rect) {
    assert(node);

    uint32_t change_flags = 0;

    if (rect.y != node->rect.y || rect.x != node->rect.x)
        change_flags |= TS_MOVED;

    if (rect.rows != node->rect.rows || rect.cols != node->rect.cols)
        change_flags |= TS_RESIZED;

    node->rect = rect;

    if (node->type == TS_LEAF) {
        if (!node->is_data_init) {
            if (node->init_data(node) == 0)
                node->is_data_init = true;

        } else if (change_flags != 0)
            node->resize(node, change_flags);

    } else {
        ts_rect rects[TS_NODE_CHILD_MAX];

        switch (node->type) {
        case TS_SPLIT: calc_split(node, rects); break;
        case TS_QUAD:  calc_quad(node, rects);  break;
        default: LIB_FATAL("termsplit: unexpected node type");
        }

        for (size_t i = 0; i < node->child_n; ++i)
            ts_refresh(node->children[i], rects[i]);
    }
}

void ts_free(ts_node *node) {
    if (node->type == TS_LEAF) {
        node->free_data(node);

    } else {
        for (size_t i = 0; i < node->child_n; ++i)
            ts_free(node->children[i]);
    }

    free(node);
}
