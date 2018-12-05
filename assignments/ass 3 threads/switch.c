#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>

int main(int argc, char *argv[]) {
    register int done = 0;

    ucontext_t one;
    ucontext_t two;

    // Populate the first context with our currently used process
    getcontext(&one);

    printf("Hello \n");

    if (!done) {
        done = 1;
        // Saves the currently running in "two" and copy the context of "one" into the proper registers of the CPU
        swapcontext(&two, &one);
    }

    return 0;
}