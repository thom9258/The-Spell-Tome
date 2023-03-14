# The Spell Tome

A personal spelltome of libraries that aim to greatly improve my c/c++ projects.
The libraries adheres to an ansi-c99 code style, and are mostly stb-like header only
libraries. For more information, see
![stb](https://nothings.org/stb/stb_h.html)
for details.

__Original author:__ Thomas Alexgaard Jensen (![https://github.com/thom9258](https://github.com/thom9258))

## Overview of libraries

Below are introductions/overviews of the contained libraries, and in some cases a quick example
of usage.

### th_allocator

th_allocator allows the creation of easy-to-use memory allocation techniques.
The allocators are memory aligned and thread-safe unless specifically disabled.
The allocators are also stdlib.h free, meaning they are ideal for embedded systems with no
heap to allocate from.
Supported allocator types are:

1. __stack-allocator__ An simple allocator for simple lifetime management of grouped variabe-size allocations

2. __dynamic-allocator__ A dynamic size allocator supporting the first-fit allocation scheme. This 
library is designed to have the same interface as posix malloc() / free(), and is benchmarked in tests to be faster than posix.

3. __block-allocator__ A fixed size block allocator that supports access through handles instead
of pointers. The block allocator allows allocation and freeing of individual blocks.

```c
    uint8_t memory[4096] = {0};
	th_stackallocator sa = {0};
	status = th_stackallocator_setup(&sa, memory, 4096);

    int* positions[8] = {0};
    int i;
    for (i = 0; i < 8; i++) {
        positions[i] = (int*)th_stackallocator_alloc(&sa, sizeof(int));
    /*Do something with allocated memory*/
    th_stackallocator_reset(&sa);
```

Note in the future all allocators will support a scratch buffer on invalid allocation to ensure no access of invalid memory.

### tsarray

A compile-time templated, typesafe dynamic array implementation. Supports any data type, supports constructor/destructor/copy/print operators for the chosen data type. Allows usage of custom memory management by omitting stdlib.h through macro.

To create any array type, simply create a datatype and include the header as shown here:

```c
typedef struct {
    /*Fill up your type*/
}SomeType;

/* =Optional= operator functions */
void SomeType_print(SomeType _a) { printf("print operator called"); }

#define t_type SomeType
#define t_operator_print(a) SomeType_print(a)
#include "../tsarray.h"
```

Note this can be done for as many types as the user wants, as long as the types are created at runtime, and the type/operators are defined before the array include.
By doing this, the compiler will generate all the appropiate array functionality for 'SomeType', under the name "arr_SomeType".

Note i would like to implement sorting through a "t_operator_larger_than()" define.

### tstring

Simple string library designed to replace standard string.h.
Creates a string type that is not based on raw char* functionality, but instead uses memory
and boundaries to ensure safer operation than raw pointers.

```c
str = tstring("Hello, World\n");
printf("text (%d chars) = \"%s\"\n", str.maxlen, str.c_str);
```

tstring supports useful string functions like comparison, upper-/lower-case conversion, concatenating, memory safe char/string search operations, string extraction etc, see tests
for full demonstration.

Note in the future i would like to implement simple regex operations and a string_view like
iterator for better management of strings.

### testlib

A unit testing suite I use to test all my libraries. 
Disclaimer: The library is based on what i find useful in testing, I have not researched what a
"pro" unit testing library is capable of doing.
testlib supports multiple tests, has nice formatting of output, counts errors on the fly and
prints a summary of all the tests at the end of the test run. testlib highlights what tests are wrong, and allows for the user to integrate randomization and timers into testing.

### error-flagger 

Soft error management system allowing the implementor to define error states and eloborate on those
through an error callback system.

### mlogger 

File logging for multiple log files. To be combined with error-flagger to create a more coherent error/logging experience.

### bubble game framework 

Stack based scene manager ideal for simple multi-layered applications such as games. 
Note this library is only a simple test of the idea. In the future the framework will be more
complicated and have a heavier emphasis on the before-mentioned libraries, and function in style
of a simple os-inspired service/message manager.

## License

__All libraries are licensed under the Zlib license (unless otherwise specified):__

Copyright (c) Thomas Alexgaard Jensen

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from
the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software in
   a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.

For more information, please refer to
![Zlib License](https://choosealicense.com/licenses/zlib/)

