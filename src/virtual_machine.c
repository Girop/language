#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "virtual_machine.h"
#include "types.h"
#include "structures/hash_table.h"

#define STACK_SIZE 1024

typedef struct VirtualMachine VirtualMachine;
struct VirtualMachine{ 
    Chunk* chunk;
    uint8_t* ip; 
    Value* stacktop; 
    Value value_stack[STACK_SIZE];
    HashTable globals;
} machine;

void init_vm(void) {
    machine.chunk = NULL;
    machine.stacktop = machine.value_stack;
    init_table(&machine.globals);
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
    exit(EXIT_FAILURE); // TODO this can be done better
}

#define READ_BYTE (*(machine.ip++)) 
#define READ_CONSTANT (machine.chunk->constants.values[READ_BYTE])



#define CREATE_NUM_VAL(num) (Value) {.type = NumberValue, .as.number = (num)}
#define CREATE_BOOL_VAL(boo) (Value) {.type = BoolValue, .as.boolean = (boo)}
#define CREATE_OBJ_VAL(str) (Value) {.type = ObjValue, .as.object = ((Obj*) (str))}

static bool compare_string_values(Value a, Value b) { 
    return compare_string((StringObj*) a.as.object, (StringObj*) b.as.object);
}

#define BINARY_NUMBER_OPERATION(operator) \
    do { \
    Value b = pop_stack(); \
    Value a = pop_stack(); \
                          \
    if (!(is_number(a) && is_number(b))) { \
        runtime_error("Both operands are required to be numbers"); \
        return INTEPRET_ERROR;\
    }  \
    push_stack(CREATE_NUM_VAL(a.as.number operator b.as.number) ); \
} while (0)


#define BINARY_BOOLEAN_OPEARTOR(operator) \
    do { \
        Value b = pop_stack(); \
        Value a = pop_stack();\
        \
        if (!(is_bool(a) && is_bool(b))) { \
            runtime_error("Both operands are required to be booleans"); \
            return INTEPRET_ERROR; \
        }\
        push_stack(CREATE_BOOL_VAL(a.as.boolean operator b.as.boolean));\
    } while (0)

#define COMPARISON_OPERATOR(oper) \
    do { \
        Value b = pop_stack(); \
        Value a = pop_stack(); \
        if (a.type != b.type) { \
            runtime_error("Can't compare different types"); \
            return INTEPRET_ERROR; \
        } \
        push_stack(CREATE_BOOL_VAL(a oper b)); \
    } while (0)

#define NUMBER_COMPARSION(operator) \
    do {\
        Value b = pop_stack(); \
        Value a = pop_stack(); \
        if (!(is_number(a) && is_number(b))) { \
            runtime_error("Comparison of this type can only be made on numbers");\
            return INTEPRET_ERROR; \
        }\
        push_stack(CREATE_BOOL_VAL(a.as.number operator b.as.number)); \
    } while (0)

static InterpretResult run(void) {
    for (;;) {
        switch (READ_BYTE) {
            case OP_OR: BINARY_BOOLEAN_OPEARTOR(||); break;
            case OP_AND: BINARY_BOOLEAN_OPEARTOR(&&); break;
            case OP_NOT: {
                Value a = pop_stack();
                if (!is_number(a)) {
                    runtime_error("Can't negate anything that isn't number");
                }
                push_stack(CREATE_NUM_VAL(a.as.number));
                break;
            }
            case OP_EQUAL: {
                Value b = pop_stack();
                Value a = pop_stack();
                if (a.type != b.type) {
                    push_stack(CREATE_BOOL_VAL(false));
                    break;
                }

                if (is_string(a)) {
                    push_stack(CREATE_BOOL_VAL(compare_string_values(a, b)));
                    break;
                } else if (is_number(a)) {
                    push_stack(CREATE_BOOL_VAL(a.as.number == b.as.number));
                    break;    
                } else if (is_bool(a)) {
                    push_stack(CREATE_BOOL_VAL(a.as.boolean == b.as.boolean));
                    break;    
                }
                runtime_error("Can't compare unkown operand");
                return INTEPRET_ERROR;
            }
            case OP_NOT_EQUAL: {
                Value b = pop_stack();
                Value a = pop_stack();
                if (a.type != b.type) {
                    push_stack(CREATE_BOOL_VAL(true));
                    break;
                }

                if (is_string(a)) {
                    push_stack(CREATE_BOOL_VAL(!compare_string_values(a, b)));
                    break;
                } else if (is_number(a)) {
                    push_stack(CREATE_BOOL_VAL(a.as.number != b.as.number));
                    break;    
                } else if (is_bool(a)) {
                    push_stack(CREATE_BOOL_VAL(a.as.boolean != b.as.boolean));
                    break;    
                }

                runtime_error("Can't compare unkown operand");
                return INTEPRET_ERROR;
            }
            case OP_MORE: NUMBER_COMPARSION(>); break;
            case OP_MORE_EQUAL: NUMBER_COMPARSION(>=); break;
            case OP_LESS: NUMBER_COMPARSION(<); break;
            case OP_LESS_EQUAL: NUMBER_COMPARSION(<=); break;

            case OP_NEGATE: {
                Value a = pop_stack();
                if (!is_number(a)) {
                    runtime_error("Can negate only numbers ");
                    return INTEPRET_ERROR;
                }
                push_stack(CREATE_BOOL_VAL(-a.as.boolean));
                break;
            } 
            case OP_ADD: BINARY_NUMBER_OPERATION(+); break;
            case OP_SUB: BINARY_NUMBER_OPERATION(-); break;
            case OP_MUL: BINARY_NUMBER_OPERATION(*); break;
            case OP_DIV: BINARY_NUMBER_OPERATION(/); break;
            case OP_MOD: {
                Value b = pop_stack(); 
                Value a = pop_stack(); 
                    
                if (a.type != NumberValue || b.type != NumberValue) {
                    runtime_error("Both operands are required to be numbers");
                    return INTEPRET_ERROR;
                }
                long long int res = ((long long int) a.as.number) % ((long long int) b.as.number);
                push_stack(CREATE_NUM_VAL(res));
                break;
            }
            case OP_CONCAT: {
                Value b = pop_stack();
                Value a = pop_stack();
                if (!(is_string(a) && is_string(b))) {
                    runtime_error("Can concat only strings");
                    return INTEPRET_ERROR;
                }
    
                StringObj* obj = concat_strings((StringObj*) a.as.object, (StringObj*) b.as.object);
                push_stack(CREATE_OBJ_VAL(obj));
                break;
            }
            case OP_CONSTANT: push_stack(READ_CONSTANT); break;
            case OP_PRINT: print_value(pop_stack()); break;
            case OP_POP: pop_stack(); break;
            case OP_NEW_GLOBAL: {
                StringObj* name = get_string(READ_CONSTANT);
                table_set(&machine.globals, name, pop_stack());
                break;
            }
            case OP_GET_GLOBAL: {
                StringObj* name = get_string(READ_CONSTANT);
                Value result;
                if (!table_get(&machine.globals, name, &result)) {
                    printf("Undefined global\n"); 
                    return INTEPRET_ERROR;
                }
                push_stack(result);
                break;
            }
            case OP_RETURN: return INTERPRET_OK;
        }
    }
}

InterpretResult interpret(Chunk* chunk) {
    machine.chunk = chunk;
    machine.ip = chunk->op_codes.values;
    return run();
}
