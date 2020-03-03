#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void do_write(void)
{
    char str[] = "write me to stdout\n";

    /* write(2) is a simple wrapper around a syscall so it should be easy to
     * find in the syscall trace. */
    if (sizeof(str) != write(STDOUT_FILENO, str, sizeof(str))){
        perror("write");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    /* gcc -Wall -Wextra (and we should always go for both) doesn't like unused
     * parameters so we silence it. */
    (void) argc; (void) argv;

    do_write();

    /* Notice the return values. Process return values are not very well defined
     * in Linux and across UNIX-likes in general. Generally it's 0 for success
     * and any other values. So it's easier to just stick with standard macros
     * provided by stdlib. */
    return EXIT_SUCCESS;
}
