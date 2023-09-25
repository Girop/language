#include <stdio.h>
#include <stdlib.h>

#include "virtual_machine.h"
#include "common.h"
#include "chunk.h"
#include "parser.h"

int main(void) {
    //const char* source = read_file("./test.txt");
    // const char* source = "1 + 10 * ((10 + 1) + 2) + 3";
    const char* source = "1 + 2";
    Chunk chunk;
    init_chunk(&chunk);

    if (!compile(source, &chunk)) {
        printf("Compilation error occured\n");
        exit(-1);
    }

    disassemble_chunk(&chunk, "Test:");
    // deb_last_value();
    return -1;

    init_vm();
    interpret(&chunk);

    free((char*) source);
    printf("Finished\n");
}
