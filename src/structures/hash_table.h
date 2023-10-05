#ifndef HASH_TABLE_H
#define HASH_TABLE_H
#include "../types.h"

typedef struct {
    StringObj* key;
    Value value;
} HashEntry;

typedef struct {
    size_t occupied;
    size_t capacity;
    HashEntry* entries;
} HashTable;

void init_table(HashTable* table);

bool table_set(HashTable* table, StringObj* key, Value value);
bool table_get(HashTable* table, StringObj* key, Value* value);
bool table_delete(HashTable* table, StringObj* key);

void free_table(HashTable* table);

#endif
