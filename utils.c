#include <stdlib.h>
#include "basic.h"
void __attribute__((noreturn)) panic(int i) {
    if (i >= 0) exit(i);
    abort();
}


#define exit_program(i) panic(i)


__attribute__((hot, aligned(64), reproducable)) void freeMExecs(mexec** execs) {
    mexec* exec = *execs;
    while (exec != NULL) {
        for (register int i = 0; i < exec->argsNumber; ++i) {
            free(exec->commands[i]);
        }
        free(exec->commands);
        free(exec->target);
        exec = exec->next;
    }
}
