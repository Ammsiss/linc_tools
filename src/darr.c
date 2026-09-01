#include <assert.h>
#include <string.h>
#include <sys/types.h>

#include "darr.h"
#include "common.h"

#if __has_include("darr_type_defs.h")
    #include "darr_type_defs.h"
#endif

#define DEFINE_DYN_ARR(name, type) \
    void name##_init(name *arr) { \
        assert(arr); \
        \
        *arr = (name){0}; \
    } \
    \
    void name##_free(name *arr) { \
        assert(arr); \
        \
        free(arr->data); \
        *arr = (name){0}; \
    } \
    \
    void name##_reserve(name *arr, size_t min) { \
        assert(arr); \
        assert(arr->size <= arr->cap); \
        \
        if (min == 0 || min <= arr->cap) \
            return; \
        \
        void *tmp = realloc(arr->data, min * sizeof(*arr->data)); \
        if (!tmp) \
            LIB_FATAL("realloc: out of memory"); \
        \
        arr->data = tmp; \
        arr->cap = min; \
    } \
    \
    type *name##_push(name *arr) { \
        assert(arr); \
        \
        name##_reserve(arr, arr->size + 1); \
        \
        ++arr->size; \
        \
        type *p = &arr->data[arr->size - 1]; \
        \
        return p; \
    } \
    \
    void name##_delete(name *arr, size_t remove_i) { \
        assert(arr); \
        assert(remove_i < arr->size); \
        assert(remove_i >= 0); \
        \
        for (size_t i = remove_i + 1; i < arr->size; ++i) \
            arr->data[i - 1] = arr->data[i]; \
        \
        arr->size -= 1; \
    } \
    \
    type *name##_insert(name *arr, size_t insert_i) { \
        assert(arr); \
        assert(insert_i < arr->size); \
        assert(insert_i >= 0); \
        \
        name##_reserve(arr, arr->size + 1); \
        \
        for (size_t i = arr->size - 2; i >= insert_i; --i) { \
            arr->data[i + 1] = arr->data[i]; \
            if (i == insert_i) \
                return &arr->data[i]; \
        } \
        \
        LIB_FATAL("unexpected insertion index"); \
    }

#define DA_DEFINE(_, name, type) DEFINE_DYN_ARR(name, type)
    DYN_ARR_TYPES(DA_DEFINE, _)
#undef X
