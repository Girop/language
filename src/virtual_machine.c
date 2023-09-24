#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "virtual_machine.h"
#include "types.h"

#define STACK_SIZE 512
typedef struct { 
    Chunk* chunk;
    uint8_t* ip; 
    Value* stacktop; 
    Value value_stack[STACK_SIZE];
} VirtualMachine;
VirtualMachine machine;

void init_vm(void) {
    machine.stacktop = machine.value_stack;
}

static Value pop_stack(void) {
    return *(--machine.stacktop);
}

static void push_stack(Value val) {
    *(machine.stacktop++) = val;
}


static void runtime_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = machine.ip - machine.chunk->op_codes.values - 1;
    int line = machine.chunk->lines.values[instruction];
    
    fprintf(stderr, "[line %d] in script\n", line);
    exit(-1); // TODO this can be done better
}

#define READ_BYTE (*(machine.ip++)) 
#define READ_CONSTANT (machine.chunk->constants.values[READ_BYTE])

#define bin_num_operand(operator) \
    do { \
    Value b = pop_stack(); \
    Value a = pop_stack(); \
                          \
    if (a.type != NumberValue || b.type != NumberValue) { \
        runtime_error("both operands are required to be numbers"); \
    }  \
    push_stack(val_from_num(a.as.number operator b.as.number) ); \
} while (0)


static void modulo_operand(void) {
    Value b = pop_stack(); 
    Value a = pop_stack(); 
        
    if (a.type != NumberValue || b.type != NumberValue) {
        runtime_error("both operands are required to be numbers");
    }
    long long int res = ((long long int) a.as.number) % ((long long int) b.as.number);
    push_stack(val_from_num((double) res));
}

static void num_bool_binary(void) {
    Value b = pop_stack();
    Value a = pop_stack();

    if (a.type != b.type) {
        push_stack(val_from_bool(false));
        return;
    }
     
}

// TODO finish other instructions
static InterpretResult run(void) {
    for (;;) {
        switch (READ_BYTE) {
            case OP_ADD: bin_num_operand(+); break;
            case OP_SUB: bin_num_operand(-); break;
            case OP_MUL: bin_num_operand(*); break;
            case OP_DIV: bin_num_operand(/); break;
            case OP_MOD: modulo_operand(); break;
            
        }
    }
}


/* static InterpretResult run(void) { */
/*     for (;;) {     */
/*         switch (READ_BYTE) { */
/*             case OP_CONSTANT:   push_stack(READ_CONSTANT);      break; */
/*             case OP_NEGATE:     push_stack(-pop_stack());   break; */
/*             case OP_ADD:        BINARY_INSTRUCTION(+);      break; */
/*             case OP_SUB:        BINARY_INSTRUCTION(-);      break; */
/*             case OP_MUL:        BINARY_INSTRUCTION(*);      break; */
/*             case OP_DIV:        BINARY_INSTRUCTION(/);      break; */
/*             case OP_MOD:        mod_operator();      break; */
/*             case OP_NOT:              push_stack(!pop_stack()); break; */
/*             case OP_EQUAL:            BINARY_INSTRUCTION(==);      break; */
/*             case OP_NOT_EQUAL:        BINARY_INSTRUCTION(!=);      break; */
/*             case OP_LESS:           BINARY_INSTRUCTION(<);      break; */
/*             case OP_LESS_EQUAL: BINARY_INSTRUCTION(<=);      break; */
/*             case OP_MORE:       BINARY_INSTRUCTION(>);      break; */
/*             case OP_MORE_EQUAL:     BINARY_INSTRUCTION(>=);      break; */
/*             case OP_AND:            BINARY_INSTRUCTION(&&);      break; */
/*             case OP_OR:            BINARY_INSTRUCTION(||);      break; */
/*             case OP_PRINT:           */
/*             case OP_RETURN:     return INTERPRET_OK; */
/*             default: */
/*                 printf("VM encounterd invalid instruction!\n"); */
/*                 exit(-1); */
/*         }  */
/*     } */
/* } */

InterpretResult interpret(Chunk* chunk) {
    machine.chunk = chunk;
    machine.ip = chunk->op_codes.values;
    return run();
}
