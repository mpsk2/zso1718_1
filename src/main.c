#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <errno.h>
#include <inttypes.h>

#include "../include/alienos.h"
#include "../include/debug.h"
#include "../include/display.h"

#define ALIENOS_END 0
#define ALIENOS_GETRAND 1
#define ALIENOS_GETKEY 2
#define ALIENOS_PRINT 3
#define ALIENOS_SETCURSOR 4

int read_char() {
    // TODO, returns key up
    return KEY_UP;
}

int main(int argc, char **argv) {
    pid_t child;
    struct user_regs_struct regs;
    int status;
    long r;
    int was_execve = 0;
    int enter = 0;
    int exit_status = EXIT_SUCCESS;
    void* localv;
    struct iovec local_cpy;
    struct iovec remote_cpy;

    display_init();

    child = fork();

    if (child == 0) {
        // CHILD!!
        r = ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        if (r == -1) {
            debug("ptrace PTRACE_TRACEME fails with %d\n", errno);
            exit(1);
        }
        r = execlp("./hello", "hello", NULL);
        if (r == -1) {
            debug("execv fails with %d\n", errno);
            exit(1);
        }
    } else {
        while (1) {
            wait(&status);
            if (WIFEXITED(status)) {
                debug("Break by status\n");
                break;
            }
            r = ptrace(PTRACE_GETREGS, child, NULL, &regs);
            if (r == -1) {
                debug("ptrace PTRACE_PEEKUSER fails with %d\n", errno);
                exit(1);
            }
            if (enter == 0) {
                enter = 1;
                switch (regs.orig_rax) {
                    case -1:
                        debug("Error %d\n", errno);
                        exit(1);
                        break;
                    case ALIENOS_END:
                        debug("End with status %llu\n", regs.rdi);
                        exit((int) regs.rdi);
                        break;
                    case ALIENOS_GETRAND:
                        debug("Get rand, return as RAX");
                        // TODO
                        break;
                    case ALIENOS_GETKEY:
                        debug("Get key\n");
                        int c = read_char();
                        // TODO
                        r = ptrace(PTRACE_POKEUSER, child, ORIG_RAX * 8, c);
                        if ( r == -1 ) {
                            goto fail;
                        }
                        enter = 0;
                        break;
                    case ALIENOS_PRINT:
                        localv = malloc(sizeof(uint16_t) * regs.r10);
                        local_cpy.iov_base = localv;
                        local_cpy.iov_len = sizeof(uint16_t) * regs.r10;
                        remote_cpy.iov_base = (void*) regs.rdx;
                        remote_cpy.iov_len = local_cpy.iov_len;
                        process_vm_readv(child, &local_cpy, 1, &remote_cpy, 1, 0);
                        display_show(regs.rdi, regs.rsi, (uint16_t*) localv, regs.r10);
                        free(localv);
                        break;
                    case ALIENOS_SETCURSOR:
                        debug("Set cursor(x=%llu, y=%llu)\n", regs.rdi, regs.rsi);
                        // TODO
                        break;
                    case 59:
                        if (was_execve == 0) {
                            was_execve = 1;
                        } else {
                            goto fail;
                        }
                        break;
                    default:
                        debug("Unknown %llu\n", regs.rax);
                }
            } else {
                // LEAVE
                // DO STUFF !!!!! TODO
                enter = 0;
            }
            r = ptrace(PTRACE_SYSCALL, child, NULL, NULL);
            if (r == -1) {
                goto fail;
            }
        }
    }
    goto winclose;

fail:
    debug("fail to do operation with errno=%d\n", errno);
    exit_status = EXIT_FAILURE;
winclose:
    display_close();
    exit(exit_status);
}