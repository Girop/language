#include <stdio.h>
#include "hash_table.h"


#define INITAL_SIZE 8
void init_table(HashTable* table) {
    table->size = 0;
    table->capacity = INITAL_SIZE;
    HashEntry** ptr = malloc(sizeof(HashTable*) * table->capacity);
    
    if (ptr == NULL) {
        printf("Memory error\n");
        exit(-1);
    }
    table->entires = ptr;
}

static size_t hash_function(const char* key);

// TODO verify
void add_entry(HashTable* table, const char* key, int value) {
    size_t addr = hash_function(key);

    HashEntry* new_entry = malloc(sizeof(HashTable));
    if (new_entry == NULL) {
        printf("Memory error\n");
        exit(-1);
    }

    *new_entry = (HashEntry) {key, value, NULL};
    HashEntry* found_entry = table->entires[addr];

    if (found_entry == NULL) {
        table->entires[addr] = new_entry;
        return;
    }

    for (;;) {
        found_entry = found_entry->next;
        if (found_entry->next == NULL) break;
    }
    found_entry->next = new_entry;
}

void free_table(HashTable* table) {
    table->entires = 0;
    table->capacity = 0;
    
    for (size_t i = 0; i < table->size; i++) {
        HashEntry* current_entry = table->entires[i];
        while (current_entry != NULL) {
            HashEntry* next_entry = current_entry->next;
            free(current_entry) ;
            current_entry = next_entry;
        }
    }

    free(table->entires);
}
