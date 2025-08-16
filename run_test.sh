#!/bin/bash

# Ensure script exits on any error
set -e

# Compiler settings
CC=gcc
CFLAGS="-Wall -Wextra -std=c11"

# Create build directory if it doesn't exist
mkdir -p build

echo "Running test program with DISABLE_CTRYCATCH=1..."
$CC $CFLAGS -DDISABLE_CTRYCATCH=1 test.c -o build/test_program_disabled
./build/test_program_disabled

echo "Running test program with DISABLE_CTRYCATCH=0..."
$CC $CFLAGS test.c -o build/test_program_enabled
./build/test_program_enabled