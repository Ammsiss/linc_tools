#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "args.h"

int get_int(char *s) {
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
    if (!opt_arg)
        return;

    data->val_int = get_int(opt_arg);
}

int arg_parse(char * const *argv, opt_data *opts) {
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

    // char *short_opts = calloc(sizeof(char), n + 1);
    // if (!short_opts)
    //     exit(EXIT_FAILURE);


    for (size_t i = 0; i < opt_n; ++i) {
        long_opts[i].name    = opts[i].long_name;
        long_opts[i].val     = 0;
        long_opts[i].has_arg = opts[i].has_arg;
        long_opts[i].flag    = NULL;

        // short_opts[i] = opts[i].short_name;
    }

    while (true) {
        char c = getopt_long_only(argc, argv, "", long_opts, &long_optind);

        if (c == -1) {
            break;

        } else if (c == 0) {
            opts[long_optind].found = true;

            if (opts[long_optind].has_arg == NO_ARG)
                continue;

            switch (opts[long_optind].arg_type) {
            case INT_ARG: parse_opt_arg_int(optarg, &opts[long_optind]); break;
            case STR_ARG: parse_opt_arg_str(optarg, &opts[long_optind]); break;
            }

        } else if (c == '?') {
            goto fail;
        }
    }

    if (optind < argc) {
        fprintf(stderr, "%s: unrecognized options: ", argv[0]);
        while (optind < argc)
            fprintf(stderr, "%s ", argv[optind++]);
        printf("\n");
        goto fail;
    }

    free(long_opts);
    // free(short_opts);

    return 0;

fail:
    free(long_opts);
    // free(short_opts);
    return -1;
}
