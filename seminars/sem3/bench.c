#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "green.h"

#define BILLION 1e9
#define MILLION 1e6

typedef struct p_arg {
    int id;
    int sum;
    pthread_mutex_t *mutex;
} p_arg;

typedef struct g_arg {
    int id;
    int sum;
    green_mutex_t *mutex;
} g_arg;

static volatile int p_shared_counter = 0;
void *p_test(void *arg) {
    int id = ((p_arg*) arg)->id;
    int sum = ((p_arg*) arg)->sum;
    pthread_mutex_t *mutex = ((p_arg*) arg)->mutex;
    printf("# Thread # %d adding %d\n", id, sum);
    for (int i = 0; i < sum; i++) {
        pthread_mutex_lock(mutex);
        p_shared_counter++;
        pthread_mutex_unlock(mutex);
    }
}

static volatile int g_shared_counter = 0;
void *g_test(void *arg) {
    int id = ((g_arg*) arg)->id;
    int sum = ((g_arg*) arg)->sum;
    green_mutex_t *mutex = ((g_arg*) arg)->mutex;
    printf("# Thread # %d adding %d\n", id, sum);
    for (int i = 0; i < sum; i++) {
        green_mutex_lock(mutex);
        g_shared_counter++;
        green_mutex_unlock(mutex);
    }
}

int main(int argc, char const *argv[]) {
    //The shared mutex
    pthread_mutex_t p_mutex;
    pthread_mutex_init(&p_mutex, NULL);
    pthread_attr_t pattr;
    pthread_attr_init(&pattr);

    pthread_t t1, t2, t3;
    p_arg ptarg1 = {1, 5000000, &p_mutex};
    p_arg ptarg2 = {2, 5000000, &p_mutex};
    p_arg ptarg3 = {3, 5000000, &p_mutex};

    clock_t p_start_time, p_end_time;
    p_start_time = clock();

    pthread_create(&t1, &pattr, p_test, (void*)&ptarg1);
    pthread_create(&t2, &pattr, p_test, (void*)&ptarg2);
    pthread_create(&t3, &pattr, p_test, (void*)&ptarg3);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    p_end_time = clock();
    double p_ns_time = ((double) p_end_time - p_start_time) / (CLOCKS_PER_SEC)*1000;

    printf("%.2f\t%d\n", p_ns_time, p_shared_counter);
    printf("# Pthread done! Counter: %d\n", p_shared_counter);

    /*********************** GREEN THREADS *************************/

    green_mutex_t g_mutex;
    green_mutex_init(&g_mutex);
    green_t g1, g2, g3;
    g_arg garg1 = {1, 5000000, &g_mutex};
    g_arg garg2 = {2, 5000000, &g_mutex};
    g_arg garg3 = {3, 5000000, &g_mutex};

    clock_t g_start_time, g_end_time;
    g_start_time = clock();
    green_create(&g1, &g_test, &garg1);
    green_create(&g2, &g_test, &garg2);
    green_create(&g3, &g_test, &garg3);
    green_join(&g1);
    green_join(&g2);
    green_join(&g3);

    g_end_time = clock();
    double g_ns_time = (((double) g_end_time - g_start_time) / (CLOCKS_PER_SEC))*1000;

    printf("%.2f\t%d\n", g_ns_time, g_shared_counter);
    printf("# Gthread done! Counter: %d\n", g_shared_counter);

    return 0;
}
