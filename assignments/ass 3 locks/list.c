#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

// The list will contain elements from 0 to 99
#define MAX 100

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
    printf(" val\tnext*\n");
    while (this != NULL) {
        printf("[%d,\t%p]->\n", this->val, this->next);
        this = this->next;
    }
}

int main(int argc, char const *argv[]) {
    if (argc != 3) {
        printf("usage: list <total> <threads>\n");
        exit(0);
    }
    
    print_list();

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
    for (int i = 0; i < n; i++) {
        pthread_create(&threads[i], NULL, bench, &thra[i]);
    }

    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    print_list();
    
    printf("Done\n");
    return 0;
}
