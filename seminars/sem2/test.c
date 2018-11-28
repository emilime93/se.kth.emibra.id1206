#include <stdio.h>
#include "buddy.h"

#define ROUNDS 10

int main(int argc, char const *argv[])
{
    printf("malloc & free\n");
    test_malloc(ROUNDS);
    printf("--------------\n");
    printf("balloc & bfree\n");
    test_balloc(ROUNDS);
    return 0;
}