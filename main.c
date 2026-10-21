#include <stdio.h>
#include "basic.h"
#include "parse.h"
#include "utils.h"
#include "help.h"
#include "run.h"
#include "config.h"
__attribute__((section(".data"))) mexec* execs = NULL;


__attribute__((constructor, no_reorder)) void initSystem() {
    mexec* mexecs = parseFile();
    if (__builtin_expect(mexecs == NULL && buildFileflag == 0, 0)) exit_program(-1);
    if (__builtin_expect(mexecs != NULL, 1)) {
        execs = mexecs;
    } else {
        exit_program(-1);
    }
}


__attribute__((destructor, no_reorder)) void closeSystem() {
    freeMExecs(&execs);
}









int main(int argc, char** argv) {
    char* arg1 = argv[1];
    if (__builtin_expect(strcmp(arg1, "--help") == 0 || strcmp(arg1, "-h") == 0, 0)) {
        displayHelp();
        return 0;
    }
    if (__builtin_expect(strcmp(arg1, "--version") == 0 || strcmp(arg1, "-v") == 0, 0)) {
        displayVersion();
        return 0;
    }
    if (__builtin_expect(buildFileflag != 0, 0)) exit_program(-1);
    if (__builtin_expect(runTarget(execs, arg1) != 0, 0)) return 1;
    return 0;
}
