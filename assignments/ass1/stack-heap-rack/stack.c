#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void zot(unsigned long*);
void foo(unsigned long*);

int main() {
    int pid = getpid();

    unsigned long p = 0x1;

    foo(&p);
    //printf("p: (%p)\n", &p);

back:
    printf("p: %p\n", &p);
    printf("back: %p\n", &&back);

    printf("\n\n /proc/%d/maps \n\n", pid);
    char command[50];
    sprintf(command, "cat /proc/%d/maps", pid);
    system(command);

    return 0;
}

void foo(unsigned long *stop) {
    unsigned long q = 0x2;
    //printf("q: (%p)\n", &q);

    zot(stop);
}

void zot(unsigned long *stop) {
    unsigned long r = 0x3;
    //printf("r: (%p)\n", &r);

    unsigned long *i;
    for(i = &r; i <= stop; i++) {
        printf(" %p = 0x%lx\n", i, *i);
    }
}