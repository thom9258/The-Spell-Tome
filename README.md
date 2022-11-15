# The Spell Tome

A personal spelltome of libraries that greatly improves c codebases.
The libraries adheres to an ansi-c99 code style, and are stb-like header only
libraries. For more information, see
![stb](https://nothings.org/stb/stb_h.html)
for details.

__Original author:__ Thomas Alexgaard Jensen (![https://gitlab.com/Alexgaard](https://gitlab.com/Alexgaard))

## Allocators

Libraries focused on memory allocation.

1. __arena-allocator__ An Arena allocator for super simple lifetime management of allocations

2. __block-allocator__ A fixed size block allocator

3. __dynamic-allocator__ A dynamic size allocator with different fit algorithms

## Datastructures

1. __typesafe-array__ A templated, typesafe at compile time, dynamic array implementation

2. __linkedlist__ A simple & standard linked-list implementation

3. __tstring__ String library for replacing standard string.h

## Utilities

1. __alg-utils__ A simple macro based utility library focused on array manipulation

3. __error-flagger__ Soft error management

4. __mlogger__ File logging for multiple log files

5. __psuedo-random__ Seed based randomization algorithms

6. __testlib__ Unit testing suite for libraries

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

