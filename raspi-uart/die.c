#include "die.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void
die(const char *fmt, ...)
{
    char buf[1024];
    va_list va;

    va_start(va, fmt);
    vsnprintf(buf, sizeof(buf), fmt, va);
    va_end(va);

    fprintf(stderr, "FATAL: %s\n", buf);
    abort();
}
