#include <stdlib.h>
#include "basic.h"
#include "config.h"
#include "parsemall.h"

void __attribute__((noreturn)) panic(int i) {
    if (i >= 0) exit(i);
    abort();
    __builtin_unreachable();
}


#define exit_program(i) panic(i)


__attribute__((hot, aligned(64), reproducible)) void freeMExecs(mexec** execs) {
    if (__builtin_expect(execs == NULL, 0)) return;
    mexec* exec = *execs;
    for (;exec != NULL;) {
        for (register int i = 0; i < exec->argsNumber; ++i) {
            free(exec->commands[i]);
        }
        free(exec->commands);
        free(exec->target);
        mexec* before = exec;
        exec = exec->next;
        free(before);
    }
    exec = NULL;
}


__attribute__((hot, aligned(64), reproducible)) void freeGroupExecs(groupexec** groupexecs) {
    if (__builtin_expect(groupexecs != NULL, 1)) {
        for (groupexec* execs = *groupexecs;execs != NULL;) {
            for (register int i = 0; i < execs->numbers; ++i) {
                free(execs->targets[i]);
            }
            free(execs->targets);
            free(execs->name);
            groupexec* before = execs;
            execs = execs->next;
            free(before);
        }
    }
}

__attribute__((hot, aligned(64))) void freeDmall() {
    if (__builtin_expect(_dmall != NULL, 1)) {
        for (register int i = 0; i < numberOfDBuilds; ++i) {
            free(_dmall[i]->directory);
            free(_dmall[i]->target);
            free(_dmall[i]);
        }
        free(_dmall);
        _dmall = NULL;
    }
}
