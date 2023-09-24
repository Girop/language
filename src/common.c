#include <stdio.h>
#include <stdlib.h>

const char* read_file(char *filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file %s\n", filename);
        exit(-1);
    }

    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    rewind(fp);    

    char* buffr = malloc(sizeof(char) * (file_size + 1));
    size_t read_size = fread(buffr, sizeof(char), file_size, fp);
    buffr[file_size] = '\0';

    fclose(fp);

    if (read_size != file_size) {
        printf("Error during reading file %s\n", filename);
        exit(-1);
    }

    return buffr;
}
