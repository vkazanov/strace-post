#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void do_write(const char *str, ssize_t len)
{
    if (len != write(STDOUT_FILENO, str, (size_t)len)){
        perror("write");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    (void) argc; (void) argv;

    char str1[] = "write me 1\n";
    do_write(str1, sizeof(str1));

    char str2[] = "write me 2\n";
    do_write(str2, sizeof(str2));

    return EXIT_SUCCESS;
}
