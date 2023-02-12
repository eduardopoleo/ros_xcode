//
//  hash_table.c
//  ros_xcode
//
//  Created by Eduardo Poleo on 2023-01-23.
//

#include "hash_table.h"
#include "object.h"

HashTable *initHashTable(void) {
    HashTable *table = malloc(sizeof(HashTable));
    table->num_bins = INITIAL_BINS;
    table->num_entries = 0;
    table->bins = calloc(table->num_bins, sizeof(HashTableEntry));
    return table;
}

HashTableEntry *initEntry(char *key, int keyLength, Object *value) {
    HashTableEntry *entry = malloc(sizeof(HashTableEntry));
    entry->key = key;
    entry->keyLength = keyLength;
    entry->value = value;
    
    return entry;
}

void insertEntry(HashTable *table, char *key, int keyLength, Object *value) {
    HashTableEntry *newEntry = initEntry(key, keyLength, value);

    int bucketIndex = hashIndex(key, keyLength, table->num_bins);
    HashTableEntry *currentEntry = table->bins[bucketIndex];

    if (currentEntry == NULL) {
        table->bins[bucketIndex] = newEntry;
    } else {
        resolveEntryCollision(currentEntry, newEntry);
    }

    table->num_entries++;
}

int hashIndex(char *key, int keyLength, int numBin) {
    long hash = 0;

    for(int i = 0; i < keyLength; i++) {
        hash += pow(HASH_PRIME, keyLength - (i+1)) * (int)key[i];
    }

    return hash % numBin;
}

void resolveEntryCollision(HashTableEntry *currentEntry, HashTableEntry *newEntry) {
    HashTableEntry *current = currentEntry;
    HashTableEntry *prev;
    while(current != NULL) {
        if(strncmp(current->key, newEntry->key, current->keyLength) == 0) {
            current->value = newEntry->value;
            return;
        }
        prev = current;
        current = current->next;
    }

    prev->next = newEntry;
}

void printTable(HashTable *table) {
    HashTableEntry *entry;
    for(int i=0; i < table->num_bins; i++) {
        entry = table->bins[i];
        while(entry != NULL) {
            printf("bin # %d", i);
            printObject(entry->value);
            entry = entry->next;
        }
    }
}

void printObject(Object *object) {
    switch(object->type) {
        case NUMBER_OBJ:
            printf("  %f\n", object->as.number.value);
        case STRING_OBJ:
            printf("  %*.*s\n",
                (int)object->as.string.length,
                (int)object->as.string.length,
                object->as.string.value
            );
    }
}

Object *getEntry(char *key, int keyLength, HashTable *table) {
    int bucketIndex = hashIndex(key, keyLength, table->num_bins);
    
    HashTableEntry *current = table->bins[bucketIndex];

    if (current == NULL) {
        printf("Value not found in hash table\n");
        exit(1);
    }

    while(strncmp(key, current->key, keyLength) != 0) {
        
        current = current->next;
        
        if (current == NULL) {
            printf("Value not found in hash table\n");
            exit(1);
        }
    }

    return current->value;
}
