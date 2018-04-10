#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <stdio.h>

int main(int argc, char** argv) {
    pid_t child;
    long orig_rax, rax;
    long params[4];
    int status;
    int insyscall = 0;

    fprintf(stderr, "Run in debug mode.\nRun with args: \n");
    for (int i = 1; i < argc; i++) {
        fprintf(stderr, "\t%s\n", argv[i]);
    }

    child = fork();

    if ( child == 0 ) {
        fprintf(stderr, "I am child process\n");
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl(argv[1], argv[1], NULL);
    } else {
        while (1) {
            fprintf(stderr, "Before wait\n");
            wait(&status);
            fprintf(stderr, "After wait, status=%d\n", status);
            if (WIFEXITED(status)) {
                break;
            }
            orig_rax = ptrace(PTRACE_PEEKUSER, child, 8 * ORIG_RAX, NULL);
            fprintf(stderr, "%ld\n", orig_rax);
            if (orig_rax == SYS_write) {
                if (insyscall == 0) {
                    insyscall = 1;
                    params[0] = ptrace(PTRACE_PEEKUSER, child, 8 * RDI, NULL);
                    params[1] = ptrace(PTRACE_PEEKUSER, child, 8 * RSI, NULL);
                    params[2] = ptrace(PTRACE_PEEKUSER, child, 8 * RDX, NULL);
                    params[3] = ptrace(PTRACE_PEEKUSER, child, 8 * R10, NULL);
                    fprintf(stderr, "Params: %ld, %ld, %ld, %ld\n", params[0], params[1], params[1], params[2], params[3]);
                }
            } else {
                rax = ptrace(PTRACE_PEEKUSER, child, 8 * RAX, NULL);
                fprintf(stderr, "%ld\n", rax);
                insyscall = 0;
            }
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
        }
    }
    return 0;
}