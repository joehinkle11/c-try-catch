# C Try Catch

A lightweight, header-only try/catch implementation for C that provides a type-safe error handling mechanism using macros. This library implements a Result type pattern similar to Rust, allowing for explicit error handling.

Disclaimer, AI was used to generate this README, but the actual macros were hand crafted.

## Features

- Header-only library - just include `ctrycatch.h`
- Zero dependencies
- Type-safe error handling through Result types
- No dynamic memory allocation
- No use of setjmp/longjmp
- Minimal runtime overhead (just result value checks)
- Can be disabled via `DISABLE_CTRYCATCH` to allow fallback to other exception techniques
- Supports primitive types and pointer types
- Support for void results
- Chainable error propagation with the `try` macro
- Pre-declared result types for common primitives (int, double, char, short, long)
- Support for pointer result types
- Easily declare new result types for your own structs

## Usage

1. Copy `ctrycatch.h` into your project (and optionally `ctrycatchsetjmp.h` if you want the setjmp fallback)
2. Include the header: `#include "ctrycatch.h"`
3. Use the provided macros for error handling

### Basic Example

Say you want to implement a `divide` function which throws an error when dividing by 0:

```c
int divide(int a, int b) {
    if (b == 0) {
        abort(); // Throw error here!
    }
    return a / b;
}
```

You can implement this instead by wrapping the `int` result around the `result` macro.

```c
result(int) divide(int a, int b) {
    result_context(int);
    if (b == 0) {
        throw_error("Division by zero");
    }
    return new_result(a / b);
}
```

Note that you also need to set the result context in the function body (this context is used by the `throw_error_` and `new_result` macros).

Now you can call divide and check if the result has an error or a value.

```c
// Using try_catch to handle errors
void example() {
    result(int) divideResult = divide(10, 0);
    if (divideResult.error) {
        printf("Error: %s\n", (const char*)divideResult.error);
    } else {
        printf("Result: %d\n", divideResult.value);
    }
}
```

### Using try catch

```c
// Using try_catch to handle errors
void example() {
    try_catch(divide(10, 0), {
        printf("Result: %d\n", value);
    }, {
        printf("Error: %s\n", (const char*)error);
    });
}
```

### Using `try(...)` macro / Chaining Results

```c
result(int) add_five(int value, int should_fail) {
    result_context(int);
    
    // The try macro automatically propagates errors
    int base = try(divide(value, should_fail ? 0 : 1));
    return new_result(base + 5);
}
```

### Using "try bang" or try macro without being in a result context

```c
int add_five(int value, int should_fail) {
    // The try bang macro will crash if there is an error. Useful in cases where you know there won't be an error.
    int base = try_bang(divide(value, should_fail ? 0 : 1));
    return base + 5;
}
```

### Working with Void Results

As you cannot directly construct a `void` value in C, you need to use `new_result_void` to create an instance of the void result type.

```c
result(void) perform_action(int should_fail) {
    result_context(void);
    if (should_fail) {
        throw_error("Action failed");
    }
    return new_result_void;
}
```

### Working with non-returning functions

Some functions always throw. In these situations, you can use the `result_context_always_error` macro to indicate that the function will never return a successful result:

```c
result_always_error always_fails(const char* message) {
    result_context_always_error;
    throw_error((void*)message);
}

void example() {
    try_catch(always_fails("This will always fail"), {
        // This block will never be executed
        printf("This is unreachable\n");
    }, {
        printf("Error message: %s\n", (const char*)error);
    });
}
```

### Using pointer value types

The library supports returning pointer values that might fail using `result_ptr(TYPE)`. Here's an example:

```c
result_ptr(int) create_dynamic_int(int initial_value) {
    result_context_ptr(int);
    
    int* ptr = malloc(sizeof(int));
    if (!ptr) {
        throw_error("Memory allocation failed");
    }
    
    *ptr = initial_value;
    return new_result_ptr(ptr);
}

void example() {
    try_catch(create_dynamic_int(42), {
        printf("Created integer with value: %d\n", *value);
        // Don't forget to free the allocated memory
        free(value);
    }, {
        printf("Failed to create integer: %s\n", (const char*)error);
    });
}
```

The `result_ptr(TYPE)` is particularly useful when working with dynamically allocated memory. In this example, we're allocating an integer on the heap and handling potential allocation failures gracefully. The macro ensures type safety while working with pointers.

### Declaring your own types to be used for the `result` / `result_ptr` macros

To use your own custom types with the `result` and `result_ptr` macros, you need to declare them using the `declare_result` and `declare_result_ptr` macros. Here's how:

```c
// For a struct type
struct Point {
    int x;
    int y;
};
typedef struct Point Point;

// Declare result type for Point
declare_result(Point);

// If you also want to work with Point pointers
declare_result_ptr(Point);

// Now you can use these types in functions
result(Point) create_point(int x, int y) {
    result_context(Point);
    Point p = {.x = x, .y = y};
    return new_result(p);
}

result_ptr(Point) create_point_on_heap(int x, int y) {
    result_context_ptr(Point);
    Point* p = malloc(sizeof(Point));
    if (!p) {
        throw_error("Memory allocation failed");
    }
    p->x = x;
    p->y = y;
    return new_result_ptr(p);
}
```

The declarations will create type-safe result structs that include both the value and any potential error. The library already includes pre-declared result types for common primitives (int, double, char, short, long), but you can declare results for any custom type you need.

Note that:
1. You must declare the result type before using it in any function
2. For pointer results, use `result_ptr(TYPE)` and `result_context_ptr(TYPE)`
3. The error type is always `void*` by default (can be customized via `CTRYCATCH_CUSTOM_ERROR_TYPE`)
4. Result types are marked with `__attribute__((warn_unused_result))` to help catch unhandled errors


### Disabling Try-Catch

The library can be disabled by defining `DISABLE_CTRYCATCH`, which allows you to provide alternative implementations:

```c
#define DISABLE_CTRYCATCH
#include "ctrycatch.h"

// Define your own error handling
void throw_error(void* error) {
    // Your implementation
}

#define try_catch(expression, success, failure) {
    // Your implementation
}
```

There's an implementation which uses setjmp in `ctrycatchsetjmp.h`. This solution requires call stack unwinding to work (most environments support this).

## Type System

The library provides two main types of results:

1. Value results: `result(TYPE)` - For returning values that might fail
2. Pointer results: `result_ptr(TYPE)` - For returning pointers that might fail

Pre-declared types include:
- `result(int)`
- `result(double)`
- `result(char)`
- `result(short)`
- `result(long)`
- `result(void)` - Special case for functions that only need to indicate success/failure

## Error Handling Macros

- `result_context(TYPE)` - Establishes error handling context for a function
- `result_context_always_error` - Establishes context for functions that always error
- `try(expression)` - Executes an expression and propagates any errors
- `throw_error(error)` - Signals an error condition
- `try_catch(expression, success, failure)` - Executes code with error handling
- `new_result(value)` - Creates a new result value
- `new_result_void` - Creates a new void result

## License

MIT License - See LICENSE file for full details