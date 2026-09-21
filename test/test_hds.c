#define _GNU_SOURCE

#include "unity_fixture.h"
#include "hds.h"

TEST_GROUP(hds);

/************ Fixture ************/

TEST_SETUP(hds) {
}

TEST_TEAR_DOWN(hds) {
}

/************ Tests ************/

TEST(hds, init_test) {
    TEST_FAIL();
}

/************ Test runner ************/

TEST_GROUP_RUNNER(hds) {
    RUN_TEST_CASE(hds, init_test);
}
