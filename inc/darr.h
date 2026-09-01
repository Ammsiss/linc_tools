#ifndef DYN_ARR_H
#define DYN_ARR_H

#include <stdlib.h>

/* Declare any types that you APPLY...
 *
 * eg) // darr_type_decs.h //
 *     struct mystruct;
 *     ...
 *     APPLY(arg, da_mystruct, struct mystruct)
 *
 * ...then include the full type definitions.
 *
 * eg) // darr_type_defs.h //
 *     #include "mystruct.h" */

#if __has_include("darr_type_decs.h")
    #include "darr_type_decs.h"
#else
    #define DYN_ARR_TYPES(APPLY, arg) \
        APPLY(arg, da_int, int)
#endif

#define DECLARE_DYN_ARR(name, type) \
    typedef struct { \
        type *data; \
        size_t size; \
        size_t cap; \
    } name; \
    void name##_init(name *arr);\
    void name##_free(name *arr); \
    type *name##_push(name *arr); \
    void name##_reserve(name *arr, size_t min); \
    void name##_delete(name *arr, size_t remove_i); \
    type *name##_insert(name *arr, size_t insert_i);

#define DA_DECLARE(_, name, type) DECLARE_DYN_ARR(name, type)
    DYN_ARR_TYPES(DA_DECLARE, _)
#undef DA_DECLARE

#define DA_GENERIC_CASE(arg, name, type) , name: name##arg

#define DA_GET(suffix, arr) \
    _Generic(*(arr) DYN_ARR_TYPES(DA_GENERIC_CASE, suffix))

#define da_init(arr) \
    DA_GET(_init, (arr))(arr)

#define da_free(arr) \
    DA_GET(_free, (arr))(arr)

#define da_push(arr) \
    ({ \
        assert(arr); \
        DA_GET(_push, (arr))(arr); \
    })

#define da_reserve(arr, size) \
    DA_GET(_reserve, (arr))(arr, size)

#define da_delete(arr, index) \
    DA_GET(_delete, (arr))(arr, index)

/* Push an element and then initialize it */
#define da_push_init(arr, init) \
    ({ \
        void *_p = da_push(arr); \
        if (_p && (init)(_p) == -1) \
            _p = NULL; \
        _p; \
    })

#endif
