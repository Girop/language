#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../types.h"

#define INITIAL_SIZE 8
#define GROW_SIZE(old_size) old_size * 1.4

#define ARRAY_SIGNATURES(name, value_type) \
typedef struct {                                                                \
    value_type* values;                                                         \
    size_t count;                                                               \
    size_t capacity;                                                            \
} name; \
void init_##name(name* arr);\
void add_##name(name* arr, value_type element);\
void free_##name(name* arr);

#define ARRAY_IMPL(name, value_type) \
void init_##name(name* arr) {                                            \
        arr->count = 0;                                                             \
        arr->capacity = INITIAL_SIZE;                                               \
        arr->values = malloc(arr->capacity * sizeof(value_type));                   \
        if (arr->values == NULL) {                                                  \
            printf("Memory error, exiting");                                        \
            exit(-1);                                                               \
        }                                                                           \
    }\
void add_##name(name* arr, value_type element) {                         \
    if ((arr->count + 1) > arr->capacity) {                                     \
        arr->capacity = GROW_SIZE(arr->capacity);                               \
        arr->values = realloc(arr->values, arr->capacity * sizeof(value_type)); \
        if (arr->values == NULL) {                                              \
            printf("Memory error, exiting");                                    \
            exit(-1);                                                           \
        }                                                                       \
    }                                                                           \
    arr->values[arr->count++] = element;                                        \
}\
void free_##name(name* arr) {    \
    arr->count = 0;                     \
    arr->capacity = 0;                  \
    free(arr->values);                  \
}

ARRAY_SIGNATURES(ValueArr, Value)
ARRAY_SIGNATURES(OpArr, uint8_t)
ARRAY_SIGNATURES(LineArr, unsigned int)

#endif
