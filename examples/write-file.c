#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

void do_write(int out_fd)
{
    char str[] = "write me to a file\n";

    if (sizeof(str) != write(out_fd, str, sizeof(str))){
        perror("write");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    (void) argc; (void) argv;

    /*
     * Path will be provided by the first program argument.
     *  */
    const char *path = argv[1];

    /*
     * Open an existing file for writing in append mode.
     *  */
    int out_fd = open(path, O_APPEND | O_WRONLY);
    if (out_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    do_write(out_fd);

    return EXIT_SUCCESS;
}
