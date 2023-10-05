#ifndef COMMON_H
#define COMMON_H
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    const char* content;
    const char* name;
    size_t char_count;
    bool is_read;
} FileDescriptor;

FileDescriptor read_file(char *filename);

static void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        printf("Memory error\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

static void* safe_realloc(void* old_ptr, size_t new_size) {
    void* ptr = realloc(old_ptr, new_size);
    if (ptr == NULL) {
        printf("Memory error\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

#endif
