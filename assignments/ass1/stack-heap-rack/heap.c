#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void zot(unsigned long*);
void foo(unsigned long*);

int main() {
    int pid = getpid();

    unsigned long p = 0x1;

    // char *heap = malloc(20);
    // *heap = 0x61;
    // printf("heap pointing to: 0x%x\n", *heap);

    // free(heap);

    // char *foo = malloc(20);
    // *foo = 0x62;
    // printf("foo pointing to: 0x%x\n", *foo);

    // /* danger ahead */
    // *heap = 0x63;
    // printf("or is it pointing to: 0x%x\n\n\n\n", *foo);

    long *heap = (unsigned long*) calloc(40, sizeof(unsigned long*));
    printf("heap[2]: 0x%lx\n", heap[2]);
    printf("heap[1]: 0x%lx\n", heap[1]);
    printf("heap[0]: 0x%lx\n", heap[0]);
    printf("heap[-1]: 0x%lx\n", heap[-1]);
    printf("heap[-2]: 0x%lx\n", heap[-2]);

    free(heap);

    printf("heap[2]: 0x%lx\n", heap[2]);
    printf("heap[1]: 0x%lx\n", heap[1]);
    printf("heap[0]: 0x%lx\n", heap[0]);
    printf("heap[-1]: 0x%lx\n", heap[-1]);
    printf("heap[-2]: 0x%lx\n", heap[-2]);

    printf("the heap variable at: %p\n", &heap);
    printf("pointing to: %p\n", heap);

    printf("p: %p\n", &p);

    printf("\n\n /proc/%d/maps \n\n", pid);
    char command[50];
    sprintf(command, "cat /proc/%d/maps", pid);
    system(command);

    return 0;
}