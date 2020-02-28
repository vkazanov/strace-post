#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *thread(void *arg)
{
    (void) arg;

    printf("Secondary thread: working\n");

    return NULL;
}

int main(int argc, char *argv[])
{
    (void) argc; (void) argv;

    /* POSIX threads are a rather fat layer above the kernel.  */
    printf("Initial thread: launching a thread\n");

    /* What syscalls are used for thread creation? */
    pthread_t thr;
    if (0 != pthread_create(&thr, NULL, thread, NULL)) {
        fprintf(stderr, "Initial thread: failed to create a thread");
        exit(EXIT_FAILURE);
    }
    printf("Initial thread: message from the initial thread\n");

    /* What syscalls are used for thread joining? */
    printf("Initial thread: joining a thread\n");
    if (0 != pthread_join(thr, NULL)) {
        fprintf(stderr, "Initial thread: failed to join a thread");
        exit(EXIT_FAILURE);
    };

    printf("Initial thread: done");

    exit(EXIT_SUCCESS);
}
