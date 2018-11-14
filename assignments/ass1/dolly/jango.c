#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int pid = fork();

    if (pid == 0) {
        int fd = open("quotes.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        dup2(fd, 1);
        close(fd);
        execl("redirect.o", "redirect.o", NULL); // WEOW
        printf("exec failed\n");
    } else {
        wait(NULL);
    }

    return 0;
}