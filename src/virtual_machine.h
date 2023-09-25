#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H
#include "chunk.h"

typedef enum {
    // Bool only operators 
    OP_NOT,
    OP_OR,
    OP_AND,

    // Bool as result operators
    OP_EQUAL,
    OP_NOT_EQUAL,
    OP_MORE,
    OP_MORE_EQUAL,
    OP_LESS,
    OP_LESS_EQUAL,

    // Number operators
    OP_NEGATE,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,

    // String operators
    OP_CONCAT,
    // OP_EQUAL as well

    // Special
    OP_CONSTANT,
    OP_PRINT,
    OP_RETURN,
} OpCode;

typedef enum {
    INTERPRET_OK,
    INTEPRET_ERROR,
} InterpretResult;

void init_vm(void);
InterpretResult interpret(Chunk* chunk);

#endif
