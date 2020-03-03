#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

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
     * The filename template will be modified by mkstemp so we cannot allocate
     * it statically.
     * */
    char tmp_filename_template[] = "/tmp/output_fileXXXXXX";

    /*
     * Create a temporary file and open it in read/write mode.
     * */
    int out_fd = mkstemp(tmp_filename_template);
    if (out_fd == -1) {
        perror("mkstemp");
        exit(EXIT_FAILURE);
    }

    do_write(out_fd);

    return EXIT_SUCCESS;
}
