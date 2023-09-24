#include "chunk.h"
#include "virtual_machine.h"

void init_chunk(Chunk *chunk) {
    init_OpArr(&chunk->op_codes);
    init_ValueArr(&chunk->constants);
}

void write_instruction(Chunk* chunk, uint8_t op_code) {
    add_OpArr(&chunk->op_codes, op_code);
}

// Returns address of consant
// NOTE: due to OP_code size of 1 byte, max number of constants per chunks is limited to 255
uint8_t add_constant(Chunk *chunk, Value value) {
    add_ValueArr(&chunk->constants, value);
    return chunk->constants.count - 1;
}

void free_chunk(Chunk* chunk) {
    free_OpArr(&chunk->op_codes);
    free_ValueArr(&chunk->constants);
}

static size_t simple_instruction(const char* name, size_t offset) {
    printf("%04zu\t%s\n", offset, name); 
    return 1;
}

static size_t data_instruction(const char* name, size_t offset, Chunk* chunk) {
    uint8_t addrs = chunk->op_codes.values[offset + 1];
    Value val = chunk->constants.values[addrs];
    printf("%04zu\t%s\t%f\n", offset, name, val); 
    return 2;
}

// TODO line information
void disassemble_chunk(Chunk *chunk, const char* name) {

    printf("Chunk: %s\n", name);
    printf("Chunk size: %zu\n", chunk->op_codes.count);

    for (size_t i = 0; i < chunk->op_codes.count;) {
        switch (chunk->op_codes.values[i]) {
            case OP_CONSTANT: i += data_instruction("OP_CONSTANT", i, chunk); break; 
            case OP_NEGATE:   i += simple_instruction("OP_NEGATE", i); break;
            case OP_ADD:      i += simple_instruction("OP_ADD", i); break;
            case OP_SUB:   i += simple_instruction("OP_SUB", i); break;
            case OP_MUL:   i += simple_instruction("OP_MUL", i); break;
            case OP_DIV:   i += simple_instruction("OP_DIV", i); break;
            case OP_RETURN:   i += simple_instruction("OP_RET", i); break;
            case OP_EQUAL:   i += simple_instruction("OP_EQUAL", i); break;
            case OP_NOT_EQUAL:   i += simple_instruction("OP_NOT_EQUAL", i); break;
            default:
                printf("Unkown instruction!\n");
                exit(-1);
        }
    }
}
