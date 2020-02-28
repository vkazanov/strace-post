#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    (void) argc; (void) argv;

    pid_t parent_pid = getpid();

    /* Ignore possible error conditions here. */
    pid_t child_pid = fork();
    if (child_pid == 0) {
        /* A child is born! */
        child_pid = getpid();

        /* In the end of the day printf is just a call to write(2). */
        printf("child (self=%d)\n", child_pid);
        exit(EXIT_SUCCESS);
    }

    /* Business as usual for the parent. */
    printf("parent (self=%d, child=%d)\n", parent_pid, child_pid);

    /* Wait for children termination. Notice how wait() is a wrapper around the
     * wait4(2) syscall. */
    wait(NULL);

    exit(EXIT_SUCCESS);
}
