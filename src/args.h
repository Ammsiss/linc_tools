/*  Define an array of opt_data for the opts you want to parse.

    For each opt_data element, arg_parse sets 'found' to true and one of
    the union members to the corresponding opt arg if, respectively, the
    option was found and the option takes an argument.

    If you define an enum of option names...

    enum opt_names {
        VERBOSE,
        PORT,
        LOGFD,
    };

    opt_data *opts = {
        [VERBOSE] = { ... },
        [PORT] = { ... },
        [LOGFD] = {... }
    };

    ...then you can access options like so:

    if (opts[LOGFD].found)
        logfd = opts[LOGFD].val_int;  */

#ifndef LINC_TOOLS_ARGS_H
#define LINC_TOOLS_ARGS_H

#include <stddef.h>
#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "common.h"

#define ARGS_SILENT 1
#define ARGS_NO_NON_OPTS 2

typedef enum {
    ARGS_INT_ARG,
    ARGS_STR_ARG,
} args_opt_arg_type;

typedef enum {
    ARGS_NO_ARG = 0,
    ARGS_REQUIRED_ARG,
    ARGS_OPTIONAL_ARG,
} args_opt_arg_info;

typedef struct {
    char *long_name;
    char short_name;
    args_opt_arg_info has_arg;
    args_opt_arg_type arg_type;
    bool found;
    bool opt_arg_found;
    union {
        int val_int;
        char *val_str;
    };
} args_opt_data;

__attribute__ ((format(printf, 1, 2)))
static inline void _args_report_err(const char *fmt, ...) {
    if (opterr == 0)
        return;

    va_list va;

    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
}

static inline int _args_get_int(char *s, int *out) {
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

static inline void _args_parse_opt_arg_str(char *opt_arg, args_opt_data *data) {
    if (!opt_arg)
        return;

    data->val_str = opt_arg;
}

static inline int _args_parse_opt_arg_int(char *opt_arg, args_opt_data *data) {
    if (!opt_arg) {
        data->opt_arg_found = false;
        return 0;
    }

    int num;
    if (_args_get_int(opt_arg, &num) == -1)
        return -1;

    data->val_int = num;
    data->opt_arg_found = true;

    return 0;
}

static inline args_opt_data *_args_lookup_opt_by_short_opt(args_opt_data *opts,
        char short_opt)
{
    for (args_opt_data *opt = opts;; ++opt) {
        if (memcmp(opt, &(args_opt_data){0}, sizeof(args_opt_data)) == 0)
            break;

        if (short_opt == opt->short_name)
            return opt;
    }

    return NULL;
}

static inline int args_parse(int argc, char * const *argv, args_opt_data *opts,
        int flags)
{
    assert(argv && opts);

    optind = 0;

    int saved_opterr = opterr;

    if (flags & ARGS_SILENT)
        opterr = 0;

    size_t opt_n = 0;
    for (args_opt_data *opt = opts;; ++opt) {
        if (memcmp(opt, &(args_opt_data){0}, sizeof(args_opt_data)) == 0)
            break;
        ++opt_n;
    }

    int long_optind;

    struct option *long_opts = calloc(opt_n + 1, sizeof(*long_opts));
    if (!long_opts)
        _LINC_LIB_FATAL("calloc: out of memory");

    char *short_opts = calloc(sizeof(char), opt_n * 3 + 1);
    if (!short_opts)
        _LINC_LIB_FATAL("calloc: out of memory");

    for (size_t i = 0, y = 0; i < opt_n; ++i, ++y) {
        long_opts[i].name    = opts[i].long_name;
        long_opts[i].val     = opts[i].short_name;
        long_opts[i].has_arg = opts[i].has_arg;
        long_opts[i].flag    = NULL;

        short_opts[y] = opts[i].short_name;

        if (opts[i].has_arg == ARGS_REQUIRED_ARG)
            short_opts[++y] = ':';
        if (opts[i].has_arg == ARGS_OPTIONAL_ARG) {
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

        args_opt_data *opt = _args_lookup_opt_by_short_opt(opts, c);
        if (!opt) {
            _args_report_err("%s: unexpected opt value: %c\n", argv[0], c);
            goto fail;
        }

        opt->found = true;

        if (opt->has_arg == ARGS_NO_ARG)
            continue;

        switch (opt->arg_type) {
        case ARGS_INT_ARG:
            if (_args_parse_opt_arg_int(optarg, opt) == -1) {
                _args_report_err("%s: bad arg to '--%s' -- '%s'\n",
                        argv[0], opt->long_name, optarg);
                goto fail;
            }
            break;
        case ARGS_STR_ARG:
            _args_parse_opt_arg_str(optarg, opt);
            break;
        }
    }

    if (flags & ARGS_NO_NON_OPTS) {
        if (optind < argc) {
            _args_report_err("%s: unrecognized argv elements: ", argv[0]);
            while (optind < argc)
                _args_report_err("%s ", argv[optind++]);
            _args_report_err("\n");

            goto fail;
        }
    }

    free(long_opts);
    free(short_opts);
    opterr = saved_opterr;

    return 0;

fail:
    free(long_opts);
    free(short_opts);
    opterr = saved_opterr;

    return -1;
}

#endif
