#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#define HIGH 20
#define FREQ 80
#define PAGES 100
#define SAMPLES 20

typedef struct pte {
  int present;
} pte;

void init(int *sequence, int refs, int pages){
  srand(time(NULL));

  int high = (int)(pages*(float)HIGH/100);

  for (int i = 0; i < refs; i++){
    if(rand()%100 < FREQ){
      /* The frequently case */
      sequence[i] = rand()%high;
    } else {
      sequence[i] = high + rand()%(pages-high);
    }
  }
}

void clear_page_table(pte *page_table, int pages){
  for(int i = 0; i < pages; i++) {
    page_table[i].present = 0;
  }
}

int simulate (int *seq, pte *table, int refs, int frames, int pages){
  int hits = 0;
  int allocated = 0;

  int i;

  for(i = 0; i < refs; i++) {
    int next = seq[i];
    pte *entry = &table[next];

    if (entry->present == 1) {
      hits++;
      if (entry->next != NULL) {
        /* Unlink the entry and place it last */
        if (first == entry) {
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
      } else {
        pte *evict;
        /* Initial values */

        int sofar = 0;
        int candidate = pages;

        for(int c = 0; c < pages; c++){
          if(table[c].present == 1){
            /* The page is allocated */
            int dist = 0;
            while(seq[i+dist] != c && i+dist < refs){
              dist++;
            }
            if(dist > sofar){
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

int main(int argc, char *argv[]){
  /* Could be command line arguments */
  int refs = 100000;
  int pages = 100;
  pte table[PAGES];

  int *sequence = (int*)malloc(refs*sizeof(int));

  init(sequence, refs, pages);

  /* A small experiment to show that it works */
  //for(int i = 0; i < refs; i++) {
  //  printf("%d, ", sequence[i]);
  //}
  //printf("\n");

  printf("# This is a benchmark of random replacement\n");
  printf("# %d page reference\n", refs);
  printf("# %d pages \n", pages);
  printf("#\n#\n#frames\tratio\n");

  /* Frames is the sixe of the memory in frames */
  int frames;

  int incr = pages/SAMPLES;

  for(frames = incr; frames <= pages; frames += incr){
    /* Clear page tables entries */
    clear_page_table(table, pages);

    int hits = simulate(sequence, table, refs, frames, pages);

    float ratio = (float)hits/refs;

    printf("%d\t%.2f\n", frames, ratio);
  }

  return 0;
}
