#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

volatile int count;

void handler(int sig) {
    printf("signal %d was caught\n", sig);
    // exit(1);
    // return;
}

int not_so_good() {
    int x = 0;
    return 1 % x;
}

int main(int argc, const char *argv[]) {
    struct sigaction sa;

    printf("ok, let's go. I'll catch my own error!\n");

    sa.sa_handler = handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGFPE, &sa, NULL);

    not_so_good();

    printf("Won't survive this long...\n");

    return(0);
}