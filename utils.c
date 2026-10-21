#include <stdlib.h>
#include "basic.h"
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
