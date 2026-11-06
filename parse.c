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




__attribute__((section(".data"))) groupexec* groupexecs = NULL;




#define resize_commands \
if (__builtin_expect(found + 1 >= newExec->argsCapacity, 0)) {\
    int newSize = newExec->argsCapacity + (__INITIAL_ARGS_CAPACITY * sizeof(char*));\
    char** tmp = realloc(newExec->commands, newSize);\
    if (__builtin_expect(tmp == NULL, 0)) {\
        perror("reallocation of commands failed! (mbuild internal error)\n");\
        exit_program(1);\
    }\
    newExec->commands = tmp;\
}\


DECLARE_GIVE_STR

static inline __attribute__((always_inline, hot, aligned(64), artificial)) mexec* parseContent(char* content, size_t len) {
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
            resize_commands
            char* newArg = giveString(content, lastIndex, i);
            if (__builtin_expect(newArg == NULL, 0)) exit_program(-1);
            newExec->commands[found++] = newArg;
            lastIndex = i + 1;
        } else if (__builtin_expect(len == i + 1, 0)) {
            resize_commands
            char* newArg = giveString(content, lastIndex, i + 1);
            if (__builtin_expect(newArg == NULL, 0)) exit_program(-1);
            newExec->commands[found++] = newArg;
            lastIndex = i + 1;
        }
    }
    newExec->commands[found++] = NULL;
    newExec->argsNumber = found;
    return newExec;
    fail:
        free(newExec);
        return NULL;

}


static inline __attribute__((always_inline, artificial, hot, aligned(64), nonnull(1, 2))) struct groupexec* parseNewGroupExec(char* __restrict__ name, char* groupexecBuff, int len) {
    register int lastIndex = 0;
    groupexec* newGroupexec = malloc(sizeof(groupexec));
    if (__builtin_expect(newGroupexec == NULL, 0)) {
        perror("memory allocation failed for new group exec!\n");
        exit_program(-1);
    }
    char** targets = malloc(sizeof(char*) * __INITIAL_GROUP_EXEC_TARGETS_NUMBER__);
    if (__builtin_expect(targets == NULL, 0)) {
        perror("failed to allocate memory for the new targets of group exec!\n");
        free(newGroupexec);
        exit_program(-1);
    }
    newGroupexec->targets = targets;
    newGroupexec->capacity = __INITIAL_GROUP_EXEC_TARGETS_NUMBER__;
    newGroupexec->name = name;
    register int found = 0;
    for (register int i = 0; i < len; ++i) {
        if (__builtin_expect(groupexecBuff[i] == ',', 0)) {
            char* newTarget = giveString(groupexecBuff, lastIndex, i);
            if (__builtin_expect(newTarget == NULL, 0)) {
                fprintf(stderr, "error while parsing the targets of group exec!\n");
                exit_program(-1);
            }
            if (__builtin_expect(found >= newGroupexec->capacity, 0)) {
                int newCapacity= newGroupexec->capacity + __INITIAL_GROUP_EXEC_TARGETS_NUMBER__;
                char** tmpTargets = realloc(newGroupexec->targets, (newCapacity * sizeof(char*)));
                if (__builtin_expect(tmpTargets == NULL, 0)) {
                    fprintf(stderr, "failed to re-allocate group exec targets!\n");
                    exit_program(-1);
                }
                newGroupexec->targets = tmpTargets;
                newGroupexec->capacity = newCapacity;
            }
            newGroupexec->targets[found++] = newTarget;
            lastIndex = i + 1;
        }
    }
    newGroupexec->numbers = found;
    return newGroupexec;
}



__attribute__((hot)) mexec* parseFile() {
    int fd = open(__DEFAULT_BUILD_FILE, O_RDONLY);
    if (__builtin_expect(fd == -1, 0)) {
        if (errno == EACCES) {
            fprintf(stderr, "permission denied!\n");
            buildFileflag = 1;
        } else {
            perror("can't open build file\n");
            buildFileflag = 1;
        }
        return NULL;
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
    int cmdlastIndex = 0;
    int nameLastIndex = 0;
    char* name = NULL;
    mexec* mexecs = NULL;
    enum {FIND_SECTION, FIND_END} state = FIND_SECTION;
    enum {IS_GROUP, IS_COMMAND} execType = IS_COMMAND;
    int groupFirstIndex = 0;
    for (register int i = 0; i < st.st_size; ++i) {
        switch(state) {
            case FIND_SECTION:
                if (__builtin_expect(data[i] == ':', 0)) {
                    i++;
                    cmdlastIndex = i + 2;
                    state = FIND_END;
                    char* tmpName = giveString(data, nameLastIndex, i - 1);
                    if (__builtin_expect(tmpName == NULL, 0)) goto fail;
                    name = tmpName;
                };
                break;
            case FIND_END:
                if (__builtin_expect(data[i] == '[', 0)) {
                    execType = IS_GROUP;
                    groupFirstIndex = i + 1;
                }
                switch(execType) {
                    case IS_GROUP:
                        if (__builtin_expect(data[i] == ']', 0)) {
                            char* groupexecBuff = giveString(data, groupFirstIndex, i);
                            if (__builtin_expect(groupexecBuff == NULL, 0)) {
                                fprintf(stderr, "error while parsing group exec!\n");
                                goto crash;
                            }
                            groupexec* newGroupExec = parseNewGroupExec(name, groupexecBuff, i - groupFirstIndex);
                            if (__builtin_expect(newGroupExec == NULL, 0)) {
                                fprintf(stderr, "error while parsing group exec!\n");
                                goto crash;
                            }
                            groupexec** groupexecss = &groupexecs;
                            newGroupExec->next = groupexecs;
                            *groupexecss = newGroupExec;
                            nameLastIndex = i + 2;
                            name = NULL;
                            free(groupexecBuff);
                            state = FIND_SECTION;
                        }
                        break;
                    case IS_COMMAND:
                        if (__builtin_expect(data[i] == '\n', 0)) {
                            char* command = giveString(data, cmdlastIndex, i);
                            if (__builtin_expect(command == NULL, 0)) goto crash;
                            mexec* newExec = parseContent(command, i - cmdlastIndex);
                            if (__builtin_expect(newExec == NULL, 0)) {
                                free(command);
                                goto crash;
                            };
                            newExec->target = name;
                            mexec* before = mexecs;
                            mexecs = newExec;
                            newExec->next = before;
                            free(command);
                            state = FIND_SECTION;
                            nameLastIndex = i + 1;
                        }
                        break;
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
        printf("crashed!\n");
        exit_program(-1);
}
