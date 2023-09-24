#include <stdio.h>
#include <string.h>

#include "types.h"


bool is_string(Value value) {
    return value.type == ObjValue && value.as.object->type == StringObjectType;
}

StringObj* get_string(Value value) {
    return (StringObj*) value.as.object;
}

Value val_from_num(double number) {
    return (Value) {NumberValue, number};
}

Value val_from_bool(bool boolean) {
    return (Value) {BoolValue, boolean};
}
Value val_from_nil(void) {
    return (Value) {NilValue};
}

// Heap objects stuff goes bellow
StringObj* create_string(char* value, size_t size) {
    StringObj* ptr = malloc(sizeof(StringObj) + sizeof(char) * (size + 1));
    if (ptr == NULL) {
        printf("Memory error\n");
        exit(-1);
    }
    strncpy(ptr->text, value, size);
    ptr->type = StringObjectType;
    ptr->size = size;

    return ptr;
}

// TODO possible bug, idk if StringObj size counts "\0"
StringObj* concat_strings(StringObj* first, StringObj* second) { 
    size_t total_size = first->size + second->size + 1;
    char* tmp = malloc(sizeof(char) * total_size);

    if (tmp == NULL) {
        printf("Memory error\n") ;
        exit(-1);
    }
    *tmp = '\0';

    strcat(tmp, first->text);
    strcat(tmp, second->text);

    StringObj* res = create_string(tmp, total_size);
    free(tmp);

    return res;
}

bool compare_string(StringObj* first, StringObj* second) {
    if (first->size != second->size) return false;
    if (first == second) return true; 

    for (size_t i = 0; i < first->size; i++) {
        if (first->text[i] != second->text[i]) return false;
    }

    return true;
}

void free_string(StringObj *target) {
    free(target); // TODO A(tomatic)RC based memory management
}
