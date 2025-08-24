/*
 * Copyright (c) 2025 Joseph Hinkle
 * See LICENSE file for full MIT license details.
 */

#ifndef CTRYCATCH_H
#define CTRYCATCH_H

// Custom error type. You can define this to use a custom error type.
#if !defined(CTRYCATCH_CUSTOM_ERROR_TYPE)
#define CTRYCATCH_CUSTOM_ERROR_TYPE void*
#endif

// Custom error handling when CTRYCATCH is disabled
#include <stdint.h>
#include <sys/cdefs.h>
#if defined(DISABLE_CTRYCATCH)
// Define this yourself, either in this header or before including this header
// #define throw_error(error)
// #try_catch(expression, success, failure)
#endif

// Let's you define your own "bang" operator (should crash / abort)
#if !defined(CTRYCATCH_BANG_OPERATOR)
void abort(void);
#define CTRYCATCH_BANG_OPERATOR abort()
#endif

// Stub for void type (which cannot be size 0 in C, so we need to define it as a tiny struct)
struct __void_result_value {
    __unused int8_t _value;
};

// Type definitions
#if defined(DISABLE_CTRYCATCH)
#define result(TYPE) __typeof__( __builtin_choose_expr(__builtin_types_compatible_p(__typeof__(TYPE), void), new_result_void, *( (TYPE*)0 )) )
#define result_ptr(TYPE) TYPE*
#define result_const_ptr(TYPE) const TYPE*
#define result_always_error struct __void_result_value __attribute__((noreturn))
#else
#define result(TYPE) result_##TYPE
#define result_ptr(TYPE) result_##TYPE##_ptr
#define result_const_ptr(TYPE) result_##TYPE##_const_ptr
__attribute__((warn_unused_result)) typedef struct {
    __unused int8_t _value;
    CTRYCATCH_CUSTOM_ERROR_TYPE _error;
} result_always_error;
#endif

// Always error type helpers (this is for when there is a result type which is always an error, as in there is never a value for it, just an error)
#if defined(DISABLE_CTRYCATCH)
#define always_error_to_result(error) error
#else
#define always_error_to_result(expression) ({ \
    result_always_error alwaysError = expression; \
    __result_context._error = alwaysError._error; \
    __result_context; \
})
#endif

// Type declarations
#if defined(DISABLE_CTRYCATCH)
#define declare_result(TYPE)
#define declare_result_ptr(TYPE)
#define declare_result_const_ptr(TYPE)
#else
#define declare_result(TYPE) __attribute__((warn_unused_result)) typedef struct result(TYPE) { \
    TYPE _value; \
    void* _error; \
} result(TYPE)
#define declare_result_ptr(TYPE) __attribute__((warn_unused_result)) typedef struct result_ptr(TYPE) { \
    TYPE* _value; \
    void* _error; \
} result_ptr(TYPE)
#define declare_result_const_ptr(TYPE) __attribute__((warn_unused_result)) typedef struct result_const_ptr(TYPE) { \
    const TYPE* _value; \
    void* _error; \
} result_const_ptr(TYPE)
#endif

// Create a result value
#if defined(DISABLE_CTRYCATCH)
#define new_result(value) value
#else
#define new_result(value) (__typeof__(__result_context)){ ._value = value, ._error = NULL }
#endif

// Create a void result value
#if defined(DISABLE_CTRYCATCH)
#define new_result_void (struct __void_result_value){._value = 0}
#else
#define new_result_void (result(void)){ ._value = { ._value = 0 }, ._error = NULL }
#endif

// Result context
#if defined(DISABLE_CTRYCATCH)
#define result_context(TYPE)
#define result_context_ptr(TYPE)
#define result_context_const_ptr(TYPE)
#define result_context_always_error
#else
#define result_context(TYPE) result(TYPE) __result_context = {0}
#define result_context_ptr(TYPE) result_ptr(TYPE) __result_context = {0}
#define result_context_const_ptr(TYPE) result_const_ptr(TYPE) __result_context = {0}
#define result_context_always_error result_always_error __result_context = {0}
#endif

// Try
#if defined(DISABLE_CTRYCATCH)
#define try(expression) expression
#else
#define try(expression) ({ \
    __typeof__(expression) __result = expression; \
    if (__result._error) { \
        __result_context._error = __result._error; \
        return __result_context; \
    } \
    __result._value; \
})
#endif

// Try!
#if defined(DISABLE_CTRYCATCH)
#define try_bang(expression) expression
#else
#define try_bang(expression) ({ \
    __typeof__(expression) __result = expression; \
    if (__result._error) { \
        CTRYCATCH_BANG_OPERATOR; \
    } \
    __result._value; \
})
#endif

// Conversion
#if defined(DISABLE_CTRYCATCH)
#define convert_error(error) error
#define convert_always_error(error) error
#else
#define convert_error(error) ({ \
    __typeof__(error) __error = error; \
    __result_context._error = __error; \
    __result_context; \
})
#define convert_always_error(expression) ({ \
    result_always_error alwaysError = expression; \
    __result_context._error = alwaysError._error; \
    __result_context; \
})
#endif

// Throw
#if defined(DISABLE_CTRYCATCH)
#define throw_always_error(error) throw_error(error)
#else
#define throw_error(error) ({ \
    return convert_error(error); \
})
#define throw_always_error(expression) ({ \
    return convert_always_error(expression); \
})
#endif

// Try catch
#if !defined(DISABLE_CTRYCATCH)
#define try_catch(expression, success, failure) ({ \
    __typeof__(expression) __result = expression; \
    if (__result._error) { \
        __attribute__((unused)) CTRYCATCH_CUSTOM_ERROR_TYPE error = __result._error; \
        failure; \
    } else { \
        __attribute__((unused)) __typeof__(__result._value) value = __result._value; \
        success; \
    } \
})
#endif

// Default declarations
declare_result(int); declare_result_ptr(int); declare_result_const_ptr(int);
declare_result(double); declare_result_ptr(double); declare_result_const_ptr(double);
declare_result(char); declare_result_ptr(char); declare_result_const_ptr(char);
declare_result(short); declare_result_ptr(short); declare_result_const_ptr(short);
declare_result(long); declare_result_ptr(long); declare_result_const_ptr(long);
declare_result_ptr(void);

// Declaration for void type
#if !defined(DISABLE_CTRYCATCH)
__attribute__((warn_unused_result)) typedef struct result(void) { \
    __unused struct __void_result_value _value; \
    void* _error; \
} result(void);
#endif

#endif // CTRYCATCH_H
