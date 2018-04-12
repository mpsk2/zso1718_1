//
// Created by micha on 12.04.18.
//

#include <stdio.h>
#include <stdarg.h>
#include "../include/debug.h"


int debug(const char *__restrict __format, ...) {
#if DEBUG
    int r;
    va_list argptr;
    va_start(argptr, __format);
    r = vfprintf(stderr, __format, argptr);
    va_end(argptr);
    return r;
#else
    return 0;
#endif
}