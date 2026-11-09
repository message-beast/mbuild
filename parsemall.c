#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200809L
#endif
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "utils.h"
#include "config.h"
#include <stdio.h>
#include <errno.h>
#include "basic.h"

__attribute__((section(".data"))) mall** _dmall = NULL;


DECLARE_GIVE_STR



#define IS_DIRECTORY 2
#define IS_TARGET 3
#define IS_UNKNOWN 4

static inline __attribute__((always_inline, hot, aligned(64), nonnull(1))) char* parseContent(const char* data, const int len, int* type) {
    int lastIndex = 0;
    enum {FIND_KEY, FIND_VALUE} state = FIND_KEY;
    for (register int i = 0; i < len; ++i) {
        switch(state) {
            case FIND_KEY:
                if (__builtin_expect(data[i] == ':', 0)) {
                    char* key = giveString(data, lastIndex, i);
                    if (__builtin_expect(key == NULL, 0)) return NULL;
                    if (__builtin_expect(strcmp(key, "wd") == 0, 0)) *type = IS_DIRECTORY;
                    else if (__builtin_expect(strcmp(key, "target") == 0, 0)) *type = IS_TARGET;
                    else {
                        printf("the got key: %s\n", key);
                        free(key);
                        fprintf(stderr, "you have got unknown key, the known keys are wd and target!\n");
                        exit_program(-1);
                    }
                    free(key);
                    ++i;
                    lastIndex = i + 1;
                    state = FIND_VALUE;
                }
                break;
            case FIND_VALUE:
                if (__builtin_expect(data[i] == ';', 0)) {
                    char* value = giveString(data, lastIndex, i);
                    if (__builtin_expect(value == NULL, 0)) goto fail;
                    return value;
                }
                break;
        }
    }
    goto crash;
    fail:
        exit_program(-1);
    crash:
        return NULL;
}



__attribute__((hot)) int parseMall() {
    int fd = open(__DEFAULT_BUILD_ALL_FILE, O_RDONLY);
    if (__builtin_expect(fd == -1, 0)) {
        if (__builtin_expect(errno == ENOENT, 1)) {
            buildFileflag = 1;
        } else {
            perror("can't open build file\n");
        }
        return -1;
    }
    struct stat st;
    if (__builtin_expect(fstat(fd, &st) != 0, 0)) {
        perror("fstat failed on the mbuildall file!\n");
        goto fail1;
    }
    if (__builtin_expect(st.st_size == 0, 0)) {
        goto fail1;
    }
    char* data = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (__builtin_expect(data == MAP_FAILED, 0)) {
        perror("mmap failed on mbuildall file!\n");
        goto fail1;
    }
    enum {FIND_SECTION, EXTRACT_KV_PAIRS} state = FIND_SECTION;
    int lastIndex = 0;
    mall* newMall = NULL;
    for (register int i = 0; i < st.st_size; ++i) {
        switch(state) {
            case FIND_SECTION:
                if (__builtin_expect(data[i] == '{', 1)) {
                    state = EXTRACT_KV_PAIRS;
                    ++i;
                    lastIndex = i + 2;
                    newMall = malloc(sizeof(mall));
                    if (__builtin_expect(newMall == NULL, 0)) goto fail2;
                }
                break;
            case EXTRACT_KV_PAIRS:
                if (__builtin_expect(data[i] == ';', 0)) {
                    char* newMallDBuff = giveString(data, lastIndex, i + 1);
                    if (__builtin_expect(newMallDBuff == NULL, 0)) goto fail2;
                    int type;
                    char* value = parseContent(newMallDBuff, i - lastIndex + 1, &type);
                    free(newMallDBuff);
                    if (type == IS_DIRECTORY) newMall->directory = value;
                    else if (type == IS_TARGET) newMall->target = value;
                    else {
                        fprintf(stderr, "you just declared unknwon key!\n");
                        goto fail2;
                        }
                    lastIndex = i + 3;
                } else if (__builtin_expect(data[i] == '}', 0)) {
                    if (__builtin_expect(capacityOfDBuilds <= numberOfDBuilds, 0)) {
                        int newCapacity = capacityOfDBuilds + __INITIAL_BUILD_D_CAPACITY;
                        mall** tmpDmall = realloc(_dmall, newCapacity);
                        if (__builtin_expect(tmpDmall == NULL, 0)) goto fail2;
                        _dmall = tmpDmall;
                        tmpDmall = NULL;
                        capacityOfDBuilds = newCapacity;
                    }
                    _dmall[numberOfDBuilds++] = newMall;
                    state = FIND_SECTION;
                }
        }
    }
    return 0;
    fail1:
        close(fd);
        return -1;
    fail2:
        munmap(data, st.st_size);
        close(fd);
        exit_program(-1);
}
