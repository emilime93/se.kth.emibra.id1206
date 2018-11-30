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
    // Init settings from args, or lack thereof
    rounds = (argc > 1) ? atoi(argv[1]) : ROUNDS;
    loop = (argc > 2) ? atoi(argv[2]) : LOOP;

    void *buffer[BUFFER];
    for (int i = 0; i < BUFFER; i++) {
        buffer[i] = NULL;
    }
    srand(time(0)); // Extra random!

    struct timeval balloc_stop, balloc_start;
    for (int i = 0; i < rounds; i++) {
        double total = 0;
        gettimeofday(&balloc_start, NULL);
        for (int j = 0; j < loop; j++) {
            // Pick a position in the buffer to use, if it's free, use it!
            int index = rand() % BUFFER;
            if (buffer[index] != NULL) {
                bfree(buffer[index]);
            }
            // Get a random number and allocate it
            size_t size = (size_t) request();
            // size_t size = (size_t) (rand() % 2000)+2000;
            // size_t size = (size_t) (rand() % 500);
            int *memory;
            memory = balloc(size);

            if (memory == NULL) {
                fprintf(stderr, "memory allication failed\n");
                exit(1);
            }
            buffer[index] = memory;
            *memory = 123;  // Force the os to actually create the memory for us
            gettimeofday(&balloc_stop, NULL);
        }
        total += (double)(balloc_stop.tv_usec - balloc_start.tv_usec);
        printf("%f\n", total/1000);
        // Time in MS
        // Precent memory usage as a rational fraction
    }
    // printf("%f\n", (double)mem_rat/(rounds*loop));
    // printf("%f\n", (double)total/(LOOP*ROUNDS)/1000);

    return 0;
}