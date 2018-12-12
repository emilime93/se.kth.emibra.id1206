#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include "green.h"

typedef struct thread_arg {
    int id;
    int count;
} thread_arg;

void *test(void *arg) {
    int id = ((thread_arg *) arg)->id;
    int loop = ((thread_arg *) arg)->count;
    while (loop > 0) {
        // Loop for wasting time and seeing the effect of the timer
        for(int j = 0; j < 10000000; j++);
        printf("thread %d: %d\n", id, loop);
        loop--;
        // green_yield();
    }
}

/* Variables for mutex test */
green_mutex_t mutex;
volatile int shared_counter = 0;
int three = 0, four = 0;

void *mutext_test(void *arg) {
    int id = ((thread_arg *) arg)->id;
    int count = ((thread_arg *) arg)->count;
    printf("#%d count: %d\n", id, count);
    for (int j = 0; j < 10000000; j++);
    for (int i = 0; i < count; i++) {
        green_mutex_lock(&mutex);
        shared_counter++;
        // DEBUG
        if(id==3) {
            three++;
        } else {
            four++;
        }
        green_mutex_unlock(&mutex);
    }
}

/* variables for conditionals test */
int flag = 0;
green_cond_t cond;

void *cond_test(void *arg) {
    int id = ((thread_arg *) arg)->id;
    int count = ((thread_arg *) arg)->count;
    int loop = count;
    while (loop > 0) {
        if (flag == id) {
            printf(">>> thread %d: %d\n", id, loop);
            loop--;
            flag = (id + 1) % 3;
            green_cond_signal(&cond);
        } else {
            green_cond_wait(&cond);
        }
    }
}
int main(int argc, char const *argv[]) {
    green_t g0, g1, g2, g3, g4, g5, g6, g7;
    thread_arg a0 = {0, 10};
    thread_arg a1 = {1, 10};
    thread_arg a2 = {2, 10};
    thread_arg a3 = {3, 1000000};
    thread_arg a4 = {4, 1000000};
    thread_arg a5 = {0, 8};
    thread_arg a6 = {1, 8};
    thread_arg a7 = {2, 8};


    /* VANILLA TEST */
    // green_create(&g0, test, &a0);
    // green_create(&g1, test, &a1);
    // green_create(&g2, test, &a2);
    // green_join(&g0);
    // green_join(&g1);
    // green_join(&g2);

    /* LOCK/MUTEX TEST */
    green_mutex_init(&mutex);
    green_create(&g3, mutext_test, &a3);
    green_create(&g4, mutext_test, &a4);
    green_join(&g3);
    green_join(&g4);
    printf("three=%d four=%d\nTOTAL: %d\n", three, four, shared_counter);

    /* CONDITIONAL TEST */
    // green_cond_init(&cond);
    // green_create(&g5, cond_test, &a5);
    // green_create(&g6, cond_test, &a6);
    // green_create(&g7, cond_test, &a7);
    // green_join(&g5);
    // green_join(&g6);
    // green_join(&g7);

    printf("Test done\n");

    return 0;
}
