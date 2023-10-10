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

static void pprint_value_arg(Value val) {
    switch (val.type) {
        case NumberValue:   printf("\t%f", val.as.number); break;
        case BoolValue:     printf("\t%b", val.as.boolean); break;
        case NilValue:      printf("\tnil"); break;
        case ObjValue:
            if (is_string(val)) {
                char* str_val = ((StringObj*) val.as.object)->text;
                printf("\t%s", str_val);
                break;
            }
        default:
            printf("Unkown value type!\n");
            exit(EXIT_FAILURE);
    }
}


static size_t data_instruction(const char* name, size_t offset, int arg_count, Chunk* chunk) {
    printf("%04zu\t%s", offset, name); 
    for (int i = 1; i <= arg_count; i++) {
        uint8_t addrs = chunk->op_codes.values[offset + i];
        Value val = chunk->constants.values[addrs];
        pprint_value_arg(val);
    }

    printf("\n");
    return 1 + arg_count;
}


// TODO line information
// TODO use value string representation
void disassemble_chunk(Chunk *chunk, const char* name) {

    printf("Chunk: %s\n", name);
    printf("Chunk size: %zu\n", chunk->op_codes.count);

    for (size_t i = 0; i < chunk->op_codes.count;) {
        switch (chunk->op_codes.values[i]) {
            case OP_CONSTANT: i += data_instruction("OP_CONSTANT", i, 1, chunk); break; 
            case OP_NEGATE:   i += simple_instruction("OP_NEGATE", i); break;
            case OP_ADD:      i += simple_instruction("OP_ADD", i); break;
            case OP_SUB:   i += simple_instruction("OP_SUB", i); break;
            case OP_MUL:   i += simple_instruction("OP_MUL", i); break;
            case OP_DIV:   i += simple_instruction("OP_DIV", i); break;
            case OP_RETURN:   i += simple_instruction("OP_RET", i); break;
            case OP_MOD: i += simple_instruction("OP_MOD", i); break;
            case OP_EQUAL:   i += simple_instruction("OP_EQUAL", i); break;
            case OP_NOT_EQUAL:   i += simple_instruction("OP_NOT_EQUAL", i); break;
            case OP_CONCAT: i += simple_instruction("OP_CONCAT", i); break;
            case OP_PRINT: i += simple_instruction("OP_PRINT", i); break;
            case OP_NEW_GLOBAL: i += data_instruction("OP_NEW_GLOBAL", i, 1, chunk); break;
            case OP_GET_GLOBAL: i += data_instruction("OP_GET_GLOBAL", i, 1, chunk); break;
            default:
                printf("Unkown instruction!\n");
                exit(EXIT_FAILURE);
        }
    }
}
