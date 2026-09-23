#include "unity_fixture.h"
#include "xfuncs.h"

TEST_GROUP(xfuncs);

/************ Fixture ************/

void xfatal_func(const xinfo *info [[maybe_unused]], ...) {
}

TEST_SETUP(xfuncs) {
}

TEST_TEAR_DOWN(xfuncs) {
}

/************ Tests ************/

TEST(xfuncs, init_test) {
}

/************ Test runner ************/

TEST_GROUP_RUNNER(xfuncs) {
    RUN_TEST_CASE(xfuncs, init_test);
}
