#define _GNU_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "unity_fixture.h"
#include "termsplit.h"

TEST_GROUP(termsplit);

/************ Utils ************/

#define RECT(_r, _c, _y, _x) \
    (ts_rect){ .rows = _r, .cols = _c, .y = _y, .x = _x }

#define LEAF() \
    ts_new_leaf(termsplit_init, termsplit_resize, termsplit_free)

#define USER_DATA(node) \
    *(ts_rect *)node->user_data

#define ASSERT_EQUAL_RECTANGLE(a, b) \
    do { \
        TEST_ASSERT_EQUAL_INT_MESSAGE(a.rows, b.rows, "rows"); \
        TEST_ASSERT_EQUAL_INT_MESSAGE(a.cols, b.cols, "cols"); \
        TEST_ASSERT_EQUAL_INT_MESSAGE(a.y, b.y, "y"); \
        TEST_ASSERT_EQUAL_INT_MESSAGE(a.x, b.x, "x"); \
    } while (false)

static void assert_split_rects(ts_node *split, ts_rect rect, ts_rect left,
        ts_rect right, ts_rect gap)
{
    ASSERT_EQUAL_RECTANGLE(rect, split->rect);
    ASSERT_EQUAL_RECTANGLE(left, split->children[TS_SPLIT_LEFT]->rect);
    ASSERT_EQUAL_RECTANGLE(right, split->children[TS_SPLIT_RIGHT]->rect);
    ASSERT_EQUAL_RECTANGLE(gap, split->children[TS_SPLIT_GAP]->rect);
}

static void assert_quad_rects(ts_node *quad, ts_rect rect, ts_rect tl,
        ts_rect tr, ts_rect bl, ts_rect br, ts_rect gl, ts_rect gr,
        ts_rect gt, ts_rect gb, ts_rect gj)
{
    ASSERT_EQUAL_RECTANGLE(rect, quad->rect);
    ASSERT_EQUAL_RECTANGLE(tl, quad->children[TS_QUAD_TL]->rect);
    ASSERT_EQUAL_RECTANGLE(tr, quad->children[TS_QUAD_TR]->rect);
    ASSERT_EQUAL_RECTANGLE(bl, quad->children[TS_QUAD_BL]->rect);
    ASSERT_EQUAL_RECTANGLE(br, quad->children[TS_QUAD_BR]->rect);
    ASSERT_EQUAL_RECTANGLE(gl, quad->children[TS_QUAD_GL]->rect);
    ASSERT_EQUAL_RECTANGLE(gr, quad->children[TS_QUAD_GR]->rect);
    ASSERT_EQUAL_RECTANGLE(gt, quad->children[TS_QUAD_GT]->rect);
    ASSERT_EQUAL_RECTANGLE(gb, quad->children[TS_QUAD_GB]->rect);
    ASSERT_EQUAL_RECTANGLE(gj, quad->children[TS_QUAD_GJ]->rect);
}

static int termsplit_init(ts_node *node) {
    ts_rect *ts_test_data = malloc(sizeof(ts_rect));
    if (!ts_test_data)
        TEST_FAIL_MESSAGE("malloc");

    *ts_test_data = node->rect;
    node->user_data = ts_test_data;

    return 0;
}

static void termsplit_resize(ts_node *node, uint32_t _) {
    *(ts_rect *)node->user_data = node->rect;
}

static void termsplit_free(ts_node *node) {
    free(node->user_data);
}

/************ Fixture ************/

ts_node *root;

TEST_SETUP(termsplit) {}

TEST_TEAR_DOWN(termsplit) {
    ts_free(root);
    root = NULL;
}

/************ Tests ************/

TEST(termsplit, vertical_split_with_two_leaves) {
    root = ts_new_split((ts_split_config){
            0.5F, TS_VSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    ts_refresh(root, RECT(10, 10, 0, 0));

    assert_split_rects(root,
            RECT(10, 10, 0, 0),
            RECT(10, 5, 0, 0),
            RECT(10, 5, 0, 5),
            RECT(10, 0, 0, 5)
    );
}

TEST(termsplit, horizontal_split_with_two_leaves) {
    root = ts_new_split((ts_split_config){
            0.5F, TS_HSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    ts_refresh(root, RECT(10, 10, 0, 0));

    assert_split_rects(root,
            RECT(10, 10, 0, 0),
            RECT(5, 10, 0, 0),
            RECT(5, 10, 5, 0),
            RECT(0, 10, 5, 0)
    );
}

TEST(termsplit, vertical_split_with_odd_cols) {
    root = ts_new_split((ts_split_config){
            0.5F, TS_VSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    ts_refresh(root, RECT(10, 11, 0, 0));

    assert_split_rects(root,
            RECT(10, 11, 0, 0),
            RECT(10, 5, 0, 0),
            RECT(10, 6, 0, 5),
            RECT(10, 0, 0, 5)
    );
}

TEST(termsplit, horizontal_split_with_odd_rows) {
    root = ts_new_split((ts_split_config){
            0.5F, TS_HSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    ts_refresh(root, RECT(11, 10, 0, 0));

    assert_split_rects(root,
            RECT(11, 10, 0, 0),
            RECT(5, 10, 0, 0),
            RECT(6, 10, 5, 0),
            RECT(0, 10, 5, 0)
    );
}

TEST(termsplit,vertical_split_with_single_col) {
    root = ts_new_split((ts_split_config){
            0.5F, TS_VSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    ts_refresh(root, RECT(10, 1, 0, 0));

    assert_split_rects(root,
            RECT(10, 1, 0, 0),
            RECT(10, 0, 0, 0),
            RECT(10, 1, 0, 0),
            RECT(10, 0, 0, 0)
    );
}

TEST(termsplit, horizontal_split_with_single_row) {
    root = ts_new_split((ts_split_config){
            0.5F, TS_HSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    ts_refresh(root, RECT(1, 10, 0, 0));

    assert_split_rects(root,
            RECT(1, 10, 0, 0),
            RECT(0, 10, 0, 0),
            RECT(1, 10, 0, 0),
            RECT(0, 10, 0, 0)
    );
}

TEST(termsplit, vertical_split_with_zero_cols) {
    root = ts_new_split((ts_split_config){
            0.5F, TS_VSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    ts_refresh(root, RECT(10, 0, 0, 0));

    assert_split_rects(root,
            RECT(10, 0, 0, 0),
            RECT(10, 0, 0, 0),
            RECT(10, 0, 0, 0),
            RECT(10, 0, 0, 0)
    );
}

TEST(termsplit, horizontal_split_with_zero_rows) {
    root = ts_new_split((ts_split_config){
            0.5F, TS_HSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    ts_refresh(root, RECT(0, 10, 0, 0));

    assert_split_rects(root,
            RECT(0, 10, 0, 0),
            RECT(0, 10, 0, 0),
            RECT(0, 10, 0, 0),
            RECT(0, 10, 0, 0)
    );
}

TEST(termsplit, vertical_split_with_zero_cols_zero_rows) {
    root = ts_new_split((ts_split_config){
            0.5F, TS_VSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    ts_refresh(root, RECT(0, 0, 0, 0));

    assert_split_rects(root,
            RECT(0, 0, 0, 0),
            RECT(0, 0, 0, 0),
            RECT(0, 0, 0, 0),
            RECT(0, 0, 0, 0)
    );
}

TEST(termsplit, horizontal_split_with_zero_cols_zero_rows) {
    root = ts_new_split((ts_split_config){
            0.5F, TS_HSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    ts_refresh(root, RECT(0, 0, 0, 0));

    assert_split_rects(root,
            RECT(0, 0, 0, 0),
            RECT(0, 0, 0, 0),
            RECT(0, 0, 0, 0),
            RECT(0, 0, 0, 0)
    );
}

TEST(termsplit, horizontal_split_with_gap) {
    root = ts_new_split((ts_split_config){
            0.5F, TS_HSPLIT, 1, LEAF(), LEAF(), LEAF()
    });

    ts_refresh(root, RECT(10, 10, 0, 0));

    assert_split_rects(root,
            RECT(10, 10, 0, 0),
            RECT(4, 10, 0, 0),
            RECT(5, 10, 5, 0),
            RECT(1, 10, 4, 0)
    );
}

TEST(termsplit, vertical_split_with_gap) {
    root = ts_new_split((ts_split_config){
            0.5F, TS_VSPLIT, 4, LEAF(), LEAF(), LEAF()
    });

    ts_refresh(root, RECT(10, 10, 0, 0));

    assert_split_rects(root,
            RECT(10, 10, 0, 0),
            RECT(10, 3, 0, 0),
            RECT(10, 3, 0, 7),
            RECT(10, 4, 0, 3)
    );
}

TEST(termsplit, horizontal_split_with_gap_equal_to_rows) {
    root = ts_new_split((ts_split_config){
            0.5F, TS_HSPLIT, 10, LEAF(), LEAF(), LEAF()
    });

    ts_refresh(root, RECT(10, 10, 0, 0));

    assert_split_rects(root,
            RECT(10, 10, 0, 0),
            RECT(0, 10, 0, 0),
            RECT(0, 10, 10, 0),
            RECT(10, 10, 0, 0)
    );
}

TEST(termsplit, horizontal_split_with_gap_greater_then_rows) {
    root = ts_new_split((ts_split_config){
            0.5F, TS_HSPLIT, 21, LEAF(), LEAF(), LEAF()
    });

    ts_refresh(root, RECT(10, 10, 0, 0));

    assert_split_rects(root,
            RECT(10, 10, 0, 0),
            RECT(0, 10, 0, 0),
            RECT(0, 10, 10, 0),
            RECT(10, 10, 0, 0)
    );
}

TEST(termsplit, simple_quad) {
    root = ts_new_quad((ts_quad_config){
            0.5F, 0.5F, 1, LEAF(), LEAF(), LEAF(), LEAF(), LEAF(), LEAF(),
            LEAF(), LEAF(), LEAF(),
    });

    ts_refresh(root, RECT(11, 11, 0, 0));

    assert_quad_rects(root,
            RECT(11, 11, 0, 0), // root rec

            RECT(5, 5, 0, 0), // tl
            RECT(5, 5, 0, 6), // tr
            RECT(5, 5, 6, 0), // bl
            RECT(5, 5, 6, 6), // br

            RECT(1, 5, 5, 0), // gl
            RECT(1, 5, 5, 6), // gr
            RECT(5, 1, 0, 5), // gt
            RECT(5, 1, 6, 5), // gb

            RECT(1, 1, 5, 5) // gj
    );
}

TEST(termsplit, quad_gap_equal_to_rows_and_cols) {
    root = ts_new_quad((ts_quad_config){
            0.5F, 0.5F, 10, LEAF(), LEAF(), LEAF(), LEAF(), LEAF(), LEAF(),
            LEAF(), LEAF(), LEAF(),
    });

    ts_refresh(root, RECT(10, 10, 0, 0));

    assert_quad_rects(root,
            // root rec
            RECT(10, 10, 0, 0),
            // main panes
            RECT(0, 0, 0, 0), // tl
            RECT(0, 0, 0, 10), // tr
            RECT(0, 0, 10, 0), // bl
            RECT(0, 0, 10, 10), // br
            // gaps
            RECT(10, 0, 0, 0), // gl
            RECT(10, 0, 0, 10), // gr
            RECT(0, 10, 0, 0), // gt
            RECT(0, 10, 10, 0), // gb

            RECT(10, 10, 0, 0) // gj
    );
}

TEST(termsplit, quad_gap_less_then_rows_larger_then_cols) {
    root = ts_new_quad((ts_quad_config){
            0.5F, 0.5F, 10, LEAF(), LEAF(), LEAF(), LEAF(), LEAF(), LEAF(),
            LEAF(), LEAF(), LEAF(),
    });

    ts_refresh(root, RECT(12, 8, 0, 0));

    assert_quad_rects(root,
            // root rec
            RECT(12, 8, 0, 0),
            // main panes
            RECT(2, 0, 0, 0), // tl
            RECT(2, 0, 0, 8), // tr
            RECT(2, 0, 10, 0), // bl
            RECT(2, 0, 10, 8), // br
            // gaps
            RECT(8, 0, 2, 0), // gl
            RECT(8, 0, 2, 8), // gr
            RECT(2, 8, 0, 0), // gt
            RECT(2, 8, 10, 0), // gb

            RECT(8, 8, 2, 0) // gj
    );
}

/************ Test runner ************/

TEST_GROUP_RUNNER(termsplit) {
    RUN_TEST_CASE(termsplit, vertical_split_with_two_leaves);
    RUN_TEST_CASE(termsplit, horizontal_split_with_two_leaves);
    RUN_TEST_CASE(termsplit, vertical_split_with_odd_cols);
    RUN_TEST_CASE(termsplit, horizontal_split_with_odd_rows);
    RUN_TEST_CASE(termsplit, vertical_split_with_single_col);
    RUN_TEST_CASE(termsplit, horizontal_split_with_single_row);
    RUN_TEST_CASE(termsplit, vertical_split_with_zero_cols);
    RUN_TEST_CASE(termsplit, horizontal_split_with_zero_rows);
    RUN_TEST_CASE(termsplit, vertical_split_with_zero_cols_zero_rows);
    RUN_TEST_CASE(termsplit, horizontal_split_with_zero_cols_zero_rows);
    RUN_TEST_CASE(termsplit, horizontal_split_with_gap);
    RUN_TEST_CASE(termsplit, vertical_split_with_gap);
    RUN_TEST_CASE(termsplit, horizontal_split_with_gap_equal_to_rows);
    RUN_TEST_CASE(termsplit, horizontal_split_with_gap_greater_then_rows);
    RUN_TEST_CASE(termsplit, simple_quad);
    RUN_TEST_CASE(termsplit, quad_gap_equal_to_rows_and_cols);
    RUN_TEST_CASE(termsplit, quad_gap_less_then_rows_larger_then_cols);
}
