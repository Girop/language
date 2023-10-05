#include <stdio.h>
#include <string.h>

#include "common.h"
#include "types.h"

// Assumes input is correct type
bool is_truthy(Value val) {
    return !is_nil(val) || get_bool(val); 
}

bool is_falsey(Value val) {
    return is_nil(val) || !get_bool(val);
}

Value from_number(double number) {
    return (Value) {.type = NumberValue, .as.number = number};
}

Value from_string(StringObj* string) {
    return (Value) {.type = ObjValue, .as.object = (Obj*) string};
}

Value from_bool(bool boolean) {
    return (Value) {.type = BoolValue, .as.boolean = boolean};    
}

Value from_nil(void) {
    return (Value) {.type = NilValue};
}

bool is_number(Value value){
    return value.type == NumberValue;
}

bool is_bool(Value value){
    return value.type == BoolValue;
}

bool is_nil(Value value) {
    return value.type == NilValue;
}

bool is_string(Value value) {
    return value.type == ObjValue && value.as.object->type == StringObjectType;
}

double get_number(Value value) {
    return value.as.number;
}

bool get_bool(Value value) {
    return value.as.boolean;
}

StringObj* get_string(Value value) {
    return (StringObj*) value.as.object;
}

// Using FNV-1a for hashing
uint32_t hash_string(const char* key, size_t length) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t) key[i];
        hash *= 16777619;
    }
    return hash;
}

StringObj* create_string(const char* source, size_t size) {
    StringObj* ptr = safe_malloc(sizeof(StringObj) + sizeof(char) * (size + 1));

    memcpy(ptr->text, source, size);
    ptr->text[size] = '\0';
    ptr->object_type = StringObjectType;
    ptr->size = size;
    ptr->hash = hash_string(source, size);

    return ptr;
}

StringObj* concat_strings(StringObj* first, StringObj* second) { 
    size_t total_size = first->size + second->size - 1;
    StringObj* res = safe_malloc(sizeof(StringObj) + sizeof(char) * total_size);
    
    res->text[0] = '\0';
    strcat(res->text, first->text);
    strcat(res->text, second->text);

    res->object_type = StringObjectType;
    res->size = total_size;
    return res;
}

bool compare_string(StringObj* first, StringObj* second) {
    if (first->size != second->size || first->hash != second->hash) return false;
    if (first == second) return true; 

    for (size_t i = 0; i < first->size; i++) {
        if (first->text[i] != second->text[i]) return false;
    }

    return true;
}

void print_value(Value val) {
    switch (val.type) {
        case NumberValue:
            printf("Number<%f>\n", val.as.number);
            break;
        case NilValue:
            printf("Nil\n");
            break;
        case BoolValue:
            printf("Bool<%s>\n", val.as.boolean ? "true" : "false");
            break;
        case ObjValue: {
           if (is_string(val)) {
                printf("String<%s>\n", ((StringObj*) val.as.object)->text);
            } else {
                printf("Object<TODO>\n");
            }
            break;
        }
    }
}
