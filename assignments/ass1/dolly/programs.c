#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int pid = fork();

    if (pid == 0) {
        execlp("ls", "ls", NULL);
        printf("this will only happen if exec fails..\n");
    } else {
        wait(NULL);
        printf("we're done here\n");
    }
    return 0;
}