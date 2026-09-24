#include "unity_fixture.h"
#include "linc_tools.h"

TEST_GROUP(xfuncs);

/************ Fixture ************/

XFATAL_HANDLER(xfatal_func) {
    va_list va;
    va_start(va, info);
    int xfatal_arg = va_arg(va, int);
    va_end(va);

    TEST_ASSERT_EQUAL_INT(10, XERRNO);
    TEST_ASSERT_EQUAL_STRING("test_name", XSYSNAME);
    TEST_ASSERT_EQUAL_INT(getpid(), XPID);
    TEST_ASSERT_EQUAL_INT(getppid(), XPPID);
    TEST_ASSERT_EQUAL_INT(getpgid(0), XPGID);
    TEST_ASSERT_EQUAL_INT(gettid(), XTID);
    TEST_ASSERT_EQUAL_INT(1, xfatal_arg);

    if (info->available & XFUNCS_AVAIL_SID) {
        pid_t sid = getsid(0);
        TEST_ASSERT_NOT_EQUAL_INT(-1, sid);
        TEST_ASSERT_EQUAL_INT(sid, info->sid);
    }

    if (info->available & XFUNCS_AVAIL_BTRACE)
        free(info->backtrace);

    TEST_PASS();
}

TEST_SETUP(xfuncs) {
}

TEST_TEAR_DOWN(xfuncs) {
}

/************ Tests ************/

TEST(xfuncs, verify_xfatal_info_data) {
    xfatal xf;
    xfatal *xf_p = &xf;
    xf_p->handler = xfatal_func;

    errno = 0;
    errno = 10;

    _XFUNCS_SYS_FAIL(xf_p, &_XFUNCS_SITE, test_name, 1);
}

/************ Test runner ************/

TEST_GROUP_RUNNER(xfuncs) {
    RUN_TEST_CASE(xfuncs, verify_xfatal_info_data);
}
