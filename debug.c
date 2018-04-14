//
// Michał Piotr Stankiewicz
// ms335789
// Zadanie zaliczeniowe ZSO 2017 / 2018
// Zadanie numer 1
//

#include <stdio.h>
#include <stdarg.h>
#include "debug.h"


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