#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100
#define MAX_LINE 81

/* Define a linked list node for storing key-value pairs */
typedef struct Node {
    char* key;
    int value;
    int external;

    struct Node* next;
} Node;

/* Define the hash table */
typedef struct {
    Node* table[TABLE_SIZE];
} HashTable;

/* Hash function: djb2 */
unsigned long hash(const char* key) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash % TABLE_SIZE;
}

/* Initialize the hash table */
void initHashTable(HashTable* ht) {
    int i;
    for ( i = 0; i < TABLE_SIZE; i++) {
        ht->table[i] = NULL;
    }
}

/* Insert a key-value pair into the hash table */
void insert(HashTable* ht, const char* key, int value) {
    unsigned long index = hash(key);

    /* Create a new node */
    Node* newNode = malloc(sizeof(Node));
    newNode->key = malloc(strlen(key) + 1);
    strcpy(newNode->key, key);
    newNode->value = value;
    newNode->next = NULL;

    /* Insert the node into the list at the hash table index */
    if (ht->table[index] == NULL) {
        ht->table[index] = newNode;
    } else {
        Node* curr = ht->table[index];
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = newNode;
    }
}

/* Search for a key in the hash table */
int search(HashTable* ht, const char* key) {
    unsigned long index = hash(key);

    /* Traverse the list at the hash table index to find the key */
    Node* curr = ht->table[index];
    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            return curr->value;
        }
        curr = curr->next;
    }

    /* Key not found */
    return -1;
}

/* Remove a key-value pair from the hash table */
void removeKey(HashTable* ht, const char* key) {
    unsigned long index = hash(key);

    /* Traverse the list at the hash table index to find the key */
    Node* curr = ht->table[index];
    Node* prev = NULL;
    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            /* Remove the node from the list */
            if (prev == NULL) {
                ht->table[index] = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr->key);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

/* Print the hash table */
void printHashTable(HashTable* ht) {
    int i;
    for ( i = 0; i < TABLE_SIZE; i++) {
        Node* curr = ht->table[i];
        if (curr != NULL) {
            while (curr != NULL) {
                printf(" (%s, %d)", curr->key, curr->value);
                curr = curr->next;
            }
            printf("\n");
        }
    }
}


/* Search for a key in the hash table with spaces before or after */
int searchWithSpaces(HashTable* ht, const char* key) {
    char trimmedKey[MAX_LINE];

    int i, j;
    for (i = 0; key[i] == ' '; i++);
    for (j = strlen(key) - 1; j >= 0 && key[j] == ' '; j--);
    strncpy(trimmedKey, key + i, j - i + 1);
    trimmedKey[j - i + 1] = '\0';

    /* Traverse the hash table to find the key */
    for (i = 0; i < TABLE_SIZE; i++) {
        Node* curr = ht->table[i];
        while (curr != NULL) {
            if (strstr(curr->key, trimmedKey) != NULL) {

                return curr->value;
            }
            curr = curr->next;
        }
    }


    return -1;
}


