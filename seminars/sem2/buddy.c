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
void print_lvl(int);
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
        printf("MMAP failed.\n");
        return NULL;
    }
    assert(((long int)new & 0xfff) == 0); // last 12 bits should be zero
    new->status = Free;
    new->level = LEVELS - 1;
    return new;
}

void reclaim_mem(struct head *block) {
    int rc = munmap((void *) block, PAGE);
    if (rc != 0) {
        printf("Couldn't reclaim memory to to OS\n");
        exit(1);
    }
}

/* Returns the buddy block independently of if its the primary or suplementary one pair */
struct head *buddy(struct head *block) {
    int index = block->level;
    long int mask = 0x1 << (MIN + index);
    return (struct head*)((long int) block ^ mask); // ^ == XOR
}

/* returns the address to the secondary block with correct headers, and sets the main block headers */
struct head *split(struct head *block) {
    int index = block->level - 1;
    int mask = 0x1 << (index + MIN);
    return (struct head*) ((long int) block | mask);
}

/* Returns the primary block from any of the pair blocks */
struct head *primary(struct head *block) {
    int index = block->level;
    long int mask = 0xffffffffffffffff << (MIN + index + 1);
    return (struct head*) ((long int) block & mask);
}

/* Gives the address to the next address to hide access to the header */
void *hide(struct head* block) {
    return (void *) (block + 1 );
}

/* Decrements the address one step in order to retrive the header from a block */
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
    printf("balloc\n");
    if (size == 0) {
        return NULL;
    }
    int index = level(size);
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
    } else { // Not first..
        if (block->next == NULL) { // I'm last
            block->prev->next = NULL;
        } else { // Link me out from the middle of a sandwhich
            block->next->prev = block->prev;
            block->prev->next = block->next;
        }
    }
}

/* Strictly inserts a block in the beginning of a linled list, at the level that the block has */
void link_block(struct head *block) {
    int level = block->level;
    block->next = flists[level];
    if (flists[level] != NULL) {
        flists[level]->prev = block;
    }
    block->prev = NULL;
    flists[level] = block;
}

/* splits up the blocks from level-level to goal */
/* first level guaranteed by caller to have a block */
/* and subsequent levels are by logic guaranteed to contain data */
void split_up(int level, int goal) {
    if (level == goal) {
        return;
    }
    // Unlink the block from list
    struct head *block = flists[level];
    unlink_block(block);


    // Split it up
    struct head *supl = split(block);
    supl->level = block->level - 1;
    supl->status = Free;
    
    block->level--;

    // Link these into the lower level.
    link_block(supl);
    link_block(block);
    
    // Continue
    split_up(level - 1, goal);
}

/* fetches a block of the level specified in the parameter */
struct head *find(int level) {
    if (flists[level] == NULL) {
        if (level == LEVELS - 1) {
            printf("Allocating all new memory!\n");
            insert(new());
        } else {
            int lvl_w_mem = level + 1;
            while(flists[lvl_w_mem] == NULL && lvl_w_mem < LEVELS) {
                lvl_w_mem++;
            }
            split_up(lvl_w_mem, level);
        }
    }
    struct head *alloc = flists[level];
    unlink_block(alloc);
    alloc->status = Taken;
    return alloc;
}

void merge(struct head *block) {
    if (block->level == LEVELS - 1) {
        link_block(block);
        return;
    }
    if (buddy(block)->status == Free) {
        unlink_block(buddy(block));
        struct head *prim = primary(block);
        prim->level++;
        prim->status = Free;
        merge(prim);
    } else {
        link_block(block);
    }
}

/* Sets a block status to free, and links it into the structure */
void insert(struct head *block) {
    if (block->level == LEVELS - 1) { // 4KB block
        if (flists[block->level] == NULL) {
            block->status = Free;
            link_block(block);
        } else {
            printf("Extra 4KB block. Reclaiming to the OS\n");
            reclaim_mem(block);
        }
    } else {
        if (buddy(block)->status == Taken) { // No free buddy
            block->status = Free;
            link_block(block);
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

void print_lvl(int lvl) {
    printf("\nLevel %d: \n", lvl);
    struct head *curr;
    if (flists[lvl] != NULL) {
        curr = flists[lvl];
        while (curr != NULL) {
            test_headers(curr);
            curr = curr->next;
        }
    }
}

void print_mem() {
    printf("\n=== MEMORY STRUCTURE ===\n");
    int i;
    for (i = 0 ; i < LEVELS; i++) {
        print_lvl(i);
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
    dyn_inter_alloc();

    char *str1 = balloc(sizeof(char) * 50);
    print_mem();

    char *str2 = balloc(sizeof(char) * 700);
    print_mem();

    char *str3 = balloc(sizeof(char) * 8);
    print_mem();

    char *str4 = balloc(sizeof(char) * 200);
    print_mem();

    char *str5 = balloc(sizeof(char) * 20);
    print_mem();

    char *str6 = balloc(sizeof(char) * 50);
    print_mem();
    
    char *str7 = balloc(sizeof(char) * 40);
    print_mem();

    printf("-----------------------------------------------------------------");
    bfree(str7);
    print_mem();
    printf("-----------------------------------------------------------------");
    bfree(str1);
    print_mem();
    printf("-----------------------------------------------------------------");
    bfree(str6);
    print_mem();
    printf("-----------------------------------------------------------------");
    bfree(str5);
    print_mem();
    printf("-----------------------------------------------------------------");
    bfree(str4);
    print_mem();
    printf("-----------------------------------------------------------------");
    bfree(str3);
    print_mem();
}