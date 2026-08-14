#include <getopt.h>

#include "unity_fixture.h"
#include "args.h"

TEST_GROUP(args);

/************ Shared utils ************/

int argc;
char *const *argv;
opt_data *opts;
int parse_flags = ARG_SILENT | ARG_NO_NON_OPTS;

#define OPT(_long_name, _short_name, _has_arg, _arg_type) \
    (opt_data){ \
        .long_name = _long_name, \
        .short_name = _short_name, \
        .has_arg = _has_arg, \
        .arg_type = _arg_type \
    }

#define SIMPLE_OPT(_long_name, _short_name) \
    OPT(_long_name, _short_name, NO_ARG, 0)

#define OPTS_ARE(...) \
    opts = (opt_data []){ \
        __VA_ARGS__, \
        (opt_data){0} \
    }

#define ARGV_IS(...) \
    argv = (char *const []){ \
        __VA_ARGS__, \
        NULL \
    }

#define assert_arg_parse() \
    TEST_ASSERT_EQUAL_INT(0, arg_parse(argv, opts, parse_flags));

/************ Fixture ************/

TEST_SETUP(args) {
    argc = 0;
    argv = NULL;
    opts = NULL;
}

TEST_TEAR_DOWN(args) {
    optind = 0; /* reset global getopt state */
}

/************ Tests ************/

TEST(args, parse_simple_opt) {
    ARGV_IS("test", "--debug");
    OPTS_ARE(SIMPLE_OPT("debug", 'd'));

    TEST_ASSERT_EQUAL_INT(0, arg_parse(argv, opts, parse_flags));
}

TEST(args, passing_unexpected_opt_arg_fails) {
    ARGV_IS("test", "--noarg=x");
    OPTS_ARE(SIMPLE_OPT("noarg", 'n'));

    TEST_ASSERT_EQUAL_INT(-1, arg_parse(argv, opts, parse_flags));
}

TEST(args, not_passing_required_arg_fails) {
    ARGV_IS("test", "--required");
    OPTS_ARE(OPT("required", 'r', REQUIRED_ARG, INT_ARG));

    TEST_ASSERT_EQUAL_INT(-1, arg_parse(argv, opts, parse_flags));
}

TEST(args, opt_arg_passes_when_not_supplied) {
    ARGV_IS("test", "--optional");
    OPTS_ARE(OPT("optional", 'r', OPTIONAL_ARG, INT_ARG));

    TEST_ASSERT_EQUAL_INT(0, arg_parse(argv, opts, parse_flags));
}

TEST(args, int_opt_arg_correctly_parsed) {
    ARGV_IS("test", "--add=12");
    OPTS_ARE(OPT("add", 'r', REQUIRED_ARG, INT_ARG));
    assert_arg_parse();

    TEST_ASSERT_EQUAL_INT(12, opts[0].val_int);
}

TEST(args, string_opt_arg_correctly_parsed) {
    ARGV_IS("test", "--username=ammsiss");
    OPTS_ARE(OPT("username", 'u', REQUIRED_ARG, STR_ARG));
    assert_arg_parse();

    TEST_ASSERT_EQUAL_STRING("ammsiss", opts[0].val_str);
}

TEST(args, passing_non_opts_fails) {
    ARGV_IS("test", "--first", "bad", "--last", "bad2");
    OPTS_ARE(SIMPLE_OPT("first", 'f'), SIMPLE_OPT("last", 'l'));

    TEST_ASSERT_EQUAL_INT(-1, arg_parse(argv, opts, parse_flags));
}

TEST(args, passing_short_opt_with_no_arg) {
    ARGV_IS("test", "-s");
    OPTS_ARE(SIMPLE_OPT("silent", 's'));

    assert_arg_parse();

    TEST_ASSERT_TRUE(opts[0].found);
}

TEST(args, passing_short_opt_with_required_arg) {
    ARGV_IS("test", "-c", "main.c");
    OPTS_ARE(OPT("compile", 'c', REQUIRED_ARG, STR_ARG));

    assert_arg_parse();

    TEST_ASSERT_TRUE(opts[0].found);
    TEST_ASSERT_EQUAL_STRING("main.c", opts[0].val_str);
}

TEST(args, passing_short_opt_with_missing_required_arg) {
    ARGV_IS("test", "-c");
    OPTS_ARE(OPT("compile", 'c', REQUIRED_ARG, STR_ARG));

    TEST_ASSERT_EQUAL_INT(-1, arg_parse(argv, opts, parse_flags));
}

TEST(args, passing_short_opt_with_opt_arg) {
    ARGV_IS("test", "-kTERM");
    OPTS_ARE(OPT("kill", 'k', OPTIONAL_ARG, STR_ARG));

    assert_arg_parse();

    TEST_ASSERT_TRUE(opts[0].found);
    TEST_ASSERT_EQUAL_STRING("TERM", opts[0].val_str);
}

TEST(args, passing_short_opt_with_missing_opt_arg) {
    ARGV_IS("test", "-k");
    OPTS_ARE(OPT("kill", 'k', OPTIONAL_ARG, STR_ARG));

    assert_arg_parse();

    TEST_ASSERT_TRUE(opts[0].found);
    TEST_ASSERT_FALSE(opts[0].opt_arg_found);
}

TEST(args, passing_unexpected_opt_fails) {
    ARGV_IS("test", "--bad");
    OPTS_ARE(SIMPLE_OPT("good", 'g'));

    TEST_ASSERT_EQUAL_INT(-1, arg_parse(argv, opts, parse_flags));
}

TEST(args, when_duplicate_opt_last_optarg_is_used) {
    ARGV_IS("test", "--option", "1", "--option", "2");
    OPTS_ARE(
        OPT("option", 'o', REQUIRED_ARG, INT_ARG),
        OPT("option", 'o', REQUIRED_ARG, INT_ARG)
    );

    assert_arg_parse();

    TEST_ASSERT_TRUE(opts[0].found);
    TEST_ASSERT_EQUAL_INT(2, opts[0].val_int);
}

TEST(args, passing_no_opts_when_some_defined_is_ok) {
    ARGV_IS("test");
    OPTS_ARE(SIMPLE_OPT("option", 'o'));

    assert_arg_parse();
}

TEST(args, passing_empty_opt_data_array_is_ok) {
    ARGV_IS("test");
    OPTS_ARE((opt_data){0});

    assert_arg_parse();
}

/************ Test runner ************/

TEST_GROUP_RUNNER(args) {
    RUN_TEST_CASE(args, parse_simple_opt);
    RUN_TEST_CASE(args, passing_unexpected_opt_arg_fails);
    RUN_TEST_CASE(args, not_passing_required_arg_fails);
    RUN_TEST_CASE(args, opt_arg_passes_when_not_supplied);
    RUN_TEST_CASE(args, int_opt_arg_correctly_parsed);
    RUN_TEST_CASE(args, string_opt_arg_correctly_parsed);
    RUN_TEST_CASE(args, passing_non_opts_fails);
    RUN_TEST_CASE(args, passing_short_opt_with_no_arg);
    RUN_TEST_CASE(args, passing_short_opt_with_required_arg);
    RUN_TEST_CASE(args, passing_short_opt_with_missing_required_arg);
    RUN_TEST_CASE(args, passing_short_opt_with_opt_arg);
    RUN_TEST_CASE(args, passing_short_opt_with_missing_opt_arg);
    RUN_TEST_CASE(args, passing_unexpected_opt_fails);
    RUN_TEST_CASE(args, when_duplicate_opt_last_optarg_is_used);
    RUN_TEST_CASE(args, passing_no_opts_when_some_defined_is_ok);
    RUN_TEST_CASE(args, passing_empty_opt_data_array_is_ok);
}
