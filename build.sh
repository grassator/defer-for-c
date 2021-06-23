#!/bin/bash
set -e

# You can explicitly set the C compiler by providing a CC environment variable.
# This also works inline: CC=clang ./build.sh
CC="${CC:-cc}"

FLAGS="-std=c89 -g -O0 -Wall"

if [[ $($CC -v 2>&1) == *"clang version"* ]]
then
  FLAGS="$FLAGS -Wno-tautological-constant-out-of-range-compare -Wno-initializer-overrides"
fi

rm -rf build
mkdir -p build

cp demo.c build\

$CC $FLAGS demo.c -o build/demo -lm