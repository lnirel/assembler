#ifndef HASHTABLE_H
#define HASHTABLE_H

#define TABLE_SIZE 100

typedef struct Node {
    char* key;
    int value;
    int external;

    struct Node* next;
} Node;

typedef struct {
    Node* table[TABLE_SIZE];
} HashTable;

unsigned long hash(const char* key);
void initHashTable(HashTable* ht);
void insert(HashTable* ht, const char* key, int value);
int search(HashTable* ht, const char* key);
void removeKey(HashTable* ht, const char* key);
void printHashTable(HashTable* ht);
int searchWithSpaces(HashTable* ht, const char* key);
void setExternal(HashTable* ht, const char* key, int value);
int isExternal(HashTable* ht, const char* key);
#endif /* HASHTABLE_H */
