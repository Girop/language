#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../types.h"
#include "../common.h"

#define INITIAL_SIZE 8
#define GROW_SIZE(old_size) old_size * 1.4


#define NEW_ARRAY(name, typename) \
typedef struct {                                                                \
    typename* values;                                                         \
    size_t count;                                                               \
    size_t capacity;                                                            \
} name; \
\
static inline void init_##name(name* arr) {                                            \
        arr->count = 0;                                                             \
        arr->capacity = INITIAL_SIZE;                                               \
        arr->values = safe_malloc(arr->capacity * sizeof(typename));                   \
    }\
static inline void add_##name(name* arr, typename element) {                         \
    if ((arr->count + 1) > arr->capacity) {                                     \
        arr->capacity = GROW_SIZE(arr->capacity);                               \
        arr->values = safe_realloc(arr->values, arr->capacity * sizeof(typename)); \
    }                                                                           \
    arr->values[arr->count++] = element;                                        \
}\
static inline void free_##name(name* arr) {    \
    arr->count = 0;                     \
    arr->capacity = 0;                  \
    free(arr->values);                  \
}

NEW_ARRAY(ValueArr, Value)
NEW_ARRAY(OpArr, uint8_t)
NEW_ARRAY(LineArr, unsigned int)

#endif
