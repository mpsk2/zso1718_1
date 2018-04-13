#include <time.h>
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
#include <elf.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../include/alienos.h"
#include "../include/debug.h"
#include "../include/display.h"

int change_elf(pid_t child, int argc, char **argv) {
    int r = 0;
    int found = -1;
    struct stat st;
    int f;
    char *p;
    if (stat(argv[1], &st) != 0) {
        r = 1;
        goto change_elf_end;
    }

    f = open(argv[1], O_RDONLY);
    if (f == 0) {
        r = 1;
        goto change_elf_end;
    }
    p = mmap(0, (size_t) st.st_size, PROT_READ, MAP_PRIVATE, f, 0);
    if (p == 0) {
        r = 1;
        goto change_elf_file_close_end;
    }

    Elf64_Ehdr *ehdr = (Elf64_Ehdr*) p;
    Elf64_Phdr *phdr = (Elf64_Phdr*) (p + ehdr->e_phoff);
    int phnum = ehdr->e_phnum;

    Elf64_Phdr *ph_strtab = &phdr[ehdr->e_phentsize];
    const char *const ph_strtab_p = p + ph_strtab->p_offset;

    for (int i = 0; i < phnum; i++) {
        if (phdr[i].p_type == PT_PARAMS) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        r = 1;
        goto change_elf_p_end;
    }

    debug("Section number is %d\n"
          "\ttype: %x\n"
          "\tflags: %x\n"
          "\toffset: %x\n"
          "\tpaddr: %x\n"
          "\tfilesz: %x\n"
          "\tmemsz: %x\n"
          "\t\tArguments number %d\n"
          "\talign: %x\n",
          found,
          phdr[found].p_type,
          phdr[found].p_flags,
          phdr[found].p_offset,
          phdr[found].p_paddr,
          phdr[found].p_filesz,
          phdr[found].p_memsz,
          phdr[found].p_memsz / 4,
          phdr[found].p_align
    );

    int32_t* localv;
    struct iovec local_cpy;
    struct iovec remote_cpy;

    localv = (int32_t*) malloc(phdr[found].p_memsz);

    for (int i = 0; (i < phdr[found].p_memsz / 4) && (i < argc - 2); i++) {
        int32_t n = (int32_t) strtoll(argv[i + 2], NULL, 10);
        localv[i] = n;
    }

    local_cpy.iov_base = localv;
    local_cpy.iov_len = phdr[found].p_memsz;
    remote_cpy.iov_base = (void*) phdr[found].p_paddr;
    remote_cpy.iov_len = local_cpy.iov_len;
    process_vm_writev(child, &local_cpy, 1, &remote_cpy, 1, 0);

change_elf_p_end:
    munmap(p, (size_t) st.st_size);
change_elf_file_close_end:
    close(f);
change_elf_end:
    return r;
}

int main(int argc, char **argv) {
    pid_t child;
    long r;
    int exit_status = EXIT_SUCCESS;

    child = fork();

    if (child == 0) {
        // CHILD!!
        r = ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        if (r == -1) {
            debug("ptrace PTRACE_TRACEME fails with %d\n", errno);
            exit(1);
        }
        r = execl(argv[1], argv[1], NULL);
        if (r == -1) {
            debug("execv fails with %d\n", errno);
            exit(1);
        }
    } else {
        void* localv;
        struct iovec local_cpy;
        struct iovec remote_cpy;
        int was_execve = 0;
        int enter = 0;
        int status;
        struct user_regs_struct regs;
        if (display_init() != 0 ) {
            goto fail;
        }
        srand((unsigned int) time(NULL));
        while (1) {
            wait(&status);
            if (WIFEXITED(status)) {
                break;
            }
            r = ptrace(PTRACE_GETREGS, child, NULL, &regs);
            if (r == -1) {
                exit(1);
            }
            if (enter == 0) {
                enter = 1;
                switch (regs.orig_rax) {
                    case -1:
                        goto fail;
                    case ALIENOS_END:
                        debug("End with status %llu\n", regs.rdi);
                        exit_status = regs.rdi;
                        goto winclose;
                    case ALIENOS_GETRAND:
                        debug("Get rand, return as RAX\n");
                        uint32_t n = (uint32_t) rand();
                        r = ptrace(PTRACE_POKEUSER, child, RAX * 8, n);
                        if ( r == -1 ) {
                            goto fail;
                        }
                        break;
                    case ALIENOS_GETKEY:
                        debug("Get key\n");
                        int c;
                        if (display_read_char(&c) != 0) {
                            goto fail;
                        }
                        r = ptrace(PTRACE_POKEUSER, child, RAX * 8, c);
                        if ( r == -1 ) {
                            goto fail;
                        }
                        enter = 0;
                        break;
                    case ALIENOS_PRINT:
                        debug("print(x=%llu, y=%llu)\n", regs.rdi, regs.rsi);
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
                        display_move_cursor((int) regs.rdi, (int) regs.rsi);
                        break;
                    case 59:
                        if (change_elf(child, argc, argv) != 0) {
                            goto fail;
                        }
                        if (was_execve == 0) {
                            was_execve = 1;
                        } else {
                            errno = EINVAL;
                            goto fail;
                        }
                        break;
                    default:
                        debug("Unknown %llu\n", regs.rax);
                }
            } else {
                if (was_execve == 1) {
                    was_execve = 2;
                }
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