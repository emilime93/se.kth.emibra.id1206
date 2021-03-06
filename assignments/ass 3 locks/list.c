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
} cell;

cell sentinel = {MAX, NULL};
cell dummy = {-1, &sentinel};

cell *global = &dummy;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void toggle(cell *list, int r) {
    cell *prev = NULL;
    cell *this = list;
    cell *removed = NULL;

    pthread_mutex_lock(&mutex);

    while (this->val < r) {
        prev = this;
        this = this->next;
    }
    if (this->val == r) {
        prev->next = this->next;
        removed = this;
    } else {
        cell *new = malloc(sizeof(cell));
        new->val = r;
        new->next = this;
        prev->next = new;
    }
    pthread_mutex_unlock(&mutex);
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

    pthread_mutex_init(&mutex, NULL);

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
