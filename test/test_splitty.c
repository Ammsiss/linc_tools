#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "unity_fixture.h"
#include "splitty.h"

TEST_GROUP(splitty);

/************ Utils ************/

#define RECT(_r, _c, _y, _x) \
    (sp_rect){ .rows = _r, .cols = _c, .y = _y, .x = _x }

#define LEAF() \
    sp_new_leaf(splitty_init, splitty_resize, splitty_free)

#define USER_DATA(node) \
    *(sp_rect *)node->user_data

#define ASSERT_EQUAL_RECTANGLE(a, b) \
    do { \
        TEST_ASSERT_EQUAL_INT_MESSAGE(a.rows, b.rows, "rows"); \
        TEST_ASSERT_EQUAL_INT_MESSAGE(a.cols, b.cols, "cols"); \
        TEST_ASSERT_EQUAL_INT_MESSAGE(a.y, b.y, "y"); \
        TEST_ASSERT_EQUAL_INT_MESSAGE(a.x, b.x, "x"); \
    } while (false)

static void assert_split_rects(sp_node *split, sp_rect rect, sp_rect left,
        sp_rect right, sp_rect gap)
{
    ASSERT_EQUAL_RECTANGLE(rect, split->rect);
    ASSERT_EQUAL_RECTANGLE(left, split->children[SP_SPLIT_LEFT]->rect);
    ASSERT_EQUAL_RECTANGLE(right, split->children[SP_SPLIT_RIGHT]->rect);
    ASSERT_EQUAL_RECTANGLE(gap, split->children[SP_SPLIT_GAP]->rect);
}

static void assert_quad_rects(sp_node *quad, sp_rect rect, sp_rect tl,
        sp_rect tr, sp_rect bl, sp_rect br, sp_rect gl, sp_rect gr,
        sp_rect gt, sp_rect gb, sp_rect gj)
{
    ASSERT_EQUAL_RECTANGLE(rect, quad->rect);
    ASSERT_EQUAL_RECTANGLE(tl, quad->children[SP_QUAD_TL]->rect);
    ASSERT_EQUAL_RECTANGLE(tr, quad->children[SP_QUAD_TR]->rect);
    ASSERT_EQUAL_RECTANGLE(bl, quad->children[SP_QUAD_BL]->rect);
    ASSERT_EQUAL_RECTANGLE(br, quad->children[SP_QUAD_BR]->rect);
    ASSERT_EQUAL_RECTANGLE(gl, quad->children[SP_QUAD_GL]->rect);
    ASSERT_EQUAL_RECTANGLE(gr, quad->children[SP_QUAD_GR]->rect);
    ASSERT_EQUAL_RECTANGLE(gt, quad->children[SP_QUAD_GT]->rect);
    ASSERT_EQUAL_RECTANGLE(gb, quad->children[SP_QUAD_GB]->rect);
    ASSERT_EQUAL_RECTANGLE(gj, quad->children[SP_QUAD_GJ]->rect);
}

static int splitty_init(sp_node *node) {
    sp_rect *sp_test_data = malloc(sizeof(sp_rect));
    if (!sp_test_data)
        TEST_FAIL_MESSAGE("malloc");

    *sp_test_data = node->rect;
    node->user_data = sp_test_data;

    return 0;
}

static void splitty_resize(sp_node *node, uint32_t _) {
    *(sp_rect *)node->user_data = node->rect;
}

static void splitty_free(sp_node *node) {
    free(node->user_data);
}

/************ Fixture ************/

sp_node *root;

TEST_SETUP(splitty) {}

TEST_TEAR_DOWN(splitty) {
    sp_free(root);
    root = NULL;
}

/************ Tests ************/

TEST(splitty, vertical_split_with_two_leaves) {
    root = sp_new_split((sp_split_config){
            0.5F, SP_VSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    sp_refresh(root, RECT(10, 10, 0, 0));

    assert_split_rects(root,
            RECT(10, 10, 0, 0),
            RECT(10, 5, 0, 0),
            RECT(10, 5, 0, 5),
            RECT(10, 0, 0, 5)
    );
}

TEST(splitty, horizontal_split_with_two_leaves) {
    root = sp_new_split((sp_split_config){
            0.5F, SP_HSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    sp_refresh(root, RECT(10, 10, 0, 0));

    assert_split_rects(root,
            RECT(10, 10, 0, 0),
            RECT(5, 10, 0, 0),
            RECT(5, 10, 5, 0),
            RECT(0, 10, 5, 0)
    );
}

TEST(splitty, vertical_split_with_odd_cols) {
    root = sp_new_split((sp_split_config){
            0.5F, SP_VSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    sp_refresh(root, RECT(10, 11, 0, 0));

    assert_split_rects(root,
            RECT(10, 11, 0, 0),
            RECT(10, 5, 0, 0),
            RECT(10, 6, 0, 5),
            RECT(10, 0, 0, 5)
    );
}

TEST(splitty, horizontal_split_with_odd_rows) {
    root = sp_new_split((sp_split_config){
            0.5F, SP_HSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    sp_refresh(root, RECT(11, 10, 0, 0));

    assert_split_rects(root,
            RECT(11, 10, 0, 0),
            RECT(5, 10, 0, 0),
            RECT(6, 10, 5, 0),
            RECT(0, 10, 5, 0)
    );
}

TEST(splitty,vertical_split_with_single_col) {
    root = sp_new_split((sp_split_config){
            0.5F, SP_VSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    sp_refresh(root, RECT(10, 1, 0, 0));

    assert_split_rects(root,
            RECT(10, 1, 0, 0),
            RECT(10, 0, 0, 0),
            RECT(10, 1, 0, 0),
            RECT(10, 0, 0, 0)
    );
}

TEST(splitty, horizontal_split_with_single_row) {
    root = sp_new_split((sp_split_config){
            0.5F, SP_HSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    sp_refresh(root, RECT(1, 10, 0, 0));

    assert_split_rects(root,
            RECT(1, 10, 0, 0),
            RECT(0, 10, 0, 0),
            RECT(1, 10, 0, 0),
            RECT(0, 10, 0, 0)
    );
}

TEST(splitty, vertical_split_with_zero_cols) {
    root = sp_new_split((sp_split_config){
            0.5F, SP_VSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    sp_refresh(root, RECT(10, 0, 0, 0));

    assert_split_rects(root,
            RECT(10, 0, 0, 0),
            RECT(10, 0, 0, 0),
            RECT(10, 0, 0, 0),
            RECT(10, 0, 0, 0)
    );
}

TEST(splitty, horizontal_split_with_zero_rows) {
    root = sp_new_split((sp_split_config){
            0.5F, SP_HSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    sp_refresh(root, RECT(0, 10, 0, 0));

    assert_split_rects(root,
            RECT(0, 10, 0, 0),
            RECT(0, 10, 0, 0),
            RECT(0, 10, 0, 0),
            RECT(0, 10, 0, 0)
    );
}

TEST(splitty, vertical_split_with_zero_cols_zero_rows) {
    root = sp_new_split((sp_split_config){
            0.5F, SP_VSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    sp_refresh(root, RECT(0, 0, 0, 0));

    assert_split_rects(root,
            RECT(0, 0, 0, 0),
            RECT(0, 0, 0, 0),
            RECT(0, 0, 0, 0),
            RECT(0, 0, 0, 0)
    );
}

TEST(splitty, horizontal_split_with_zero_cols_zero_rows) {
    root = sp_new_split((sp_split_config){
            0.5F, SP_HSPLIT, 0, LEAF(), LEAF(), LEAF()
    });

    sp_refresh(root, RECT(0, 0, 0, 0));

    assert_split_rects(root,
            RECT(0, 0, 0, 0),
            RECT(0, 0, 0, 0),
            RECT(0, 0, 0, 0),
            RECT(0, 0, 0, 0)
    );
}

TEST(splitty, horizontal_split_with_gap) {
    root = sp_new_split((sp_split_config){
            0.5F, SP_HSPLIT, 1, LEAF(), LEAF(), LEAF()
    });

    sp_refresh(root, RECT(10, 10, 0, 0));

    assert_split_rects(root,
            RECT(10, 10, 0, 0),
            RECT(4, 10, 0, 0),
            RECT(5, 10, 5, 0),
            RECT(1, 10, 4, 0)
    );
}

TEST(splitty, vertical_split_with_gap) {
    root = sp_new_split((sp_split_config){
            0.5F, SP_VSPLIT, 4, LEAF(), LEAF(), LEAF()
    });

    sp_refresh(root, RECT(10, 10, 0, 0));

    assert_split_rects(root,
            RECT(10, 10, 0, 0),
            RECT(10, 3, 0, 0),
            RECT(10, 3, 0, 7),
            RECT(10, 4, 0, 3)
    );
}

TEST(splitty, horizontal_split_with_gap_equal_to_rows) {
    root = sp_new_split((sp_split_config){
            0.5F, SP_HSPLIT, 10, LEAF(), LEAF(), LEAF()
    });

    sp_refresh(root, RECT(10, 10, 0, 0));

    assert_split_rects(root,
            RECT(10, 10, 0, 0),
            RECT(0, 10, 0, 0),
            RECT(0, 10, 10, 0),
            RECT(10, 10, 0, 0)
    );
}

TEST(splitty, horizontal_split_with_gap_greater_then_rows) {
    root = sp_new_split((sp_split_config){
            0.5F, SP_HSPLIT, 21, LEAF(), LEAF(), LEAF()
    });

    sp_refresh(root, RECT(10, 10, 0, 0));

    assert_split_rects(root,
            RECT(10, 10, 0, 0),
            RECT(0, 10, 0, 0),
            RECT(0, 10, 10, 0),
            RECT(10, 10, 0, 0)
    );
}

TEST(splitty, simple_quad) {
    root = sp_new_quad((sp_quad_config){
            0.5F, 0.5F, 1, LEAF(), LEAF(), LEAF(), LEAF(), LEAF(), LEAF(),
            LEAF(), LEAF(), LEAF(),
    });

    sp_refresh(root, RECT(11, 11, 0, 0));

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

TEST(splitty, quad_gap_equal_to_rows_and_cols) {
    root = sp_new_quad((sp_quad_config){
            0.5F, 0.5F, 10, LEAF(), LEAF(), LEAF(), LEAF(), LEAF(), LEAF(),
            LEAF(), LEAF(), LEAF(),
    });

    sp_refresh(root, RECT(10, 10, 0, 0));

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

TEST(splitty, quad_gap_less_then_rows_larger_then_cols) {
    root = sp_new_quad((sp_quad_config){
            0.5F, 0.5F, 10, LEAF(), LEAF(), LEAF(), LEAF(), LEAF(), LEAF(),
            LEAF(), LEAF(), LEAF(),
    });

    sp_refresh(root, RECT(12, 8, 0, 0));

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

TEST_GROUP_RUNNER(splitty) {
    RUN_TEST_CASE(splitty, vertical_split_with_two_leaves);
    RUN_TEST_CASE(splitty, horizontal_split_with_two_leaves);
    RUN_TEST_CASE(splitty, vertical_split_with_odd_cols);
    RUN_TEST_CASE(splitty, horizontal_split_with_odd_rows);
    RUN_TEST_CASE(splitty, vertical_split_with_single_col);
    RUN_TEST_CASE(splitty, horizontal_split_with_single_row);
    RUN_TEST_CASE(splitty, vertical_split_with_zero_cols);
    RUN_TEST_CASE(splitty, horizontal_split_with_zero_rows);
    RUN_TEST_CASE(splitty, vertical_split_with_zero_cols_zero_rows);
    RUN_TEST_CASE(splitty, horizontal_split_with_zero_cols_zero_rows);
    RUN_TEST_CASE(splitty, horizontal_split_with_gap);
    RUN_TEST_CASE(splitty, vertical_split_with_gap);
    RUN_TEST_CASE(splitty, horizontal_split_with_gap_equal_to_rows);
    RUN_TEST_CASE(splitty, horizontal_split_with_gap_greater_then_rows);
    RUN_TEST_CASE(splitty, simple_quad);
    RUN_TEST_CASE(splitty, quad_gap_equal_to_rows_and_cols);
    RUN_TEST_CASE(splitty, quad_gap_less_then_rows_larger_then_cols);
}
