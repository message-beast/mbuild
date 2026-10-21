#include <string.h>
#include "basic.h"
void __attribute__((noreturn)) panic(int i);
#define exit_program(i) panic(i)

#define DECLARE_GIVE_STR static inline __attribute__((always_inline, hot, aligned(64), nonnull(1))) char* giveString(char* str, int startIndex, int lastIndex) {\
    int size = lastIndex - startIndex;\
    if (__builtin_expect(size <= 0, 0)) return NULL;\
    char* newString = malloc(size + 1);\
    if (__builtin_expect(newString == NULL, 0)) return NULL;\
    memcpy(newString, str + startIndex, size);\
    newString[size] = '\0';\
    return newString;\
}\


__attribute__((hot, aligned(64))) void freeMExecs(mexec** execs);
