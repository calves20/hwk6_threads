#include "ts_hashmap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Author @Curtis Alves
unsigned int hash_function(int key) {
    return key % HASH_MAP_SIZE;
}

// Initializes the hash map structure and allocates memory for its items
// Returns a pointer to the allocated hash map structure
HashMap *initmap(int capacity) {
    // Allocate memory for the hash map structure itself
    HashMap *map = malloc(sizeof(HashMap));
    if (!map) {
        fprintf(stderr, "Failed to allocate memory for HashMap\n");
        return NULL;
    }

    // Allocate memory for the array of pointers to HashItem structures
    map->items = calloc(HASH_MAP_SIZE, sizeof(HashItem*));
    if (!map->items) {
        free(map); // Clean up previously allocated map structure
        fprintf(stderr, "Failed to allocate memory for items\n");
        return NULL;
    }

    // Initialize the mutex for later use in thread-safe operations
    pthread_mutex_init(&map->mutex, NULL);
    map->numOps = 0; // Initialize the operation counter
    return map;
}

// Frees the hash map structure and all its associated items
void freeMap(HashMap *map) {
    // Loop through the array of item pointers and free each HashItem
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        if (map->items[i] != NULL) {
            free(map->items[i]); // Free the individual HashItem
        }
    }
    free(map->items); // Free the array of pointers
    pthread_mutex_destroy(&map->mutex); // Destroy the mutex
    free(map); // Finally, free the hash map structure itself
}

// Inserts a key-value pair into the hash map
// If an item with the same key already exists, it replaces it
int put(HashMap *map, int key, int value) {
    unsigned int index = hash_function(key); // Compute the index for the key

    pthread_mutex_lock(&map->mutex); // Ensure thread-safe write access
    if (map->items[index] != NULL) {
        // If an item exists at the index, free it to prevent memory leaks
        free(map->items[index]);
    }

    // Allocate and setup the new HashItem
    HashItem *newItem = malloc(sizeof(HashItem));
    if (!newItem) {
        pthread_mutex_unlock(&map->mutex);
        return -1; // Return error if memory allocation fails
    }

    newItem->key = key;
    newItem->value = value;
    map->items[index] = newItem; // Place the new item in the map
    map->numOps++; // Increment the operation counter
    pthread_mutex_unlock(&map->mutex);

    return 0; // Success
}

// Retrieves a value by its key from the hash map
// Returns the value if found, -1 otherwise
int get(HashMap *map, int key) {
    unsigned int index = hash_function(key);
    int value = -1; // Default to -1 if key not found

    pthread_mutex_lock(&map->mutex); // Ensure thread-safe read access
    HashItem *item = map->items[index];
    if (item && item->key == key) { // Verify item exists and keys match
        value = item->value;
    }
    pthread_mutex_unlock(&map->mutex);

    return value;
}

// Deletes a key-value pair from the hash map by its key
// Returns 0 on success, -1 if the key is not found
int del(HashMap *map, int key) {
    unsigned int index = hash_function(key);
    
    pthread_mutex_lock(&map->mutex); // Ensure thread-safe access
    if (map->items[index] != NULL && map->items[index]->key == key) {
        free(map->items[index]); // Free the HashItem
        map->items[index] = NULL; // Clear the pointer
        map->numOps++; // Increment the operation counter
        pthread_mutex_unlock(&map->mutex);
        return 0; // Success
    }

    pthread_mutex_unlock(&map->mutex);
    return -1; // Key not found
}
