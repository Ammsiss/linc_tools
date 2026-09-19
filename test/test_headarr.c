#include "unity_fixture.h"
#include "headarr.h"

TEST_GROUP(headarr);

/************ Fixture ************/

int *numbers;

TEST_SETUP(headarr) {
    numbers = nullptr;
}

TEST_TEAR_DOWN(headarr) {
    hdr_free(numbers);
}

/************ Tests ************/

TEST(headarr, push_element_to_null_pointer) {
    hdr_push(numbers, 1);

    TEST_ASSERT_EQUAL_size_t(1, hdr_size(numbers));
    TEST_ASSERT_EQUAL_size_t(2, hdr_cap(numbers));
    TEST_ASSERT_EQUAL_INT(1, numbers[0]);
}

TEST(headarr, push_two_elements) {
    hdr_push(numbers, 1);

    TEST_ASSERT_EQUAL_size_t(1, hdr_size(numbers));
    TEST_ASSERT_EQUAL_size_t(2, hdr_cap(numbers));
    TEST_ASSERT_EQUAL_INT(1, numbers[0]);

    hdr_push(numbers, 10);

    TEST_ASSERT_EQUAL_size_t(2, hdr_size(numbers));
    TEST_ASSERT_EQUAL_size_t(2, hdr_cap(numbers));
    TEST_ASSERT_EQUAL_INT(10, numbers[1]);
}

TEST(headarr, push_many_elements) {
    for (int num = 1; num <= 100000; ++num)
        hdr_push(numbers, num);

    for (int i = 0; i < 100000; ++i)
        TEST_ASSERT_EQUAL_INT(i + 1, numbers[i]);

    TEST_ASSERT_EQUAL_size_t(100000, hdr_size(numbers));
    TEST_ASSERT_EQUAL_size_t(131072, hdr_cap(numbers));
}

TEST(headarr, two_dimensional_array) {
    int **grid = nullptr;

    int total_rows = 50;
    int total_cols = 100;

    for (int row = 0; row < total_rows; ++row) {
        hdr_push(grid, nullptr);

        for (int col = 0; col < total_cols; ++col)
            hdr_push(grid[row], row * col);
    }

    TEST_ASSERT_EQUAL_INT(100, grid[10][10]);
    TEST_ASSERT_EQUAL_INT(4851, grid[49][99]);

    TEST_ASSERT_EQUAL_size_t(50, hdr_size(grid));
    TEST_ASSERT_EQUAL_size_t(64, hdr_cap(grid));

    TEST_ASSERT_EQUAL_size_t(100, hdr_size(*grid));
    TEST_ASSERT_EQUAL_size_t(128, hdr_cap(*grid));

    for (int row = 0; row < total_rows; ++row)
        hdr_free(grid[row]);

    hdr_free(grid);
}

TEST(headarr, delete_span_of_elements_from_start) {
    hdr_push(numbers, 1);
    hdr_push(numbers, 2);
    hdr_push(numbers, 3);

    hdr_delete(numbers, 0, 2);

    TEST_ASSERT_EQUAL_size_t(1, hdr_size(numbers));
    TEST_ASSERT_EQUAL_size_t(4, hdr_cap(numbers));
    TEST_ASSERT_EQUAL_INT(3, numbers[0]);
}

TEST(headarr, delete_last_element) {
    hdr_push(numbers, 1);
    hdr_delete(numbers, 0, 1);

    TEST_ASSERT_EQUAL_size_t(0, hdr_size(numbers));
    TEST_ASSERT_EQUAL_size_t(2, hdr_cap(numbers));
}

TEST(headarr, delete_elements_to_end) {
    hdr_push(numbers, 1);
    hdr_push(numbers, 2);
    hdr_push(numbers, 3);

    hdr_delete(numbers, 1, 2);

    TEST_ASSERT_EQUAL_size_t(1, hdr_size(numbers));
    TEST_ASSERT_EQUAL_size_t(4, hdr_cap(numbers));
    TEST_ASSERT_EQUAL_INT(1, numbers[0]);
}

TEST(headarr, insert_elements) {
    hdr_insert(numbers, 0, /*len*/3, { 1, 2, 3});

    TEST_ASSERT_EQUAL_size_t(3, hdr_size(numbers));
    TEST_ASSERT_EQUAL_size_t(3, hdr_cap(numbers));

    TEST_ASSERT_EQUAL_INT(1, numbers[0]);
    TEST_ASSERT_EQUAL_INT(2, numbers[1]);
    TEST_ASSERT_EQUAL_INT(3, numbers[2]);
}

TEST(headarr, insert_elements_before_existing) {
    hdr_push(numbers, 4);
    hdr_insert(numbers, 0, 3, { 1, 2, 3 });

    TEST_ASSERT_EQUAL_size_t(4, hdr_size(numbers));
    TEST_ASSERT_EQUAL_size_t(4, hdr_cap(numbers));

    TEST_ASSERT_EQUAL_INT(1, numbers[0]);
    TEST_ASSERT_EQUAL_INT(2, numbers[1]);
    TEST_ASSERT_EQUAL_INT(3, numbers[2]);
    TEST_ASSERT_EQUAL_INT(4, numbers[3]);
}

TEST(headarr, insert_elements_after_existing) {
    hdr_push(numbers, 1);
    hdr_insert(numbers, 1, 3, { 2, 3, 4 });

    TEST_ASSERT_EQUAL_size_t(4, hdr_size(numbers));
    TEST_ASSERT_EQUAL_size_t(4, hdr_cap(numbers));

    TEST_ASSERT_EQUAL_INT(1, numbers[0]);
    TEST_ASSERT_EQUAL_INT(2, numbers[1]);
    TEST_ASSERT_EQUAL_INT(3, numbers[2]);
    TEST_ASSERT_EQUAL_INT(4, numbers[3]);
}

TEST(headarr, get_last_element) {
    hdr_push(numbers, 1);
    hdr_push(numbers, 2);

    int *num = hdr_last(numbers);
    TEST_ASSERT_EQUAL_INT(2, *num);
}

typedef struct {
    char *grades;
    int id;
} test_struct;

TEST(headarr, push_struct_type) {
    test_struct *data = nullptr;;

    hdr_push(data, { .grades = nullptr, .id = 3 });
    hdr_push(hdr_last(data)->grades, 'A');
    hdr_push(hdr_last(data)->grades, 'B');

    TEST_ASSERT_EQUAL_INT(3, data->id);
    TEST_ASSERT_EQUAL_CHAR('A', data->grades[0]);
    TEST_ASSERT_EQUAL_CHAR('B', data->grades[1]);

    hdr_free(data->grades);
    hdr_free(data);
}

/************ Test runner ************/

TEST_GROUP_RUNNER(headarr) {
    RUN_TEST_CASE(headarr, push_element_to_null_pointer);
    RUN_TEST_CASE(headarr, push_two_elements);
    RUN_TEST_CASE(headarr, push_many_elements);
    RUN_TEST_CASE(headarr, two_dimensional_array);
    RUN_TEST_CASE(headarr, delete_last_element);
    RUN_TEST_CASE(headarr, delete_elements_to_end);
    RUN_TEST_CASE(headarr, delete_span_of_elements_from_start);
    RUN_TEST_CASE(headarr, insert_elements);
    RUN_TEST_CASE(headarr, insert_elements_before_existing);
    RUN_TEST_CASE(headarr, insert_elements_after_existing);
    RUN_TEST_CASE(headarr, get_last_element);
    RUN_TEST_CASE(headarr, push_struct_type);
}
