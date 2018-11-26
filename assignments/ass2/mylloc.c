#include <stdlib.h>
#include <unistd.h>

void *malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    void *memory = sbrk(size);
    return (memory == (void *) -1) ? NULL : memory;
}

void free(void *memory) {
    return;
}