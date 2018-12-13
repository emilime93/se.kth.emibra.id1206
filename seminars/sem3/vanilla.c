#include <stdio.h>
#include <unistd.h>
#include "green.h"

typedef struct thread_arg {
    int id;
    int count;
} thread_arg;

void *test(void *arg) {
    int id = ((thread_arg *) arg)->id;
    int loop = ((thread_arg *) arg)->count;
    while (loop > 0) {
        /* Dummy loop to make threads waste time
         And demonstrate that the threads actually
         works concurrently */
        for (int i = 0; i < 1000000; i++);
        printf("thread %d: %d\n", id, loop);
        loop--;
    }
}

int main(int argc, char const *argv[]) {
    green_t g0, g1, g2;
    thread_arg a0 = {0, 10};
    thread_arg a1 = {1, 10};
    thread_arg a2 = {2, 10};

    green_create(&g0, test, &a0);
    green_create(&g1, test, &a1);
    green_create(&g2, test, &a2);
    green_join(&g0);
    green_join(&g1);
    green_join(&g2);

    return 0;
}
