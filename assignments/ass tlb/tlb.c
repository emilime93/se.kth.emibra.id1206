#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PAGES 1024
#define REFS (1024 * 1024)
#define PAGESIZE (1024*8)

int main(int argc, char const *argv[])
{

    char *memory = malloc((long)PAGESIZE * PAGES);
    for (int p = 0; p < PAGES; p++) {
        long ref = (long) p * PAGESIZE;
        /* Force the page to be allocated */
        memory[ref] += 1;
    }

    clock_t c_start, c_stop;

    printf("# TLB experminet\n");
    printf("# page size = %d bytes\n", PAGESIZE);
    printf("# max pages = %d\n", PAGES);
    printf("# total number of references = %d Mi\n\n", REFS/(1024*1024));
    printf("#pages\t proc\t\t sum\n");
    
    for (int pages = 1; pages <= PAGES; pages *= 2) {
        int loops = REFS/pages;

        c_start = clock();

        long sum = 0;

        for (int l = 0; l < loops; l++) {
            for (int p = 0; p < pages; p++) {
                // Dummy operation
                long ref = (long) p * PAGESIZE;
                sum += memory[ref];
                // sum++;
            }
        }

        c_stop = clock();

        {
            double proc = ((double) (c_stop-c_start))/CLOCKS_PER_SEC;
            printf("%d\t %.4f\t %ld\n", pages, proc, sum);
        }
    }

    return 0;
}
