#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <err.h>
#include <sys/user.h>
#include <sys/ptrace.h>
#include <syscall.h>

/* A system call table */
#include "piglet-trace-syscalls.h"

void print_syscall_enter(uint64_t syscall_num)
{
    if (syscall_num < sizeof(syscall_to_name) / sizeof(syscall_to_name[0]))
        fprintf(stderr, "%s(%"PRIu64")", syscall_to_name[syscall_num], syscall_num);
    else
        fprintf(stderr, "unknown(%"PRIu64")", syscall_num);
}

void print_syscall_exit(uint64_t return_value)
{
    fprintf(stderr, " -> %"PRIu64"\n", return_value);
}

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        fprintf(stderr, "Usage: tiny-trace <tracee> [arg1 [arg12] ...]\n");
        exit(EXIT_FAILURE);
    }

    pid_t child_pid = fork();
    switch (child_pid) {
    case -1:
        err(EXIT_FAILURE, "fork");
    case 0:
        /* Child here */

        /* A traced mode has to be enabled. A parent will have to wait(2) for it
         * to happen. */
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);

        /* Replace itself with a program to be run. */
        execvp(argv[1], argv + 1);
        err(EXIT_FAILURE, "exec");
    }

    /* Parent here */

    /* First we wait for the child to set the traced mode (see
     * ptrace(PTRACE_TRACEME) above) */
    waitpid(child_pid, NULL, 0);

    /* A non-portable structure defined for ptrace/GDB/strace usage mostly. It
     * allows to conveniently dump and access register state using ptrace. */
    struct user_regs_struct registers;

    /* A system call tracing loop. */
    for (;;) {
        /* Enter syscall: continue execution until the next system call
         * beginning. Stop right before syscall.
         *
         * It's possible to change the system call number, system call
         * arguments, return value or even avoid executing the system call
         * completely. */
        if (ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL) == -1)
            err(EXIT_FAILURE, "enter_syscall");
        if (waitpid(child_pid, NULL, 0) == -1)
            err(EXIT_FAILURE, "enter_syscall -> waitpid");

        /* According to the x86-64 system call convention on Linux (see man 2
         * syscall) the number identifying a syscall should be put into the rax
         * general purpose register, with the rest of the arguments residing in
         * other general purpose registers (rdi,rsi, rdx, r10, r8, r9). */
        if (ptrace(PTRACE_GETREGS, child_pid, NULL, &registers) == -1)
            err(EXIT_FAILURE, "enter_syscall -> getregs");

        /* Note how orig_rax is used here. That's because on x86-64 rax is used
         * both for executing a syscall, and returning a value from it. To
         * differentiate between the cases both rax and orig_rax are updated on
         * syscall entry/exit, and only rax is updated on exit. */
        print_syscall_enter(registers.orig_rax);

        /* Exit syscall: execute of the syscall, and stop on system
         * call exit.
         *
         * More system call tinkering possible: change the return value, record
         * time it took to finish the system call, etc. */
        if (ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL) == -1)
            err(EXIT_FAILURE, "exit_syscall");
        if (waitpid(child_pid, NULL, 0) == -1)
            err(EXIT_FAILURE, "exit_syscall -> waitpid");

        /* Retrieve register state again as we want to inspect system call
         * return value. */
        if (ptrace(PTRACE_GETREGS, child_pid, NULL, &registers) == -1) {
            /* ESRCH is returned when a child terminates using a syscall and no
             * return value is possible, e.g. as a result of exit(2). */
            if (errno == ESRCH) {
                fprintf(stderr, "\nTracee terminated\n");
                break;
            }
            err(EXIT_FAILURE, "exit_syscall -> getregs");
        }

        /* Done with this system call, let the next iteration handle the next
         * one */
        print_syscall_exit(registers.rax);
    }

    return EXIT_SUCCESS;
}
