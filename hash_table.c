#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"



unsigned int hash(const char* key) {
    unsigned int hash_val = 0;
    unsigned int prime = 31;

    while (*key != '\0') {
        hash_val = hash_val * prime + *key;
        key++;
    }

    return hash_val % TABLE_SIZE;
}

HashNode* create_node(const char* key, int value) {
    HashNode* node = (HashNode*)malloc(sizeof(HashNode));
    node->key = strdup(key);
    node->value = value;
    return node;
}

HashTable* create_table() {
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    table->slots = (HashNode**)malloc(TABLE_SIZE * sizeof(HashNode*));
    table->occupied = (int*)calloc(TABLE_SIZE, sizeof(int));
    table->size = 0;
    return table;
}

void insert(HashTable* table, const char* key, int value) {
    if (table->size >= TABLE_SIZE) {
        printf("Hash table is full.\n");
        return;
    }

    unsigned int index = hash(key);

    while (table->occupied[index]) {
        index = (index + 1) % TABLE_SIZE;
    }

    HashNode* node = create_node(key, value);
    table->slots[index] = node;
    table->occupied[index] = 1;
    table->size++;
}

int lookup(HashTable* table, const char* key) {
    unsigned int index = hash(key);

    while (table->occupied[index]) {
        if (strcmp(table->slots[index]->key, key) == 0) {
            return table->slots[index]->value;
        }
        index = (index + 1) % TABLE_SIZE;
    }

    return -1;
}

void remove_node(HashTable* table, const char* key) {
    unsigned int index = hash(key);

    while (table->occupied[index]) {
        if (strcmp(table->slots[index]->key, key) == 0) {
            free(table->slots[index]->key);
            free(table->slots[index]);
            table->occupied[index] = 0;
            table->size--;
            return;
        }
        index = (index + 1) % TABLE_SIZE;
    }
}

void free_table(HashTable* table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (table->occupied[i]) {
            free(table->slots[i]->key);
            free(table->slots[i]);
        }
    }

    free(table->slots);
    free(table->occupied);
    free(table);
}

// int main() {
//     HashTable* table = create_table();

//     insert(table, "apple", 10);
//     insert(table, "banana", 20);
//     insert(table, "orange", 30);

//     printf("Value of 'apple': %d\n", lookup(table, "apple"));     // Output: Value of 'apple': 10
//     printf("Value of 'banana': %d\n", lookup(table, "banana"));   // Output: Value of 'banana': 20
//     printf("Value of 'orange': %d\n", lookup(table, "orange"));   // Output: Value of 'orange': 30

//     remove_node(table, "banana");

//     printf("Value of 'banana' after removal: %d\n", lookup(table, "banana"));   // Output: Value of 'banana' after removal: -1

//     free_table(table);

//     return 0;
// }