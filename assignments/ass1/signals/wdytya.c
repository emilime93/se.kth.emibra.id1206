// Who Do You Think You Are?
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int done = 0;

void handler(int sig, siginfo_t *siginfo, void *context) {
    printf("signal %d was caight\n", sig);
    printf("your UID is %d\n", siginfo->si_uid);
    printf("your PID is %d\n", siginfo->si_pid);

    done = 1;
}

int main(int argc, char *argv[]) {
    struct sigaction sa;

    int pid = getpid();
    printf("Ok, let's go - kill me (%d) and I'll tell you who are\n", pid);

    /* we're using the more elaborated signaction handler */
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;

    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) != 0) {
        return(1);
    }

    while(!done) {}

    printf("Told you so!\n");

    return 0;
}