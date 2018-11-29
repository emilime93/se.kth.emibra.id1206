#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "rand.h"
#include "buddy.h"

#define ROUNDS 10
#define LOOP 1000

#define BUFFER 1000

int main(int argc, char *argv[]) {
    // Initialize the buffer
    void *buffer[BUFFER];
    for (int i = 0; i < BUFFER; i++) {
        buffer[i] = NULL;
    }

    struct timeval balloc_stop, balloc_start;
    gettimeofday(&balloc_start, NULL);

    for (int i = 0; i < ROUNDS; i++) {
        for (int j = 0; j < LOOP; j++) {
            // Pick a position in the buffer, and if it's taken, free it.
            int index = rand() % BUFFER;
            if (buffer[index] != NULL) {
                bfree(buffer[index]);
            }
            // Get a random number and allocate it
            size_t size = (size_t) request();
            int *memory;
            memory = balloc(size);

            if (memory == NULL) {
                fprintf(stderr, "memory allication failed\n");
                return 1;
            }
            buffer[index] = memory;
            *memory = 123;
        }
    }
    gettimeofday(&balloc_stop, NULL);
    printf("Time taken:%fms\n", (double)(balloc_stop.tv_usec - balloc_start.tv_usec) / 1000);

    return 0;
}