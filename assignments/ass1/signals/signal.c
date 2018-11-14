#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

volatile int count;

void handler(int sig) {
    printf("the signal %d ouch that hurt\n", sig);
    count++;
}

int main(int argc, const char *argv[]) {
    struct sigaction sa;

    int pid = getpid();

    printf("ok, let's go, kill me (%d) if you can!\n", pid);

    sa.sa_handler = handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) != 0) {
        return(1);
    }

    // if (sigaction(SIGKILL, &sa, NULL) != 0) {
    //     return(1);
    // }

    while (count != 4) {}

    printf("I've had enough!\n");
    return(0);
}