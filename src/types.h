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

// TODO Rethink which string utils will be usefull
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

inline bool is_number(Value value) {
    return value.type == NumberValue;
}
inline bool is_bool(Value val) {
    return val.type == BoolValue;
}
bool is_string(Value value);
StringObj* get_string(Value value);

#endif
