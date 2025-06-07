#include <stdio.h>
#include "cmsis_compiler.h"
#include "at_assert.h"

__NO_RETURN
void __assert_func(const char* fname, int line, const char* func, const char* expr)
{

    printf("[%s:%d][%s] assert @ %s\r\n", fname, line, func, expr);

    while (1);
}
