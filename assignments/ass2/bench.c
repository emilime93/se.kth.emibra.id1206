#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "rand.h"

#define ROUNDS 10
#define LOOP 1000

#define BUFFER 100

int main() {
    // Initial heap address/position
    void *init = sbrk(0);
    void *current;

    // Initialize the buffer
    void *buffer[BUFFER];
    for (int i = 0; i < BUFFER; i++) {
        buffer[i] = NULL;
    }

    printf("The initial top of the heap is %p.\n", init);

    for (int i = 0; i < ROUNDS; i++) {
        for (int j = 0; j < LOOP; j++) {
            // Pick a position in the buffer, and if it's taken, free it.
            int index = rand() % BUFFER;
            if (buffer[index] != NULL) {
                free(buffer[index]);
            }
            // Get a random number and allocate it
            size_t size = (size_t) request();
            int *memory;
            memory = malloc(size);

            if (memory == NULL) {
                fprintf(stderr, "memory allication failed\n");
                return 1;
            }
            buffer[index] = memory;
            *memory = 123;
        }
        current = sbrk(0);
        int allocated = (int)((current - init) / 1024);
        printf("The current top of the head is %p.\n", current);
        printf("    increased by %d Kbyte\n", allocated);
    }

    return 0;
}