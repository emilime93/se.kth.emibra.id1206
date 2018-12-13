#include <stdio.h>
#include <unistd.h>
#include "green.h"

typedef struct thread_arg {
    int id;
    int count;
} thread_arg;

static green_mutex_t mutex;
static volatile int shared_counter = 0;

void *test(void *arg) {
    int id = ((thread_arg *) arg)->id;
    int count = ((thread_arg *) arg)->count;
    printf("#%d count: %d\n", id, count);
    for (int i = 0; i < count; i++) {
        green_mutex_lock(&mutex);
        shared_counter++;
        green_mutex_unlock(&mutex);
    }
}

int main(int argc, char const *argv[]) {
    green_t g0, g1, g2;
    thread_arg a0 = {0, 1000000};
    thread_arg a1 = {1, 1000000};
    thread_arg a2 = {2, 1000000};

    green_create(&g0, test, &a0);
    green_create(&g1, test, &a1);
    green_create(&g2, test, &a2);
    green_join(&g0);
    green_join(&g1);
    green_join(&g2);

    printf(" - Shared counter: %d\n", shared_counter);

    return 0;
}
