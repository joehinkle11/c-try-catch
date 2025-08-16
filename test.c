/*
 * Copyright (c) 2025 Joseph Hinkle
 * See LICENSE file for full MIT license details.
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "ctrycatch.h"

// Custom error handling when CTRYCATCH is disabled
#if defined(DISABLE_CTRYCATCH)
#include "ctrycatchsetjmp.h"
_Thread_local jmp_buf error_jmp_buf;
_Thread_local void *error_value;
#endif

result(int) return_int_result(int value, int should_fail) {
    result_context(int);
    if (should_fail) {
        throw_error("Error");
    }
    return new_result(value);
}

result(int) return_int_plus_5_result(int value, int should_fail) {
    result_context(int);

    int result = try(return_int_result(value, should_fail));
    return new_result(result + 5);
}

result(double) return_double_plus_int_plus_5_result(double d, int i, int should_fail) {
    result_context(double);
    int int_result = try(return_int_plus_5_result(i, should_fail));
    return new_result(d + int_result);
}

int return_int(int i, int should_fail) {
    try_catch(return_int_result(i, should_fail), {
        return value;
    }, {
        assert(strcmp(error, "Error") == 0);
        return -1;
    });
}
int return_int_catching_adding_7(int i, int should_fail) {
    try_catch(return_int_result(i, should_fail), {
        return value + 7;
    }, {
        assert(strcmp(error, "Error") == 0);
        return -1;
    });
}

result(void) return_void_result(int should_fail) {
    result_context(void);
    if (should_fail) {
        throw_error("Error");
    }
    return new_result_void;
}

result_always_error always_throw_error() {
    result_context_always_error;
    throw_error("Error");
}

void catch_always_throw_error() {
    try_catch(always_throw_error(), {
        assert(0);
    }, {
        assert(strcmp(error, "Error") == 0);
    });
}

struct Point {
    int x;
    int y;
};
typedef struct Point Point;

declare_result(Point);
declare_result_ptr(Point);

result(Point) create_point(int x, int y, int should_fail) {
    result_context(Point);
    if (should_fail) {
        throw_error("Error");
    }
    Point p = {.x = x, .y = y};
    return new_result(p);
}

void *malloc(size_t size);
void free(void *_Nullable ptr);

result_ptr(Point) create_point_on_heap(int x, int y, int should_fail) {
    result_context_ptr(Point);
    if (should_fail) {
        throw_error("Error");
    }
    Point* p = malloc(sizeof(Point));
    p->x = x;
    p->y = y;
    return new_result(p);
}

#if defined(DISABLE_CTRYCATCH)
int main(void) {
    printf("Running tests for c try catch (CTRYCATCH DISABLED)\n");
    
    result(int) resultInt = return_int_result(10, 0);
    assert(resultInt == 10);

    if (setjmp(error_jmp_buf) == 0) {
        resultInt = return_int_result(10, 1);
    } else {
        assert(strcmp(error_value, "Error") == 0);
    }

    result(double) resultDouble = return_double_plus_int_plus_5_result(10.0, 10, 0);
    assert(resultDouble == 25.0);

    if (setjmp(error_jmp_buf) == 0) {
        resultDouble = return_double_plus_int_plus_5_result(10.0, 10, 1);
    } else {
        assert(strcmp(error_value, "Error") == 0);
    }
    
    int myInt = return_int(10, 0);
    assert(myInt == 10);

    myInt = return_int(10, 1);
    assert(myInt == -1);

    __unused result(void) resultVoid = return_void_result(0);

    if (setjmp(error_jmp_buf) == 0) {
        resultVoid = return_void_result(1);
    } else {
        assert(strcmp(error_value, "Error") == 0);
    }

    myInt = return_int_catching_adding_7(10, 0);
    assert(myInt == 17);

    myInt = return_int_catching_adding_7(10, 1);
    assert(myInt == -1);
    

    int myVal = 0;
    try_catch(return_int_result(5, 1), {
        myVal = value * 2;
    }, {
        assert(strcmp(error, "Error") == 0);
        myVal = -1;
    });    
    assert(myVal == -1);
    
    myVal = 0;
    try_catch(return_int_result(5, 0), {
        myVal = value * 2;
    }, {
        assert(strcmp(error, "Error") == 0);
        myVal = -1;
    });
    assert(myVal == 10);

    if (setjmp(error_jmp_buf) == 0) {
        always_throw_error();
    } else {
        assert(strcmp(error_value, "Error") == 0);
    }
    
    catch_always_throw_error();

    result(Point) point = create_point(10, 10, 0);
    assert(point.x == 10);
    assert(point.y == 10);

    if (setjmp(error_jmp_buf) == 0) {
        point = create_point(10, 10, 1);
        assert(0);
    } else {
        assert(strcmp(error_value, "Error") == 0);
    }

    result_ptr(Point) point_ptr = create_point_on_heap(10, 10, 0);
    assert(point_ptr->x == 10);
    assert(point_ptr->y == 10);

    if (setjmp(error_jmp_buf) == 0) {
        point_ptr = create_point_on_heap(10, 10, 1);
        assert(0);
    } else {
        assert(strcmp(error_value, "Error") == 0);
    }

    printf("All tests passed ✅ (CTRYCATCH DISABLED )\n");
    printf("\n");
    return 0;
}
#else
int main(void) {
    printf("Running tests for c try catch\n");
    
    result(int) resultInt = return_int_result(10, 0);
    assert(resultInt._value == 10);
    assert(resultInt._error == NULL);

    resultInt = return_int_result(10, 1);
    assert(resultInt._value == 0);
    assert(resultInt._error != NULL);
    assert(strcmp(resultInt._error, "Error") == 0);

    result(double) resultDouble = return_double_plus_int_plus_5_result(10.0, 10, 0);
    assert(resultDouble._value == 25.0);
    assert(resultDouble._error == NULL);

    resultDouble = return_double_plus_int_plus_5_result(10.0, 10, 1);
    assert(resultDouble._value == 0);
    assert(resultDouble._error != NULL);
    assert(strcmp(resultDouble._error, "Error") == 0);
    
    int myInt = return_int(10, 0);
    assert(myInt == 10);

    myInt = return_int(10, 1);
    assert(myInt == -1);

    result(void) resultVoid = return_void_result(0);
    assert(resultVoid._error == NULL);

    resultVoid = return_void_result(1);
    assert(resultVoid._error != NULL);
    assert(strcmp(resultVoid._error, "Error") == 0);

    myInt = return_int_catching_adding_7(10, 0);
    assert(myInt == 17);

    myInt = return_int_catching_adding_7(10, 1);
    assert(myInt == -1);

    int myVal = 0;
    try_catch(return_int_result(5, 1), {
        myVal = value * 2;
    }, {
        assert(strcmp(error, "Error") == 0);
        myVal = -1;
    });    
    assert(myVal == -1);
    
    myVal = 0;
    try_catch(return_int_result(5, 0), {
        myVal = value * 2;
    }, {
        assert(strcmp(error, "Error") == 0);
        myVal = -1;
    });
    assert(myVal == 10);

    result_always_error always_throw_error_result = always_throw_error();
    assert(always_throw_error_result._error != NULL);
    assert(strcmp(always_throw_error_result._error, "Error") == 0);

    catch_always_throw_error();

    result(Point) point = create_point(10, 10, 0);
    assert(point._value.x == 10);
    assert(point._value.y == 10);
    assert(point._error == NULL);

    point = create_point(10, 10, 1);
    assert(point._value.x == 0);
    assert(point._value.y == 0);
    assert(point._error != NULL);
    assert(strcmp(point._error, "Error") == 0);

    result_ptr(Point) point_ptr = create_point_on_heap(10, 10, 0);
    assert(point_ptr._value->x == 10);
    assert(point_ptr._value->y == 10);
    assert(point_ptr._error == NULL);
    free(point_ptr._value);

    point_ptr = create_point_on_heap(10, 10, 1);
    assert(point_ptr._value == NULL);
    assert(point_ptr._error != NULL);
    assert(strcmp(point_ptr._error, "Error") == 0);

    printf("All tests passed ✅\n");
    printf("\n");
    return 0;
}

#endif