//
//  hash_table.h
//  ros_xcode
//
//  Created by Eduardo Poleo on 2023-01-23.
//

#ifndef hash_table_h
#define hash_table_h

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

struct Object;

typedef struct HashTableEntry {
    int keyLength;
    char *key;
    struct Object *value;
    struct HashTableEntry *next;
} HashTableEntry;

typedef struct HashTable {
    int num_bins;
    int num_entries;
    HashTableEntry **bins;
    
} HashTable;

#define INITIAL_BINS 10
#define HASH_PRIME 499

HashTable *initHashTable(void);
HashTableEntry *initEntry(char *key, int keyLength, struct Object *value);
void insertEntry(HashTable *table, char *key, int keyLength, struct Object *value);
int hashIndex(char *key, int keyLength, int numBin);
void resolveEntryCollision(HashTableEntry *firstEntry, HashTableEntry *newEntry);
void printObject(struct Object *object);
void printTable(HashTable *table);
struct Object *getEntry(char *key, int keyLength, HashTable *table);

#endif /* hash_table_h */
