#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdio.h>

#define TABLE_SIZE 100

typedef struct {
    char* key;
    int value;
} HashNode;

typedef struct {
    HashNode** slots;
    int* occupied;
    int size;
} HashTable;

unsigned int hash(const char* key);
HashNode* create_node(const char* key, int value);
HashTable* create_table();
void insert(HashTable* table, const char* key, int value);
int lookup(HashTable* table, const char* key);
void remove_node(HashTable* table, const char* key);
void free_table(HashTable* table);

#endif  // HASHTABLE_H