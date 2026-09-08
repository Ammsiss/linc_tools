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

static void assert_equal_rectangle(ts_rect a, ts_rect b) {
    TEST_ASSERT_EQUAL_INT(a.rows, b.rows);
    TEST_ASSERT_EQUAL_INT(a.cols, b.cols);
    TEST_ASSERT_EQUAL_INT(a.y, b.y);
    TEST_ASSERT_EQUAL_INT(a.x, b.x);
}

static void assert_split_rects(ts_node *split, ts_rect rect, ts_rect left,
        ts_rect right, ts_rect gap)
{
    assert_equal_rectangle(rect, split->rect);
    assert_equal_rectangle(left, split->children[TS_SPLIT_LEFT]->rect);
    assert_equal_rectangle(right, split->children[TS_SPLIT_RIGHT]->rect);
    assert_equal_rectangle(gap, split->children[TS_SPLIT_GAP]->rect);
}

static void termsplit_init(ts_node *node) {
    ts_rect *ts_test_data = malloc(sizeof(ts_rect));
    if (!ts_test_data)
        TEST_FAIL_MESSAGE("malloc");

    *ts_test_data = node->rect;
    node->user_data = ts_test_data;
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

TEST(termsplit, leaf_as_root) {
    root = LEAF();

    ts_refresh(root, RECT(10, 10, 0, 0));
    assert_equal_rectangle(RECT(10, 10, 0, 0), root->rect);
    assert_equal_rectangle(RECT(10, 10, 0, 0), USER_DATA(root));

    ts_refresh(root, RECT(10, 5, 2, 2));
    assert_equal_rectangle(root->rect, RECT(10, 5, 2, 2));
    assert_equal_rectangle(USER_DATA(root), RECT(10, 5, 2, 2));
}

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

/************ Test runner ************/

TEST_GROUP_RUNNER(termsplit) {
    RUN_TEST_CASE(termsplit, leaf_as_root);
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
}
