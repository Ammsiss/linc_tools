#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "args.h"
#include "common.h"

static bool print_errs = true;

__attribute__ ((format(printf, 1, 2)))
static void report_err(const char *fmt, ...) {
    if (!print_errs)
        return;

    va_list va;

    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
}

static int get_int(char *s, int *out) {
    assert(s && out);

    if (*s == '\0')
        return -1;

    char *endptr;
    long num = strtol(s, &endptr, 10);

    if (num < INT_MIN || num > INT_MAX)
        return -1;

    if (*endptr != '\0')
        return -1;

    *out = num;
    return 0;
}

static void parse_opt_arg_str(char *opt_arg, opt_data *data) {
    if (!opt_arg)
        return;

    data->val_str = opt_arg;
}

static int parse_opt_arg_int(char *opt_arg, opt_data *data) {
    if (!opt_arg) {
        data->opt_arg_found = false;
        return 0;
    }

    int num;
    if (get_int(opt_arg, &num) == -1)
        return -1;

    data->val_int = num;
    data->opt_arg_found = true;

    return 0;
}

static opt_data *lookup_opt_by_short_opt(opt_data *opts, char short_opt) {
    for (opt_data *opt = opts;; ++opt) {
        if (memcmp(opt, &(opt_data){0}, sizeof(opt_data)) == 0)
            break;

        if (short_opt == opt->short_name)
            return opt;
    }

    return NULL;
}

int arg_parse(int argc, char * const *argv, opt_data *opts, int flags) {
    assert(argv && opts);

    optind = 0;

    if (flags & ARG_SILENT) {
        opterr = 0;
        print_errs = false;
    }

    size_t opt_n = 0;
    for (opt_data *opt = opts;; ++opt) {
        if (memcmp(opt, &(opt_data){0}, sizeof(opt_data)) == 0)
            break;
        ++opt_n;
    }

    int long_optind;

    struct option *long_opts = calloc(sizeof(opt_data), opt_n + 1);
    if (!long_opts)
        LIB_FATAL("calloc: out of memory");

    char *short_opts = calloc(sizeof(char), opt_n * 3 + 1);
    if (!short_opts)
        LIB_FATAL("calloc: out of memory");

    for (size_t i = 0, y = 0; i < opt_n; ++i, ++y) {
        long_opts[i].name    = opts[i].long_name;
        long_opts[i].val     = opts[i].short_name;
        long_opts[i].has_arg = opts[i].has_arg;
        long_opts[i].flag    = NULL;

        short_opts[y] = opts[i].short_name;

        if (opts[i].has_arg == REQUIRED_ARG)
            short_opts[++y] = ':';
        if (opts[i].has_arg == OPTIONAL_ARG) {
            short_opts[++y] = ':';
            short_opts[++y] = ':';
        }
    }

    while (true) {
        char c = getopt_long(argc, argv, short_opts, long_opts, &long_optind);

        if (c == -1) /* done parsing */
            break;

        if (c == '?') /* unrecognized option or no arg when required */
            goto fail;

        opt_data *opt = lookup_opt_by_short_opt(opts, c);
        if (!opt) {
            report_err("%s: unexpected opt value: %c\n", argv[0], c);
            goto fail;
        }

        opt->found = true;

        if (opt->has_arg == NO_ARG)
            continue;

        switch (opt->arg_type) {
        case INT_ARG:
            if (parse_opt_arg_int(optarg, opt) == -1) {
                report_err("%s: bad arg to '--%s' -- '%s'\n",
                        argv[0], opt->long_name, optarg);
                goto fail;
            }
            break;
        case STR_ARG:
            parse_opt_arg_str(optarg, opt);
            break;
        }
    }

    if (flags & ARG_NO_NON_OPTS) {
        if (optind < argc) {
            report_err("%s: unrecognized argv elements: ", argv[0]);
            while (optind < argc)
                report_err("%s ", argv[optind++]);
            report_err("\n");

            goto fail;
        }
    }

    free(long_opts);
    free(short_opts);

    return 0;

fail:
    free(long_opts);
    free(short_opts);

    return -1;
}
