#include <stdio.h>
#include "basic.h"
#include "parse.h"
#include "utils.h"
mexec* execs = NULL;

__attribute__((constructor, no_reorder)) void initSystem() {
    mexec** mexecs = parseFile();
    if (__builtin_expect(mexecs == NULL, 0)) exit_program(-1);
    execs = *(mexecs);
}


__attribute__((destructor, no_reorder)) void closeSystem() {
    freeMExecs(&execs);
}


int main() {
    return 0;
}
