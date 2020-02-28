#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    (void) argc; (void) argv;

    char str[] = "write me\n";

    write(STDOUT_FILENO, str, sizeof(str));

    /* Sleep indefinitely or until a signal arrives */
    pause();

    write(STDOUT_FILENO, str, sizeof(str));

    return EXIT_SUCCESS;
}
