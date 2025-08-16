/*
 * Copyright (c) 2025 Joseph Hinkle
 * See LICENSE file for full MIT license details.
 */

#ifndef CTRYCATCHSETJMP_H
#define CTRYCATCHSETJMP_H

#include <setjmp.h>

extern _Thread_local jmp_buf error_jmp_buf;
extern _Thread_local void *error_value;

static inline __attribute__((noreturn)) void throw_error(void *error) {
    error_value = error;
    longjmp(error_jmp_buf, 1);
}

#define try_catch(expression, success, failure) ({ \
    if (setjmp(error_jmp_buf) == 0) { \
        __attribute__((unused)) __typeof__(expression) value = expression; \
        success; \
    } else { \
        __attribute__((unused)) void *error = error_value; \
        failure; \
    } \
})

#endif // CTRYCATCHSETJMP_H