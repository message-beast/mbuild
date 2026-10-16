#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "config.h"
#include "basic.h"
#include <errno.h>
#include "utils.h"

DECLARE_GIVE_STR

static inline __attribute__((always_inline, hot, aligned(64))) mexec* parseContent(char* content, size_t len) {
    mexec* newExec = malloc(sizeof(mexec));
    if (__builtin_expect(newExec == NULL, 0)) return NULL;
    char** commands = malloc(sizeof(char*) * __INITIAL_ARGS_CAPACITY);
    if (__builtin_expect(commands == NULL, 0)) goto fail;
    newExec->argsCapacity = __INITIAL_ARGS_CAPACITY;
    newExec->argsNumber = 0;
    newExec->commands = commands;
    int found = 0;
    int lastIndex = 0;
    if (__builtin_expect(newExec == NULL, 0)) return NULL;
    for (register int i = 0; i < len; ++i) {
        if (__builtin_expect(content[i] == ' ', 0)) {
            char* newArg = giveString(content, lastIndex, i);
            if (__builtin_expect(newArg == NULL, 0)) exit_program(-1);
            newExec->commands[found++] = newArg;
            lastIndex = i + 1;
        }
    }
    newExec->next = NULL;
    return newExec;
    fail:
        free(newExec);
        return NULL;

}



__attribute__((hot)) mexec** parseFile() {
    int fd = open(__DEFAULT_BUILD_FILE, O_RDONLY);
    if (__builtin_expect(fd == -1, 0)) {
        if (errno == EACCES) {
            fprintf(stderr, "permission denied!\n");
        } else {
            perror("error happend!\n");
        }
        exit_program(-1);
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on build file!\n");
        exit_program(-1);
    }
    char* data = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("mmap failed on build file!\n");
        exit_program(-1);
    }
    char* content = NULL;
    int cmdlastIndex = 0;
    int nameLastIndex = 0;
    char* name = NULL;
    mexec* mexecss = (malloc(sizeof(mexec)));
    mexec** mexecs = &(mexecss);
    if (__builtin_expect(mexecs == NULL, 0)) goto fail;
    enum {FIND_SECTION, FIND_END} state = FIND_SECTION;
    int found = 0;
    for (register int i = 0; i < st.st_size; ++i) {
        switch(state) {
            case FIND_SECTION:
                if (__builtin_expect(data[i] == ':', 0)) {
                    cmdlastIndex = i + 2;
                    state = FIND_END;
                    char* tmpName = giveString(data, nameLastIndex, i);
                    if (__builtin_expect(name == NULL, 0)) goto fail;
                    name = tmpName;
                };
                break;
            case FIND_END:
                if (__builtin_expect(data[i] == '\n', 0)) {
                    char* command = giveString(data, cmdlastIndex, i);
                    if (__builtin_expect(command == NULL, 0)) goto crash;
                    mexec* newExec = parseContent(command, i - cmdlastIndex);
                    if (__builtin_expect(newExec == NULL, 0)) {
                        free(command);
                        goto crash;
                    };
                    newExec->target = name;
                    mexec* before = (*mexecs);
                    (*mexecs) = newExec;
                    newExec->next = before;
                    free(command);
                    state = FIND_SECTION;
                    nameLastIndex = i + 1;
                }
                break;
        }
    }
    return mexecs;
    fail:
        munmap(data, st.st_size);
        close(fd);
        exit_program(-1);
    crash:
        free(name);
        munmap(data, st.st_size);
        close(fd);
        exit_program(-1);
}
