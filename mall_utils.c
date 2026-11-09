#include "basic.h"
#include "parsemall.h"
#include "utils.h"
#include <stdlib.h>
#include "config.h"
#include "run_mall.h"
#include <stdio.h>

static inline __attribute__((always_inline, hot)) int setupDmall() {
    mall** tmp = malloc(sizeof(mall*) * __INITIAL_BUILD_D_CAPACITY);
    if (__builtin_expect(tmp == NULL, 0)) return -1;
    _dmall = tmp;
    tmp = NULL;
    return 0;
}



int do_everything_Dmall() {
    if (__builtin_expect(setupDmall() != 0, 0)) exit_program(-1);
    if (__builtin_expect(parseMall() != 0, 0)) exit_program(-1);
    int status = runDmall();
    freeDmall();
    return status;
}
