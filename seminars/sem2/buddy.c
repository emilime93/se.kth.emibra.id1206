#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <assert.h>
#ifdef __linux__
#include <string.h>
#elif __APPLE__
#include <strings.h>
#endif
#include <math.h>

#define MIN 5
#define LEVELS 8
#define PAGE 4096

struct head *find(int index);
void insert(struct head*);
void test_headers(struct head*);
void print_mem();

struct head *flists[LEVELS] = {NULL};

enum flag {Free = 1337, Taken = 666}; // To make test_headers print unknown

struct head {
    enum flag status;
    short int level;
    struct head *next;
    struct head *prev;
};

struct head *new() {
    struct head *new = (struct head*) mmap(NULL, PAGE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (new == MAP_FAILED) {
        return NULL;
    }
    assert(((long int)new & 0xfff) == 0); // last 12 bits should be zero
    new->status = Free;
    new->level = LEVELS - 1;
    return new;
}

struct head *buddy(struct head *block) {
    int index = block->level;
    long int mask = 0x1 << (MIN + index);
    return (struct head*)((long int) block ^ mask); // ^ == XOR
}

/* returns the address to the secondary block with correct headers, and sets the main block headers */
struct head *split(struct head *block) {
    if (block == NULL) {
        printf("ERROR IN SPLIT, block == NULL!!!!!!!\n");
        exit(1);
    }
    int index = block->level - 1;
    int mask = 0x1 << (index + MIN);
    struct head *split = (struct head*) ((long int) block | mask);
    split->level = index;
    split->status = Free;
    block->level = index;
    return split;
}

struct head *primary(struct head *block) {
    if (block == NULL) {
        printf("ERROR IN PRIMARY, block == NULL!!!!!!!\n");
        exit(1);
    }
    int index = block->level;
    long int mask = 0xffffffffffffffff << (MIN + index + 1);
    // Remove this maybe??
    struct head *prim = (struct head*) ((long int) block & mask);
    prim->level = index + 1;
    return prim;
}

/* Gives the address to the next address to hide access to the header */
void *hide(struct head* block) {
    return (void *) (block + 1 );
}

/* Backs the address one step in order to retrive the header from a block */
// struct head *magic(struct head* memory) {
//     return ((struct head*) memory - 1);
// }
struct head *magic(void* memory) {
    return ((struct head*) memory - 1);
}

/* Finds what block-level is required to allocate the requested amount of memory */
int level(int req) {
    int total = req + sizeof(struct head);

    int lvl = 0;
    int size = 1 << MIN;
    while (total > size) {
        size <<= 1;
        lvl += 1;
    }
    return lvl;
}
/* allocate a block */
void *balloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    int index = level(size);
    printf("I need to allocate level %d (%zu B) memory\n", index, size);
    struct head *taken = find(index);
    return hide(taken);
}

/* free a memory block */
void bfree(void *memory) {
    if (memory != NULL) {
        struct head *block = magic(memory);
        insert(block);
    }
    return;
}

/* Strictly removes a block from its level in the list, and relinks the others */
void unlink_block(struct head *block) {
    if(block->prev == NULL) { // It was the first in list
        if (block->next != NULL) { // Is there is something more in line
            flists[block->level] = block->next;
            flists[block->level]->prev = NULL;
        } else {
            flists[block->level] = NULL;
        }
    } else { // Link me out from the middle of a sandwhich
        block->prev->next = block->next;
        block->next->prev = block->prev;
    }
}

/* Strictly inserts a block in the beginning of a linled list, at the level that the block has */
void link_block(struct head *block) {
    int level = block->level;
    block->next = flists[level];
    if (flists[level] != NULL) {
        flists[level]->prev = block;
    }
    flists[level] = block;
    block->prev = NULL;
}

/* splits up the blocks from level-level to goal */
/* first level guaranteed by caller to have a block */
/* and subsequent levels are by logic guaranteed to contain data */
void split_up(int level, int goal) {
    if (level <= goal) {
        return;
    }
    // Unlink the block from list
    struct head *block = flists[level];
    unlink_block(block);

    // Split it up
    struct head *supl = split(block);

    // Link these into the lower level.
    link_block(supl);
    link_block(block);
    
    // Continue
    split_up(level - 1, goal);
}

/* fetches a  */
struct head *find(int level) {
    if (flists[level] == NULL) {
        int lvl_w_mem = level + 1;
        while(flists[lvl_w_mem] == NULL && lvl_w_mem < LEVELS) {
            lvl_w_mem++;
        }
        if (lvl_w_mem == LEVELS + 1) {
            //TODO: Allocate more memory with MMAP
            printf("need to allocate more memory!\n");
            exit(1);
        }
        split_up(lvl_w_mem, level);
    }
    struct head *alloc = flists[level];
    unlink_block(alloc);
    alloc->status = Taken;

    return alloc;
}

void merge(struct head *block) {
    if (block->level == LEVELS-1) {
        printf("max level reached\n");
        return;
    }
    struct head *bud = buddy(block);
    if (bud->status == Free) {
        unlink_block(block);
        unlink_block(bud);
        struct head *prim = primary(block);
        link_block(prim);
        merge(prim);
    }
}

void insert(struct head *block) {
    if (block->level == LEVELS-1) { // 4KB block
        printf("!!!4KB BLOCK!!!\nthe block should be destoryed/split up/reclaimed by the OS\n");
        int level = block->level;
        block->prev = NULL;
        block->next = flists[level];
        if (flists[level] != NULL) {
            flists[level]->prev = block;
        }
        flists[level] = block;
    } else {
        struct head *friend = buddy(block);
        if (friend->status == Taken) { // No free buddy
            int level = block->level;
            block->prev = NULL;
            block->next = flists[level];
            if (flists[level] != NULL) {
                flists[level]->prev = block;
            }
            flists[level] = block;
        } else {                    // Has a free buddy - merge time!
            merge(block);
        }
    }
}


void test_headers(struct head *mem) {
    if (mem == NULL) {
        printf("ERROR: NULL HEADER\n");
        return;
    }
    char status[20];
    if (mem->status == Free) {
        strcpy(status, "Free");
    } else if (mem->status == Taken) {
        strcpy(status, "Taken");
    } else {
        strcpy(status, "Unknown");
    }
    printf("_________________________________\n");
    printf("| Block: %p\t\t|\n", mem);
    printf("| mem->status:%s\t\t|\n", status);
    printf("| mem->level: %d (%d B)\t%s|\n", mem->level, (int) pow(2, mem->level+MIN), (int) pow(2, mem->level+MIN) > 999 ? "" : "\t");
    printf("| mem->next:  %p\t%s|\n", mem->next, mem->next == NULL ? "\t" : "");
    printf("| mem->prev:  %p\t%s|\n", mem->prev, mem->prev == NULL ? "\t" : "");
    printf("--------------------------------\n");
    /* BEWARE THE VISUAL HAX FOR EVEN BORDERS B) */
}

void print_mem() {
    int i;
    struct head *curr;
    for (i = 0 ; i < LEVELS; i++) {
        printf("\nLevel %d: \n", i);
        if (flists[i] != NULL) {
            curr = flists[i];
            while (curr != NULL) {
                test_headers(curr);
                curr = curr->next;
            }
        }
    }
}

void test() {
    // Test the headers
    printf("\n=== Create ===\n");
    struct head *mem = new();
    test_headers(mem);
    printf("\n=== Obfuscate ===\n");
    struct head *obfsk_mem = (struct head *) hide(mem);
    test_headers(obfsk_mem);
    printf("\n=== Recover ===\n");
    struct head *recovered_mem = (struct head *) magic(obfsk_mem);
    test_headers(recovered_mem);

    // Divide it
    printf("\n=== Splitting ===\n");
    struct head *halved = split(mem);
    test_headers(mem);
    test_headers(halved);
    
    printf("\n=== Splitting AGAIN ===\n");
    struct head *quarter = split(halved);
    printf("quarter:\n");
    test_headers(quarter);
    printf("halve:\n");
    test_headers(halved);

    printf("\n=== Primary ===\n");
    struct head *prim = primary(quarter);
    test_headers(prim);
}

void dyn_inter_alloc() {
    int mem_size = 0;
    do {
        printf("Allocate how much mem? (quit=0) ");
        scanf("%d", &mem_size);
        balloc(mem_size);
        print_mem();
    } while(mem_size);
}

void test2() {
    insert(new());
    print_mem();
    dyn_inter_alloc();
    // int memsize = 2;
    // printf("Allocating %d B of memory (lvl %d", memsize, level(memsize));
    // balloc(memsize);
    // memsize = 16;
    // printf("Allocating %d B of memory (lvl %d", memsize, level(memsize));
    // balloc(memsize);
    // memsize = 40;
    // printf("Allocating %d B of memory (lvl %d", memsize, level(memsize));
    // balloc(memsize);
    // memsize = 100;
    // printf("Allocating %d B of memory (lvl %d", memsize, level(memsize));
    // balloc(memsize);
    // memsize = 300;
    // printf("Allocating %d B of memory (lvl %d", memsize, level(memsize));
    // balloc(memsize);
    // memsize = 800;
    // printf("Allocating %d B of memory (lvl %d", memsize, level(memsize));
    // balloc(memsize);
    // memsize = 1000;
    // print_mem();
    
    // printf("==== Inserted a new blcok ====");
    // insert(new());
    // print_mem();
    // char *myMem = balloc(20*sizeof(int));
    // strcpy(myMem, "hejsan");
    // printf("meddelande: %s\n", myMem);
    // printf("=== HEADER TESTS ===\n");
    // test_headers(magic(myMem));
    // print_mem();
    // bfree(myMem);
    // print_mem();
}