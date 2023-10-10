#include <stdio.h>
#include <stdlib.h>

#include "virtual_machine.h"
#include "common.h"
#include "chunk.h"
#include "parser.h"
#include "structures/hash_table.h"

// TODO pretty printing improvements
// TODO variable (local and global) assigment

int main(void) {
    FileDescriptor file = read_file("./test.txt");    

    if (!file.is_read) {
        printf("Read failure\n");
        return EXIT_FAILURE;
    }

    Chunk chunk;
    init_chunk(&chunk);

    if (!compile(file.content, &chunk)) {
        printf("Compilation errors occured\n");
        exit(EXIT_FAILURE);
    }

    /* disassemble_chunk(&chunk, "Test:"); */
    /* return EXIT_SUCCESS; */

    init_vm();
    interpret(&chunk);

    free((char*) file.content);
    return EXIT_SUCCESS;
}
