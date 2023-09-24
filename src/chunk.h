#ifndef CHUNK_H
#define CHUNK_H

#include "data_structures/arrays.h"
#include "values.h"

// TODO lines

typedef struct {
    OpArr op_codes;
    ValueArr constants;
    LineArr lines;
} Chunk;

void init_chunk(Chunk* chunk);
void write_instruction(Chunk* chunk, uint8_t op_code);
uint8_t add_constant(Chunk* chunk, Value value);
void free_chunk(Chunk* chunk);

void disassemble_chunk(Chunk *chunk, const char* name);

#endif
