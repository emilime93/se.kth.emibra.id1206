#include <stdio.h>

int main(int argc, char* argv[]) {
    foo:
    printf("the code: %p\n", &&foo);
    fgetc(stdin);
    return 0;
}