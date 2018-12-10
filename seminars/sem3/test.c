#include <stdio.h>
#include "green.h"

void *test(void *arg) {
    int i = *(int*) arg;
    int loop = 4;
    while (loop > 0) {
        printf("thread %d: %d\n", i, loop);
        loop--;
        green_yield();
    }
}

int main(int argc, char const *argv[]) {
    green_t g0, g1, g2, g3;
    int a0 = 0;
    int a1 = 1;
    int a2 = 2;
    int a3 = 3;

    green_create(&g0, test, &a0);
    green_create(&g1, test, &a1);
    green_create(&g2, test, &a2);
    green_create(&g3, test, &a3);

    green_join(&g0);
    green_join(&g1);
    green_join(&g2);
    green_join(&g3);

    printf("Test done\n");

    return 0;
}
