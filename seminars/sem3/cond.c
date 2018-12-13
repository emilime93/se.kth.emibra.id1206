#include <stdio.h>
#include <unistd.h>
#include "green.h"

typedef struct thread_arg {
    int id;
    int count;
} thread_arg;

static green_mutex_t mutex;
static green_cond_t cond;
static int flag = 0;

void *test(void *arg) {
    int id = ((thread_arg *) arg)->id;
    int loop = ((thread_arg *) arg)->count;
    
    while (loop > 0) {
        green_mutex_lock(&mutex);
        while(1) {
            if (flag == id) {
                printf("%d in flag!\n", id);
                flag = (id + 1) % 3;
                green_cond_signal(&cond);
                green_mutex_unlock(&mutex);
                break;
            } else {
                green_cond_wait(&cond, &mutex);
            }
        }
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
