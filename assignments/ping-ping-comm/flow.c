#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>

#define ITERATIONS 10
#define BURSTS 100

int main(int argc, char const *argv[]) {
    int descr[2];

    assert(0 == pipe(descr));

    if (fork() == 0 ) {
        /* consumer */
        for (int i = 0; i < ITERATIONS; i++) {
            for (int j = 0; j < BURSTS; j++) {
                int buffer[10];
                read(descr[0], &buffer, 10);    
            }
            printf("consumer read part %d\n", i);
            sleep(1);
        }
        printf("consumer done");
    } else {
        /* producer */
        for (int i = 0; i < ITERATIONS; i++) {
            for (int j = 0; j < BURSTS; j++) {
                write(descr[1], "0123456789", 10);
            }
            printf("producer burst %d done\n", i);
        }
        printf("producer done\n");
    }

    wait(NULL);
    printf("all done\n");
    return 0;
}
