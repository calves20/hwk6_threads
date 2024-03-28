#include "ts_hashmap.h"
#include "rtclock.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Author @Curtis Alves
void* thread_work(void* arg) {
    HashMap* map = (HashMap*)arg;
    int maxkey = 500; // Maximum key value for operations
    for (int i = 0; i < 10000; i++) { // Perform 10,000 operations per thread
        int key = rand() % maxkey; // Generate a random key
        int value = rand(); // Generate a random value
        put(map, key, value); // Insert the key-value pair into the hash map
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <threads> <capacity> <maxkey>\n", argv[0]);
        return 1;
    }

    // Parse command-line arguments
    int threads = atoi(argv[1]);
    int capacity = atoi(argv[2]); // Note: Not used in this simplified example
    int maxkey = atoi(argv[3]);
    pthread_t thread_ids[threads]; // Array to hold thread identifiers

    // Initialize the hash map
    HashMap *map = initmap(HASH_MAP_SIZE);
    if (!map) {
        printf("Failed to initialize the hash map.\n");
        return -1;
    }

    double startTime = rtclock(); // Start timing

    // Create threads to perform operations on the hash map
    for (int i = 0; i < threads; i++) {
        pthread_create(&thread_ids[i], NULL, thread_work, map);
    }

    // Wait for all threads to complete
    for (int i = 0; i < threads; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    double endTime = rtclock(); // End timing
    double elapsed = endTime - startTime; // Calculate elapsed time
    long totalOps = (long)threads * 10000; // Total number of operations

    // Output performance metrics
    printf("Number of ops = %ld, time elapsed = %.6f sec\n", totalOps, elapsed);
    printf("Time per op   = %.6f ms\n", (elapsed / totalOps) * 1000);

    freeMap(map); // Cleanup and free the hash map
    return 0;
}
