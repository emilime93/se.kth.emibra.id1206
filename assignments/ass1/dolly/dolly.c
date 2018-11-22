#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int x = 123;
    int rc = fork();

    if (rc == 0) {
        printf("child: x is %d\n", x);
        x = 42;
        sleep(1);
        printf("child: x is %d\n", x);
    } else {
        printf("mother: x is %d\n", x);
        x = 13;
        sleep(1);
        printf("mother: x is %d\n", x);
    }
    return 0;
}