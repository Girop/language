#ifndef VALUES_LANG_H
#define VALUES_LANG_H
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum {
    StringObjectType,
} ObjType;

typedef struct Obj {
    ObjType type; 
    uint16_t rc;
} Obj;

typedef struct {
    ObjType object_type;
    uint16_t rc;

    uint32_t hash;
    size_t size;
    char text[];
} StringObj;

typedef enum {
    NumberValue,
    BoolValue,
    ObjValue,
    NilValue,
} ValueType;

typedef struct {
    ValueType type; 
    union {
        bool boolean;
        double number;
        Obj* object;
    } as;
} Value;

bool is_truthy(Value val);
bool is_falsey(Value val);

Value from_number(double number);
Value from_string(StringObj* string);
Value from_bool(bool boolean);
Value from_nil(void);

bool is_number(Value value);
bool is_bool(Value val);
bool is_string(Value value);
bool is_nil(Value value);

double get_number(Value value);
bool get_bool(Value value);
StringObj* get_string(Value value);

StringObj* create_string(const char* source, size_t size);
bool compare_string(StringObj* first, StringObj* second);
StringObj* concat_strings(StringObj* first, StringObj* second);

void print_value(Value val);
#endif
