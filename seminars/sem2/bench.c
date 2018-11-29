#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "rand.h"
#include "buddy.h"

#define ROUNDS 10
#define LOOP 1000

#define BUFFER 100

int main(int argc, char *argv[]) {
    int rounds, loop;
    rounds = (argc > 1) ? atoi(argv[1]) : ROUNDS;
    loop = (argc > 2) ? atoi(argv[2]) : LOOP;
    printf("Running with %d rounds and %d loops\n", rounds, loop);
    // Initialize the buffer
    void *buffer[BUFFER];
    for (int i = 0; i < BUFFER; i++) {
        buffer[i] = NULL;
    }

    srand(time(0));

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
                exit(1);
            }
            buffer[index] = memory;
            *memory = 123;
        }
    }
    gettimeofday(&balloc_stop, NULL);
    printf("Time taken:%fms\n", (double)(balloc_stop.tv_usec - balloc_start.tv_usec) / 1000);

    return 0;
}