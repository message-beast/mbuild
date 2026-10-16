#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200809L
#endif
#include "basic.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>
#define SHM_NAME "/proc_status"
#include "utils.h"

static inline __attribute__((always_inline, hot, aligned(64))) int run(mexec* exec) {
    int fd = shm_open(SHM_NAME, O_RDWR, 0644);
    if (__builtin_expect(fd == -1, 0)) exit_program(-1);
    char* statusBuff = mmap(NULL, 2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (__builtin_expect(statusBuff == MAP_FAILED, 0)) exit_program(-1);
    pid_t pid = fork();
    if (pid == 0) {
        statusBuff[0] = '0';
        statusBuff[1] = '\0';
        execvp(exec->commands[0], exec->commands);
        statusBuff[0] = '1';
        statusBuff[1] = '\0';
        abort();
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (__builtin_expect(WEXITSTATUS(status) != 0, 0)) {
            perror("child failed!\n");
            goto cleanup;
        }
        if (__builtin_expect(strcmp(statusBuff, "0") != 0, 0)) goto cleanup;
    }
    return 0;
    cleanup:
        munmap(statusBuff, 2);
        shm_unlink(SHM_NAME);
        return -1;

}



__attribute__((hot, aligned(64))) int runTarget(mexec* __restrict__ execs, char* __restrict__ target) {
    mexec* exec = execs;
    while (exec != NULL) {
        if (__builtin_expect(strcmp(execs->target, target) == 0, 0)) {
            return run(exec);
        };
    }
    fprintf(stderr, "failed to find any target with name %s\n", target);
    return -1;
}
