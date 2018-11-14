#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("pre fork, parent id: %d\n", getpid());
    int rc = fork();

    if (rc == 0) {
        printf("I'm the child: %d\n", getpid());
        sleep(3);
    } else {
        printf("My child is called: %d\n", rc);
        wait(NULL);
        printf("My child has died\n");
    }

    return 0;
}