#!/bin/bash

# Ensure script exits on any error
set -e

# Compiler settings
gcc -E test.c -o test_enabled_expanded_macros.c
gcc -E -DDISABLE_CTRYCATCH=1 test.c -o test_disabled_expanded_macros.c


