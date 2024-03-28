#ifndef TS_HASHMAP_H
#define TS_HASHMAP_H
// Author @Curtis Alves
#include <pthread.h>
#define HASH_MAP_SIZE 1024

typedef struct {
    int key;
    int value;
} HashItem;

typedef struct {
    HashItem** items;
    pthread_mutex_t mutex;
    int numOps; // Tracks the number of operations performed
} HashMap;

HashMap *initmap(int capacity);
void freeMap(HashMap *map);
int put(HashMap *map, int key, int value);
int get(HashMap *map, int key);
int del(HashMap *map, int key);

#endif
