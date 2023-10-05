#include <stdio.h>
#include "../common.h"
#include "hash_table.h"

#define MAX_LOAD_FACTOR 0.8
#define GROW_CAP(old) old * 1.8
#define START_CAPACITY 32

void init_table(HashTable *table) {
    table->capacity = START_CAPACITY;
    table->occupied = 0;
    table->entries = safe_malloc(sizeof(HashEntry) * START_CAPACITY);
    
    for (size_t i = 0; i < START_CAPACITY; i++) {
        table->entries[i].key = NULL;
        table->entries[i].value = from_nil();
    }
}

// Tombstones (dead entries) are marked with
// Key: NULL
// Value: Boolean(true)

static HashEntry* table_find(HashEntry* entries, size_t cap, StringObj* key) {
    size_t index = key->hash % cap;
    HashEntry* first_tombstone = NULL;

    for (;;) {
        HashEntry* res = &entries[index];
        
        if (res->key == NULL) {
           // for sure we need to return 
            if (is_nil(res->value)) {
                return first_tombstone == NULL ? res : first_tombstone;
            } else if (first_tombstone == NULL){ // Tombstone case
                first_tombstone = res;
            }
        } else if (compare_string(key, res->key)) {
            return res; 
        }


        index = (index + 1) % cap;
    }
}

bool table_get(HashTable *table, StringObj *key, Value* value) {
    if (table->occupied == 0) return false;
    HashEntry* entry = table_find(table->entries, table->capacity, key);
    
    if (entry->key == NULL) return false;
    *value = entry->value;
    return true;
}

static void adjust_table(HashTable* table, size_t new_cap) {
    HashEntry* entries = safe_malloc(sizeof(HashEntry) * new_cap);
    size_t new_occupied = 0;

    for (size_t i = 0; i < new_cap; i++) {
        entries->key = NULL;
        entries->value = from_nil();
    }

    for (size_t i = 0; i < table->capacity; i++) {
        HashEntry* entry = &table->entries[i];
        if (entry->key == NULL) continue;
        
        HashEntry* dest = table_find(entries, new_cap, entry->key);
        dest->key = entry->key;        
        dest->value = entry->value;
        new_occupied++;
    }

    // TODO mem leak for strings
    free(table->entries);
    table->entries = entries;
    table->capacity = new_cap;
    table->occupied = new_occupied; 
}

bool table_set(HashTable* table, StringObj* key, Value value) {
    if (table->occupied + 1 > table->capacity * MAX_LOAD_FACTOR) {
        size_t new_cap = GROW_CAP(table->capacity);
        adjust_table(table, new_cap);
    }

    HashEntry* entry = table_find(table->entries, table->capacity, key);
    bool is_new = entry->key == NULL;
    if (is_new && is_nil(entry->value)) table->occupied++;

    entry->key = key;
    entry->value = value; 
    return is_new;
}

bool table_delete(HashTable* table, StringObj* key) {
    if (table->occupied == 0) return false;
    
    HashEntry* entry = table_find(table->entries, table->capacity, key);
    if (entry == NULL) return false;

    entry->key = NULL;
    entry->value = from_bool(true);
    return true;
}
