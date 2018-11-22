#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int rc = fork();

    if (rc == 0) {
        printf("check the status\n");
        sleep(10);
        printf("and again\n");
        return 42;
    } else {
        sleep(20);
        int res;
        wait(&res);
        printf("my child ended with exit status %d\n", WEXITSTATUS(res));
    }
}