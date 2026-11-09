#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200809L
#endif
#include "parsemall.h"
#include "config.h"
#include "basic.h"
#include "utils.h"
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/wait.h>
#define SHM_NAME "/dproc_status"
int runDmall() {
    for (register int i = 0; i < numberOfDBuilds; ++i) {
        int statusFd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0644);
        if (__builtin_expect(statusFd == -1, 0)) {
            perror("failed to open shared memory!\n");
            exit_program(-1);
        }
        if (__builtin_expect(ftruncate(statusFd, 2) != 0, 0)) {
            perror("ftruncate failed on shared memory!\n");
            close(statusFd);
            exit_program(-1);
        }
        char* status = mmap(NULL, 2, PROT_READ | PROT_WRITE, MAP_SHARED, statusFd, 0);
        if (__builtin_expect(status == MAP_FAILED, 0)) {
            close(statusFd);
            exit_program(-1);
        }
        int pid = fork();
        if (pid == 0) {
            if (__builtin_expect(strcmp(_dmall[i]->directory, ".") != 0 && chdir(_dmall[i]->directory) != 0, 0)) {
                fprintf(stderr, "failed to change directory to: %s\n", _dmall[i]->directory);
                exit_program(-1);
            }
            status[0] = '0';
            status[1] = '\0';
            printf("\033[32mrunning: \033[35m%s\033[0m\n", _dmall[i]->target);
            execlp("mbuild", "mbuild", _dmall[i]->target, NULL);
            status[0] = '1';
            abort();
        } else {
            int childStatus;
            waitpid(pid, &childStatus, 0);
            if (__builtin_expect(WEXITSTATUS(childStatus) != 0, 0)) {
                fprintf(stderr, "\033[31mprocess failed with status \033[32m%i\033[0m\n", childStatus);
                exit_program(-1);
            }
            if (__builtin_expect(status[0] != '0', 0)) {
                fprintf(stderr, "\033[31mfailed to run target6765 \033[36m%s\033[0m\n", _dmall[i]->target);
                exit_program(-1);
            }
        }
        munmap(status, 2);
        shm_unlink(SHM_NAME);
    }
    printf("\033[32mBUILD FININSHED SUCCESSFULLY!\033[0m\n");
    return 0;
}
