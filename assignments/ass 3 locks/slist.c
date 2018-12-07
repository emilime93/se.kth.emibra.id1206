#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

// The list will contain elements from 0 to 99
#define MAX 1000000

typedef struct cell {
    int val;
    struct cell *next;
    int mutex;
} cell;

cell sentinel = {MAX, NULL, 0};
cell dummy = {-1, &sentinel, 0};

cell *global = &dummy;

int try(volatile int *mutex) {
    return __sync_val_compare_and_swap(mutex, 0, 1);
}

void lock(volatile int *mutex) {
    while (try(mutex) != 0) {
        sched_yield();
    };
}

void unlock(volatile int *mutex) {
    *mutex = 0;
}

void toggle(cell *list, int r) {
    cell *prev = list;
    lock(&prev->mutex);
    cell *this = prev->next;
    lock(&this->mutex);

    cell *removed = NULL;

    while (this->val < r) {
        unlock(&prev->mutex);
        prev = this;
        this = this->next;
        lock(&this->mutex);
    }
    if (this->val == r) {
        prev->next = this->next;
        removed = this;
    } else {
        cell *new = malloc(sizeof(cell));
        new->val = r;
        new->next = this;
        new->mutex = 0;
        // pthread_mutex_init(&new->mutex, NULL);
        prev->next = new;
        new = NULL;
    }
    unlock(&prev->mutex);
    unlock(&this->mutex);

    if (removed != NULL)
        free(removed);
    return;
}

typedef struct args {
    int inc;
    int id;
    cell *list;
} args;

void *bench(void *arg) {
    int inc = ((args*)arg)->inc;
    int id = ((args*)arg)->id;
    cell *list = ((args*)arg)->list;

    for (int i = 0; i < inc; i++) {
        int r = rand() % MAX;
        toggle(list, r);
    }
    return NULL;
}

void print_list() {
    cell *this = global;
    printf(" i\tval\tnext*\n");
    int count = 0;
    while (this != NULL) {
        printf("%d\t%d\t%p\n", count, this->val, this->next);
        count++;
        this = this->next;
    }
}

int main(int argc, char const *argv[]) {
    srand(time(NULL));
    if (argc != 3) {
        printf("usage: list <total> <threads>\n");
        exit(0);
    }

    int n = atoi(argv[2]);
    int inc = atoi(argv[1]) / n;

    printf("%d threads doing %d operations each\n", n, inc);

    args *thra = malloc(n * sizeof(args));

    for (int i = 0; i < n; i++) {
        thra[i].inc = inc;
        thra[i].id = i;
        thra[i].list = global;
    }

    pthread_t *threads = malloc(n * sizeof(pthread_t));

    struct timespec t_start, t_stop;
    clock_gettime(_CLOCK_MONOTONIC_RAW, &t_start);
    for (int i = 0; i < n; i++) {
        pthread_create(&threads[i], NULL, bench, &thra[i]);
    }

    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_gettime(_CLOCK_MONOTONIC_RAW, &t_stop);
    long wall_sec = t_stop.tv_sec - t_start.tv_sec;
    long wall_nsec = t_stop.tv_nsec - t_start.tv_nsec;
    long wall_msec = (wall_sec*1000) + (wall_nsec/1000000);

    print_list();
    
    printf("Done in %ld\n", wall_msec);
    return 0;
}
