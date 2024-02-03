# DARRAY DOCUMENTATION

Dynamic Array - a self-modifying variable-size templated array in ansi-c99.

## Acknowlegdement

Implementation is somewhat based on implementations found in the
ALTERNATIVES & RESOURCES section.

## ORIGINAL AUTHOR

Thomas Alexgaard Jensen (![gitlab/Alexgaard](https://gitlab.com/Alexgaard))

```txt
   __________________________
  :##########################:
  *##########################*
           .:######:.
          .:########:.
         .:##########:.
        .:##^:####:^##:.
       .:##  :####:  ##:.
      .:##   :####:   ##:.
     .:##    :####:    ##:.
    .:##     :####:     ##:.
   .:##      :####:      ##:.
  .:#/      .:####:.      \#:.
```

## LICENSE

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
![Zlib License](https://choosealicense.com/licenses/zlib/).

## DESCRIPTION

a stb-style header only library implemented in ansi c.

For more information, refer to
![stb_howto](https://github.com/nothings/stb/blob/master/docs/stb_howto.txt).

Dynamic array implementation based on "stretchy buffer" by Sean Barret
for ansi-c99 and a working c++ implementation for consistency and
compatability.
Designed to be a quick, verbally typed container using a phsuedo-templated
design.
Heavily reliant on macros.
Is designed to be readable, by using a consistent verbal notation like the
one used in "`std::vector<T>`".

## ALTERNATIVES & RESOURCES

- ![minimalist-container-in-c](https://ourmachinery.com/post/minimalist-container-library-in-c-part-1/)

- ![stb_ds](https://github.com/nothings/stb/blob/master/stb_ds.h)

- ![gunslinger](https://github.com/MrFrenik/gunslinger/blob/master/gs.h)

## DEFINITIONS

### Struct definitions

`array_header_s` Struct maintaining hidden members for an array, such as
allocators, current & maximum size of array etc.

### Macro definitions

```c
DARRAY_GROWTH_SCALE
```

Manage standard growth rate scale when array needs to be automatically resized.

```c
DARRAY_START_SIZE
```

Starting size of newly initialized arrays.

```c
DARRAY_OMIT_STDLIB_INCLUDE
```

Can be defined to omit include of "'stdlib.h'".
If not defined, init allocator macros are defined to be standard
malloc, realloc & free.

```c
DARRAY_INIT_FN_MALLOC
DARRAY_INIT_FN_REALLOC
DARRAY_INIT_FN_FREE
```

## INITIALIZATION & TEMPLATING

```c
array_new(TYPE)
```

Becomes `TYPE*`, used for descriptive, templated initialization for dynamic
array.

```c
array_init(ptr)
```

Initializes an array with size of DARRAY_START_SIZE.
Sets allocators to malloc, realloc & free from stdlib.h include.

```c
array_init_n(ptr, size)
```

Initializes an array with size of `size`.
Sets allocators to malloc, realloc & free from stdlib.h include.

```c
array_header(ptr)
```

Returns struct `array_header_s` associated with array.

```c
array_size(ptr)
```

Returns current size of array.

```c
array_max(ptr)
```

Returns maximum size of array.

### Generic usage

```c
array_is_full(ptr)
```

Returns 1 if array is full `size == max`, 0 otherwise.

```c
array_is_empty(ptr)
```

Returns 1 if array is empty `size == 0`, 0 otherwise.

```c
array_free(ptr)
```

Frees allocated array memory, __must__ be used as 'free(ptr)' does not free all
memory.

```c
array_clear(ptr)
```

Simple clearing of array by setting current size to 0.

```c
array_shrink_fit(ptr)
```

Resizes array to only fit current amount of entries.

```c
array_resize(ptr, growth)
```

Resizes array to specified size.

```c
array_push(ptr, item)
```

push entry to array end. internally initializes array if array is not
initialized, and internally resizes array if array is full.

```c
array_pop(ptr)
```

Pops last element from array, and removes it.

```c
array_dequeue(ptr)
```

Dequesus first element of array, relocates all other elements to eliminate
hole in entries.

```c
array_swap(ptr, i, j)
```

Swap specified array entries.

### Build-in iterators

```c
array_foreach(TYPE, VARNAME, ptr)
```

Phsuedo "foreach" loop specifing a variable type (same as array type) and the
name definition of the variable type to use. Incrementation is automatically
handled, and will range from start to end of array, iterating over all entries.

```c
array_iterator(VARNAME, ptr)
```

Phsuero iterator that specifies an already initialized variable (same as
array type) and the name definition of the variably type
(could be `it` like in c++), does not manually increment, allowing for larger
flexibility over `array_foreach()`.

**Note** Loops infinitely if no incrementation or manual termination is specified.
Make sure to manually increment with the decired incrementation.

### Serialization

```c
array_raw(ptr)
```

Returns raw array including `array_header_s` as `uint8_t` byte array, ideal for
hashing and storing array data statically.

```c
array_raw_size(ptr)
```

Returns size of raw byte array.

## Pool Allocator handlers

Pool allocators are specifically used when custom allocation functions are needed,
such as when using an allocator pool implementation.

```c
array_init_pooled(ptr, _fn_malloc, _fn_realloc, _fn_free)
```

Initializes an array with size of `DARRAY_START_SIZE`.
Sets allocators to malloc, realloc & free from stdlib.h include.

```c
array_init_n_pooled(ptr, size, _fn_malloc, _fn_realloc, _fn_free)
```

Initializes an array with size of `size`.
Sets allocators to malloc, realloc & free from stdlib.h include.

## Memory Layout

The memory layout is unneeded information in order to use the library, but is
explained in order to provide a deeper understanding of what allows the
dynamic array to become templated, and subsequently allow it to contain sizeing
information without it being exposed through structs.

The core idea is that creating an array is the same as creating an normal
allocated array, based on some static size, this is done implicitly when
using `array_push()`, if the array itself has not been allocated, eg = NULL.

Upon allocation, the size of the array is determined through a specified
size macro `DARRAY_START_SIZE`, with the idea that enough memory is allocated to
accomedate `array_header_s + TYPE * START_SIZE`.
After allocation of the specified size, the memory allocation looks like this:

```txt
____________________________   __________
| Header | Item 1 | Item 2 |...| Item N |
|________|________|________|   |________|
 ^
 |_ Allocated pointer location.
```

The header is then loaded with the current and max size of the array.
Then, in order to return the newly allocated array, the pointer is incremented
by `sizeof(array_header_s)` making the returned array become:

```txt
____________________________   __________
| Header | Item 1 | Item 2 |...| Item N |
|________|________|________|   |________|
          ^
          |_ Returned pointer location.
```

This effectively "hides" the header information, allowing explicit data be
hidden at negative indexes of the array, this also subsequently makes the array
templated, as the header has a fixed size, and the returned pointer is casted
to become the specified type of the created pointer.
The entire process of allocating looks like this:

```c
array_new(int32_t) arr = {0};
array_push(arr, 11);

printf("%d\n", arr[0]);
```

This is wanted because we effectively create a descriptive way of defining a
dynamic array, that effectively hides information about the array, while
subsequently allowing for access like that of the `[]` operator in c++.
On top of hiding header information this also succeeds in making effective
use of a phsuedo-template approach.

## COOKBOOK

### Allocating and displaying array data

```c
uint32_t i;
array_new(int32_t) numbers = {0};
array_push(numbers, 69);
array_push(numbers, 420);
array_push(numbers, 255);
array_push(numbers, 0);
array_push(numbers, 11);

array_foreach(int32_t, number, numbers)
    printf("%d ", *number);
printf("\n");

array_free(numbers);
```

This code initializes a templated array of `int32_t`, dynamically adds and
resizes indexes to the array, and then descriptively uses a foreach syntax
(imagine "foreach number in numbers") in order to display the entries.
It should be noted that the specified value is a pointer to the element, not a
copy.

### Allocating custom data types

```c
typedef struct PAGE_4096 {
    uint64_t id;
    uint8_t  data[4096];
}PAGE_4096;

...
uint32_t i;
array_new(PAGE_4096) pages = {0};
PAGE_4096 PAGE_TEMPLATE = {0,{0}};
for (i = 0; i < 1000; i++) {
    array_push(pages, PAGE_TEMPLATE);
    pages[i].id = i;
}

PAGE_4096* it = pages;
array_iterator(it, pages) {
    printf("page (id = %ld, size = %ld)\n", it->id, sizeof(it->data));
    it++;
}

array_free(pages);
```

This code creates a dynamically resizeable array of a custom large `PAGE_4096`
datatype. then iterates through them as an iterator.

### Stack and Queue

```c
int i;
array_new(int32_t) arr = {0};

for (i = 0; i < 10; i++)
    array_push(arr, i);
array_foreach(int32_t, v, vars) printf("%d ", *v);
printf("\n");

for (i = 0; i < 3; i++)
    array_pop(arr, i);
array_foreach(int32_t, v, vars) printf("%d ", *v);
printf("\n");

for (i = 0; i < 2; i++)
    array_dequeue(arr);
array_foreach(int32_t, v, vars) printf("%d ", *v);
printf("\n");

array_free(arr);
```

Using `array_push()`, `array_pop()` & `array_dequeue()`, a stack and a queue can
be easily implemented.

### Using custom pool allocators

The required pool allocator needs to accomedate allocators of malloc, realloc & free
using the same function declaration setup.

```c
uint32_t i;
array_new(int32_t) numbers = {0};
array_init_pooled(arr, pool_malloc, pool_realloc, pool_free);

array_push(numbers, 69);
array_push(numbers, 420);
array_push(numbers, 255);
array_push(numbers, 0);
array_push(numbers, 11);

array_foreach(int32_t, number, numbers)
    printf("%d ", *number);
printf("\n");

array_free(numbers);
```
