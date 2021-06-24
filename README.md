# defer-for-c

This is a small single-header implementation of Go-style `defer`
statements allowing to schedule a function call to be run before
returning from the functions.

## Quick Start

```c
#include <stdio.h>

/* Include the library header */
#include "defer.h"

int main(void) {
  USE_DEFER();

  FILE *f = fopen(__FILE__, "rb");

  /* You can use DEFER on any function that accepts a pointer
   * `fclose` is a common call that can benefit from DEFER */
  if (f) DEFER(fclose, f);

  return;
}
```

## Supported Platforms

The code is known to work with MSVC on Windows and GCC and Clang on Linux.
Supports C89/C99/C11/C17 versions of the C language.

For default implementation `alloca` support is required.
You can switch to the manual stack reservation mode that does
not required `alloca`(see below).

## Public API

`USE_DEFER();` statement-like macro needs to be included at the top
of any procedure using `DEFER()` macros.

`DEFER` function-like macro that can be viewed as
a function with the following signature:

```c
typedef void(*defer_callback_t)(void *payload);
void DEFER(defer_callback_t callback, void *payload);
```

The library also redefines `return` as a macro but the definition
does not have any observable effects on procedures without `USE_DEFER()`.

## Common Issues

### Explicit `return` in `void` Procedures

If you try to compile a procedure like this:

```c
void foo(void) {
  USE_DEFER();
}
```

> Static error detection only works on MSVC and Clang. If anyone knows how to force GCC
to error out on a *particular* unused variable help is very much appreciated.

You will get an error that looks similar to this (depending on the compiler):

```
'ERROR_void_functions_must_use_an_explicit_return_at_the_end': unreferenced local variable
```

To fix it include a `return;` statement at the end of the procedure:

```c
void foo(void) {
  USE_DEFER();
  return;  /* <-- fix */
}
```

### Missing `USE_DEFER` at the Start of the Procedure

If you try to compile a file like this:
```c
void dummy(void) {}

int main() {
  DEFER(dummy);
  return 0;
}
```

You will get an error that looks similar to this (depending on the compiler):

```
label 'USE_DEFER_must_appear_at_the_start_of_the_functions_using_DEFER' was undefined
```

To fix it include a `USE_DEFER();` statement at the start of the procedure:

```c
void dummy(void) {}

int main() {
  USE_DEFER(); /* <-- fix */
  DEFER(dummy);
  return 0;
}
```

### No `alloca` Mode

You can switch to the manual stack reservation mode by adding
`#define DEFER_NO_ALLOCA` directive before including the header.

In this mode you need to provide the number of `DEFER()` slots to reserve
when calling `USE_DEFER(4)`. In this case we are reserving `4` slots.
There is a runtime `assert()` for overflows if you have reserved
less slots than you have `DEFER()` calls.

## How Does It Work?

`USE_DEFER` sets up a list of entries that include the pointer to a
user-provided callbacks and payload for each of the `DEFER()` calls.
Depending on whether `DEFER_NO_ALLOCA` is defined either a linked list
or a fixed-size array is used on the stack.

The main trick is the redefinition of `return` keyword as macro such that it
does nothing in regular procedures. For procecures that `USE_DEFER()`
it goes through the list of pushed callbacks in the reverse order and call.

## Performance

Even though `return` keyword is redefined for all procedures it is optimized
out even with `-O1` level.

For procedures that `USE_DEFER` you will have extra stack space allocated
and there is a loop for each `return` statement that might or might not be
coalesced into a single one by the compiler since they are all identical.

Usual overhead of calling-through-a-pointer applies as well.

All in all it should perfectly reasonable to use this for any procedure
that is not called inside a performance-sensitive tight loop.

## Development

### Windows

To build the demo open up MSVC Command Prompt and run `build.bat` in the repository folder.

### Mac / Linux

You can build the demo by running `./build.sh`.
Both gcc and clang compilers are supported and you can
set which one to use by providing a `CC` environment variable:

```
CC=clang ./build.sh
```

## License

Copyright (c) 2021 Dmitriy Kubyshkin

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

