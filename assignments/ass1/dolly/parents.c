#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int rc = fork();

    if (rc == 0) {
        int child = getpid();
        printf("I'm the child %d with parent %d, group: %d\n", child, getppid(), getpgid(child));
    } else {
        int parent = getpid();
        printf("I'm the parent %d with parent %d, group: %d\n", parent, getppid(), getpgid(parent));
        wait(NULL);
    }
}