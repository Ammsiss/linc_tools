#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "args.h"

static int get_int(char *s) {
    if (!s || *s == '\0')
        exit(EXIT_FAILURE);

    char *endptr;
    long num = strtol(s, &endptr, 10);

    if (num == LONG_MIN || num == LONG_MAX)
        exit(EXIT_FAILURE);

    if (*endptr != '\0')
        exit(EXIT_FAILURE);

    return num;
}

static void parse_opt_arg_str(char *opt_arg, opt_data *data) {
    if (!opt_arg)
        return;

    data->val_str = opt_arg;
}

static void parse_opt_arg_int(char *opt_arg, opt_data *data) {
    if (!opt_arg) {
        data->opt_arg_found = false;
        return;
    }

    data->val_int = get_int(opt_arg);
    data->opt_arg_found = true;
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

int arg_parse(char * const *argv, opt_data *opts, int flags) {
    if (flags & ARG_SILENT)
        opterr = 0;

    size_t opt_n = 0;
    for (opt_data *opt = opts;; ++opt) {
        if (memcmp(opt, &(opt_data){0}, sizeof(opt_data)) == 0)
            break;
        ++opt_n;
    }

    int argc = 0;
    for (char *const *arg = argv; *arg != NULL; ++arg)
        ++argc;

    int long_optind;

    struct option *long_opts = calloc(sizeof(opt_data), opt_n + 1);
    if (!long_opts)
        exit(EXIT_FAILURE);

    char *short_opts = calloc(sizeof(char), opt_n * 3 + 1);
    if (!short_opts)
        exit(EXIT_FAILURE);

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
            fprintf(stderr, "%s: unexpected opt val: %c\n", argv[0], c);
            goto fail;
        }

        opt->found = true;

        if (opt->has_arg == NO_ARG)
            continue;

        switch (opt->arg_type) {
        case INT_ARG: parse_opt_arg_int(optarg, opt); break;
        case STR_ARG: parse_opt_arg_str(optarg, opt); break;
        }
    }

    if (flags & ARG_NO_NON_OPTS) {
        if (optind < argc) {
            if (!(flags & ARG_SILENT)) {
                fprintf(stderr, "%s: unrecognized argv elements: ", argv[0]);
                while (optind < argc)
                    fprintf(stderr, "%s ", argv[optind++]);
                printf("\n");
            }

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
