#ifndef HASH_TABLE_H
#define HASH_TABLE_H
#include <stdlib.h>
#include "../value.c"

typedef struct {
    const char* string_val;
    int value;
    int hash;
} HashKey;

typedef struct HashEntry {
    HashKey key; 
    Value value;
    struct HashEntry* next;
} HashEntry;

typedef struct {
    size_t size;
    size_t capacity;
    HashEntry** entires;
} HashTable;

void init_table(HashTable* table);
void add_entry(HashTable* table, const char* key, int value);
void free_table(HashTable* table);
#endif
