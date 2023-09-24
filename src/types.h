#ifndef VALUES_LANG_H
#define VALUES_LANG_H
#include <stdbool.h>
#include <stdlib.h>

typedef enum {
    StringObjectType, 
} ObjType;

typedef struct Obj {
    ObjType type; 
} Obj;

typedef struct StringObj StringObj;

struct StringObj {
    ObjType type;
    size_t size;
    char text[];
};

StringObj* create_string(char* value, size_t size);
bool compare_string(StringObj* first, StringObj* second);
StringObj* concat_strings(StringObj* first, StringObj* second);
void free_string(StringObj* target);


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

bool is_string(Value value);
StringObj* get_string(Value value);

Value val_from_num(double number);
Value val_from_bool(bool boolean);
Value val_from_nil(void);

#endif
