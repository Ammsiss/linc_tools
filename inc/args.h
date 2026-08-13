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


// TODO: how does a caller know if an optional opt arg was parsed?

#ifndef ARGS_H
#define ARGS_H

#include <stddef.h>

typedef enum {
    INT_ARG,
    STR_ARG,
} opt_arg_type;

typedef enum {
    NO_ARG = 0,
    REQUIRED_ARG,
    OPTIONAL_ARG,
} opt_arg_info;

typedef struct {
    char *long_name;
    char short_name;
    opt_arg_info has_arg;
    opt_arg_type arg_type;
    bool found;
    union {
        int val_int;
        char *val_str;
    };
} opt_data;

int arg_parse(char *const *argv, opt_data *opts);

#endif
