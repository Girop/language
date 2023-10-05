#include <stdio.h>
#include <stdlib.h>

#include "common.h"

FileDescriptor read_file(char *filename) {
    FileDescriptor res = {.name = filename, .is_read = false};

    FILE* fp = fopen(filename, "r");
    if (fp == NULL) return res;

    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    rewind(fp);    

    char* buffr = safe_malloc(sizeof(char) * (file_size + 1));
    size_t read_size = fread(buffr, sizeof(char), file_size, fp);
    buffr[file_size] = '\0';
    fclose(fp);

    if (read_size != file_size) return res;

    res.content = buffr;
    res.char_count = file_size + 1;
    res.is_read = true;

    return res;
}
