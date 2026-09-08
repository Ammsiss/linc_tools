#include <assert.h>
#include <stdlib.h>

#include "termsplit.h"
#include "xfuncs.h"

static void calc_split(ts_node *node, ts_rect rects[TS_SPLIT_CHILD_MAX]) {
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
        if (!node->is_data_init && node->init_data) {
            node->init_data(node);
            node->is_data_init = true;

        } else if (change_flags != 0 && node->resize)
            node->resize(node, change_flags);

    } else if (node->type == TS_SPLIT) {
        ts_rect rects[TS_SPLIT_CHILD_MAX];
        calc_split(node, rects);

        for (size_t i = 0; i < TS_SPLIT_CHILD_MAX; ++i)
            ts_refresh(node->children[i], rects[i]);
    }
}

void ts_free(ts_node *node) {
    if (node->type == TS_LEAF && node->free_data) {
        node->free_data(node);

    } else if (node->type == TS_SPLIT) {
        for (size_t i = 0; i < TS_SPLIT_CHILD_MAX; ++i)
            ts_free(node->children[i]);
    }

    free(node);
}
