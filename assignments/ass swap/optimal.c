#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>

/* 20% of the pages will have 80% of the references */
#define HIGH 20
#define FREQ 80

#define REFS 10
#define PAGES 100
#define SAMPLES 20

typedef struct pte {
    int present;
} pte;

void init(int *sequence, int refs, int pages) {

    int high = (int) (pages * ((float) HIGH/100));
    
    for (int i = 0; i < refs; i++) {
        if (rand() % 100 < FREQ) {
            /* the frequently case */
            sequence[i] = rand() % high;
        } else {
            /* the less frequently case */
            sequence[i] = high + rand() % (pages - high);
        }
    }
}

void clear_page_table(pte *page_table, int pages) {
    for (int i = 0; i < pages; i++) {
        page_table[i].present = 0;
    }
}

int simulate (int *seq, pte *table, int refs, int frames, int pages) {
    int hits = 0;
    int allocated = 0;

    int i;
    for (i = 0; i < refs; i++) {
        int next = seq[i];
        pte *entry = &table[next];

        if (entry->present == 1) {
            hits++;
        } else {
            if (allocated < frames) {
                allocated++;
                entry->present = 1;
            } else {
                pte *evict;

                int sofar = 0;
                int candidate = pages;

                for (int c = 0; c < pages; c++) {
                    if (table[c].present ==1) {
                        int dist = 0;
                        while (seq[i+dist] != c && i+dist < refs) {
                            dist++;
                        }
                        if (dist > sofar) {
                            candidate = c;
                            sofar = dist;
                        }
                    }
                }
                evict = &table[candidate];

                evict->present = 0;
                entry->present = 1;
            }
        }
    }
    return hits;
}

int main(int argc, char const *argv[]) {
    // The truest random there is!
    srand(time(NULL));

    // Initialize variables from arguments
    if (argc < 2)
        printf("# usage: <refs> <pages>\nDefaulting to %d %d\n", REFS, PAGES);
    int refs = (argc > 1) ? atoi(argv[1]) : REFS;
    int pages = (argc > 2) ? atoi(argv[2]) : PAGES;
    
    int *sequence = (int *) malloc(refs * sizeof(int));

    init(sequence, refs, pages);
    
    pte *table = (pte *) malloc(pages * sizeof(pte));

    printf("# This is a benchmark of random replacement\n");
    printf("# %d page references\n", refs);
    printf("# %d pages\n", pages);
    printf("#\n#\n# frames\tratio\n");

    /* frames is the size of the memory in frames */
    int frames;
    int incr = pages/SAMPLES;

    for (frames = incr; frames <= pages; frames += incr) {
        /* clear page table entries */
        clear_page_table(table, pages);
        int hits = simulate(sequence, table, refs, frames, pages);
        float ratio = (float)hits/refs;

        printf("%d\t\t%.4f\n", frames, ratio);
    }
    
    return 0;
}
