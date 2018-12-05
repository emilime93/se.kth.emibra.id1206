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
    int id;
    int present;
    struct pte *next;
    struct pte *prev;
} pte;

void init(int *sequence, int refs, int pages) {

    int high = (int)(pages*((float)HIGH/100));
    if(high < 2) high = 2;
    
    /* dummy value */
    int prev = pages; 
    
    for(int i = 0; i<refs; i++) {
        if(rand()%100 < FREQ) {    
        int rnd;
        do {
        /* we want to prevent the same page being picked again */
        rnd = rand()%high;
        } while (rnd == prev);
        prev = rnd;
        sequence[i] = rnd;          
        } else {
        int rnd;
        do {
        rnd = high + rand()%(pages - high);
        } while (rnd == prev);
        prev = rnd;
        sequence[i] = rnd;
        }
    }
}

void clear_page_table(pte *page_table, int pages) {
    for (int i = 0; i < pages; i++) {
        page_table[i].id = i;
        page_table[i].present = 0;
        page_table[i].next = 0;
        page_table[i].prev = 0;
    }
}

int simulate (int *seq, pte *table, int refs, int frames, int pages) {
    int hits = 0;
    int allocated = 0;

    pte *first = NULL;
    pte *last = NULL;

    for (int i = 0; i < refs; i++) {
        int next = seq[i];
        pte *entry = &table[next];

        if (entry->present == 1) {
            hits++;
            /* Unlink the entry and place last */
            if(entry->next != NULL) {
                if(first == entry) {
                    first = entry->next;
                } else {
                    entry->prev->next = entry->next;
                }
                entry->next->prev = entry->prev;

                entry->prev = last;
                entry->next = NULL;

                last->next = entry;
                last = entry;
            }
        } else {
            if (allocated < frames) {
                allocated++;

                entry->present = 1;
                entry->prev = last;
                entry->next = NULL;

                /* place entry last */
                if(last != NULL) {
                    last->next = entry;
                }
                if(first == NULL) {
                    first = entry;
                }
                last = entry;
            } else {
                pte *evict;

                assert(first != NULL);

                evict = first;
                first = evict->next;

                evict->present = 0;

                entry->present = 1;
                entry->prev = last;
                entry->next = NULL;
                
                /* place entry last */
                if(last != entry) {
                    last->next = entry;
                }

                if(first == NULL) {
                    first = entry;
                }
                last = entry;
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
