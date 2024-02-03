/*
SHORT DESCRIPTION:
	Dynamic Array - a self-modifying variable-size templated array in ansi-c99. 

	Acknowlegdement: Implementation is somewhat based on implementations found
	                 in the ALTERNATIVES & RESOURCES section.

ORIGINAL AUTHOR:
	Thomas Alexgaard Jensen (https://gitlab.com/Alexgaard)

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


LICENSE:
	Copyright (c) <Thomas Alexgaard Jensen>
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
	<https://choosealicense.com/licenses/zlib/>

MISSING:
	- Boundary checking on macros and static assertion on error.
	- array_concatenate() function.
	- array_get_by_idx() function, "dequeue" but for specified index.
	- sorting algorithm 
	  (provide "is_greater(a,b) -> true,false" to template sort)
	- reverse algorithm (just swap each side of the array until middle is
	  reached)

CHANGELOG:
	[2.0] Removed short names, if needed, define yourself.
	      Added macros for standard allocators, will be used if no specific
	      allocator pool is specified.
	      Added allocator function pointers to header.
	      Added Pool allocation setter macros, and pre-initialization macros
	      with option to specify start size of array. Updated free macro to use
	      free member function of array_header.
	      Added c++ solution to _internal_array_new.
	      Added c++ solution to _internal_array_resize.
	      Memory pool allocators fully implemented.
	[1.7] Changed array_iterator() to not initialize iterater itself, now
	      iterator needs manual initialization. This allows for iteration of 
	      non-zero start indexes.
	      Added stb_ds.h as alternative resource.
	[1.6] Rewrote header information.
	      Finished Documentation.
	      Finished Cookbook.
	[1.5] Large cleanup of implementation:
	      Fixed header declaration of _internal_array_resize() to specify
	      input difference between c and c++ implementation.
	      Fixed bug in _internal_array_byte_swapper().
	      Cleaned up leftover commented code in _internal_array_resize().
	      Fixed naming in variable macros GROWTH_SCALE and START_SIZE.
	      Removed unused includes.
	      Specified usage for <used> includes for clarification.
	      cleaned up complex macros for readability.
	[1.4] Implemented c++ compatability, completing the same tests as the c99
	      impmenentation.
	      Added safety to _internal_array_byte_swapper() when out-of-bounds.
	[1.3] Started implementation of c++ compatability.
	[1.2] Fixed dequeue memory error. Added a MISSING section, for overview of
	      what could be implemented.
	[1.1] Implemented swapping operation, and unfinished dequeue operation.
	[1.0] Core functionality implemented.
	[0.0] Initialized library.
*/
/*TODO: Implement settable growth-rate*/

/*TODO: Implement a alg library with templated algorithms such as swap*/
#define alg_swap(p1, p2, n)

#ifndef DARRAY_H
#define DARRAY_H

#include <stdint.h> /*uint32_t + uint8_t*/

#ifndef DARRAY_NO_STDLIB
	#include <stdlib.h> /*malloc + realloc + free + size_t*/
	#define DARRAY_SIZE_T   size_t
    #define DARRAY_MALLOC   malloc
    #define DARRAY_REALLOC  realloc
    #define DARRAY_FREE     free
#else
	#define DARRAY_SIZE_T   long unsigned int
#endif

#ifndef DARRAY_GROWTH 
#define DARRAY_GROWTH 1.5f
#endif

#ifndef DARRAY_START_SIZE
#define DARRAY_START_SIZE 3
#endif

typedef struct {
/*TODO: Extremely wierd bug where if the allocators are placed last in the 
 *      struct things break, but not when they are placed first.*/
	void* (*fn_malloc)  (DARRAY_SIZE_T);
	void* (*fn_realloc) (void*, DARRAY_SIZE_T);
	void  (*fn_free)    (void*);
    float    growth;
	uint32_t size;
	uint32_t max;
}_array_header_s;

/* array_new(T) 
 * Becomes `T*`, used for descriptive, templated initialization.
 * */
#define array_new(T) T*

/* array_header(ARR)
 * Provides descriptive pointer to header.
 * */
#define array_header(ARR) ( (_array_header_s*)ARR - 1 )

/* array_init_full(ARR, N, GROWTH, MALLOC, REALLOC, FREE)
 * Initializes an array.
 * Sets size to be "N"
 * Sets growth scale to be "GROWTH"
 * Sets array allocators to "MALLOC", "REALLOC" & "FREE"
 * */
#define array_init_full(ARR, N, GROWTH, MALLOC, REALLOC, FREE)                 \
    ARR = _internal_array_new(ARR,                                             \
                              sizeof(*ARR),                                    \
                              N,                                               \
                              GROWTH,                                          \
                              MALLOC,                                          \
                              REALLOC,                                         \
                              FREE)

/* array_init(PTR)
 * Initializes an array.
 * Sets array configuration to defaults. 
 * */
#define array_init(ARR)                                                        \
    array_init_full(ARR,                                                       \
                    DARRAY_START_SIZE,                                         \
                    DARRAY_GROWTH                                              \
                    DARRAY_MALLOC,                                             \
                    DARRAY_REALLOC,                                            \
                    DARRAY_FREE)

/* array_init_n(ARR, N)
 * Initializes an array.
 * Sets size to be "N"
 * Sets rest of array configuration to defaults. 
 * */
#define array_init_n(ARR, N)                                                   \
    array_init_full(ARR,                                                       \
                    N,                                                         \
                    DARRAY_GROWTH                                        \
                    DARRAY_MALLOC,                                             \
                    DARRAY_REALLOC,                                            \
                    DARRAY_FREE)

/* array_init_ng(PTR, N, GROWTH)
 * Initializes an array.
 * Sets size to be "N"
 * Sets growth scale to be "GROWTH"
 * Sets rest of array configuration to defaults. 
 * */
#define array_init_ng(PTR, N, GROWTH)                                          \
    array_init_full(ARR,                                                       \
                    N,                                                         \
                    GROWTH,                                                    \
                    DARRAY_MALLOC,                                             \
                    DARRAY_REALLOC,                                            \
                    DARRAY_FREE)

/* array_free(PTR)
 * Frees array using configured free function of array.
 * */
#define array_free(PTR)                                                        \
	do {                                                                       \
		void (*tmp_fn_free)(void*) = array_header(PTR)->fn_free;               \
		tmp_fn_free(array_raw(PTR));                                           \
	} while (0)

/* array_size(PTR)
 * Returns current size of array.
 * */
#define array_size(PTR) ( (PTR != NULL) ? array_header(PTR)->size : 0 )

/* array_max(PTR)
 * Returns max size of array.
 * */
#define array_max(PTR) ( (PTR != NULL) ? array_header(PTR)->max : 0 )

/* array_growth(PTR)
 * Returns growth scale of array.
 * */
#define array_growth(PTR) ( (PTR != NULL) ? array_header(PTR)->growth : 0 )

/* array_is_full(PTR)
 * Returns 1 if array is full, 0 otherwise.
 * */
#define array_is_full(PTR) ( array_size(PTR) >= array_max(PTR) )

/* array_is_empty(PTR)
 * Returns 1 if array is completely empty, 0 otherwise.
 * */
#define array_is_empty(PTR) ( array_size(PTR) <= 0 )

/* array_clear(PTR)
 * Clears array elements.
 * Note: Clears by resetting array indexing, elements are not zero'ed. 
 * */
#define array_clear(ptr) array_header(ptr)->size = 0

/* array_resize(PTR)
 * Resize array to specified amount of entries.
 * */
#define array_resize(ptr, growth)                                              \
    _internal_array_resize(ptr, sizeof(*ptr), growth)

/* array_grow(PTR)
 * Grow array to facilitate a larget amount of entries.
 * */
#define array_grow(ARR)                                                        \
    (ARR == NULL) ?                                                            \
        array_resize(ARR, 1 + array_size(ARR) * DARRAY_GROWTH) :         \
        array_resize(ARR, 1 + array_size(ARR) * array_header(ARR)->growth)



/* array_shrink_fit(PTR)
 * Resize array memory to fit current amount of entries.
 * */
#define array_shrink_fit(ptr) array_resize(arr, array_size(arr));

/* array_push(PTR, ELEM)
 * Add element "ELEM" to end of array.
 * */
#define array_push(ARR, ELEM)                                                  \
    (                                                                          \
        (array_is_full(ARR)) ?                                                 \
         array_grow(ARR), (ARR[array_header(ARR)->size++] = ELEM) :            \
        (ARR[array_header(ARR)->size++] = ELEM)                                \
    )

/* array_pop(PTR)
 * Get last element from array, and remove it from array.
 * */
#define array_pop(PTR) ( (array_is_empty(PTR)) ? 0 :                           \
    PTR[array_header(PTR)->size--] )

/* array_dequeue(PTR)
 * Get first element from array, and relocate all elements down 1 element.
 * */
#define array_dequeue(PTR)                                                     \
    (                                                                          \
        (array_is_empty(PTR)) ? 0 :                                            \
        PTR[array_header(PTR)->size--],                                        \
        _internal_array_dequeue_relocate(                                      \
            array_raw(PTR) + sizeof(_array_header_s),                           \
            sizeof(*PTR),                                                      \
            (array_size(PTR) + 1))                                             \
    )

/* array_swap(PTR)
 * Swap 2 elements from array.
 * Note: Swapping is bytewise.
 * */
#define array_swap(PTR, i, j)                                                  \
    _internal_array_byte_swapper(                                              \
        array_raw(PTR) + sizeof(_array_header_s) + (sizeof(*PTR) * i),          \
        array_raw(PTR) + sizeof(_array_header_s) + (sizeof(*PTR) * j),          \
        sizeof(*PTR))

/* array_foreach(VAR, PTR)
 * Iterate through all elements of array.
 * "VAR" should be same type as elements in "PTR", and has to be defined before
 * call.
 * */
#define array_foreach(VAR, PTR)                                                \
    for (VAR = PTR; VAR != &PTR[array_size(PTR)]; VAR++)

/* array_iterator(VARPTR, PTR)
 * Create iterator for array.
 * "VARPTR" should be same type ptr as elements in "PTR", and has to be defined
 * before call.
 * Note: unlike "array_foreach()" Needs manual incrementation of "VARPTR".
 * */
#define array_iterator(VARPTR, PTR)                                            \
    while (VARPTR != &PTR[array_size(PTR)])

/* array_raw(VAR)
 * Get start of allocation for array. return type is void*
 * */
#define array_raw(PTR) ( (void*)PTR - sizeof(_array_header_s) )

/* array_raw(VAR)
 * Get start of allocation for array. return type is uint8_t*
 * */
#define array_raw_u8(PTR) ( (uint8_t*)array_raw(PTR) )

/* array_raw_size_u(VAR)
 * Get array size in bytes. 
 * Return type is uint32_t
 * */
#define array_raw_size_u(PTR)                                                  \
    ( sizeof(_array_header_s) + array_size(PTR) * sizeof(PTR[0]) )

#define array_raw_size(PTR)                                                    \
    ( (int32_t)array_raw_size_u(PTR) )

#ifdef __cplusplus
template<class T>
T*
_internal_array_new(T* _p,
                    DARRAY_SIZE_T _s,
                    uint32_t _n,
                    float _growth,
                    void*(*_fn_malloc)(DARRAY_SIZE_T),
                    void*(*_fn_realloc)(void*, DARRAY_SIZE_T),
                    void (*_fn_free)(void*));

template<class T>
T*
_internal_array_resize(T* _p, size_t _s, uint32_t _n);
#else /*NOT __cplusplus*/
void*
_internal_array_new(void* _p,
                    size_t _s,
                    uint32_t _n,
                    float _growth,
                    void*(*_fn_malloc)(size_t),
                    void*(*_fn_realloc)(void*, size_t),
                    void (*_fn_free)(void*));

void*
_internal_array_resize(void* _p, size_t _s, uint32_t _n);
#endif /*__cplusplus*/

size_t
_internal_array_byte_swapper(uint8_t* _p1, uint8_t* _p2, size_t _s);

void*
_internal_array_dequeue_relocate(uint8_t* _p, size_t _s, uint32_t _n);

/******************************************************************************/
#ifdef DARRAY_IMPLEMENTATION

#ifdef __cplusplus

template<class T>
T*
_internal_array_new(T* _p,
                    DARRAY_SIZE_T _s,
                    uint32_t _n,
                    float _growth,
                    void*(*_fn_malloc)(DARRAY_SIZE_T),
                    void*(*_fn_realloc)(void*, DARRAY_SIZE_T),
                    void (*_fn_free)(void*))
/**
 * [C++ Implementation]
 *
 * _internal_array_new() - Initialize array.
 * @arg1: ptr to array.
 * @arg2: size of a single index in array.
 * @arg3: size of total indexs in array.
 * @arg4: pointer to malloc function.
 * @arg5: pointer to realloc function.
 * @arg6: pointer to free function.
 * @arg7: growth rate.
 *
 * Initially allocated a empty array specified by a size and with defined
 * allocators for usage with allocation pools.
 *
 * Return: returns @arg1 after array allocation.
 */
{
	uint8_t* p;
	DARRAY_SIZE_T n;

	if (_p != NULL)
		return reinterpret_cast<T*>(_p);

	/*Allocate new array*/
	n = sizeof(_array_header_s) + (_s * _n);
	p = reinterpret_cast<uint8_t*>(_fn_malloc(n));
	p = (uint8_t*)p + sizeof(_array_header_s);
	array_header(p)->size   = 0;
	array_header(p)->max    = _n;
	array_header(p)->growth = _growth;

	/*set custom allocators*/
	array_header(p)->fn_malloc  = _fn_malloc;
	array_header(p)->fn_realloc = _fn_realloc;
	array_header(p)->fn_free    = _fn_free;

	return reinterpret_cast<T*>(p); 
}

template<class T>
T*
_internal_array_resize(T* _p, size_t _s, uint32_t _n)
/**
 * [C++ Implementation]
 *
 * array_resize() - Resize and initialize array.
 * @arg1: ptr to array start.
 * @arg2: size of index in array.
 * @arg3: size of total indexs in array.
 *
 * CPLUSPLUS compatability implementation using a emplated class T.
 * Implemented as a Template class T using reinterpret_cast to accomedate strict
 * type conversion in c++.
 * Initially suggested by "cocoademon" (2016/4/14) at:
 * https://github.com/nothings/stb/issues/250
 *
 * New array (unallocated):
 * Allocates array with size of define <DARRAY_START_SIZE>.
 *
 * Existing valid array:
 * uses @arg3 determine size of array reallocation.
 * Using define <array_resize()>, automatically manages templated variable type. 
 *
 * Recommended @arg3 = <(old_size * growth_rate) + 1>, to ensure array
 * size incrementation for <old_size < 2>. 
 *
 * Return: Pointer to resized array, NULL if memory management fails.
 */
{
	uint8_t* p = NULL;
	DARRAY_SIZE_T n;

	/*Allocate array using default allocators*/
	if (_p == NULL) {
		return _internal_array_new(_p,
                                   _s,
                                   DARRAY_START_SIZE, 
		                           DARRAY_MALLOC, 
		                           DARRAY_REALLOC, 
		                           DARRAY_FREE);
	}

	/*Resize array*/
	n = sizeof(_array_header_s) + (_s * _n);
	p = reinterpret_cast<uint8_t*>(
	    array_header(_p)->fn_realloc(array_raw(_p), n));
	p += sizeof(_array_header_s);
	array_header(p)->max = _n;
	return reinterpret_cast<T*>(p);
}

#else /*NOT __cplusplus*/

void*
_internal_array_new(void* _p,
                    DARRAY_SIZE_T _s,
                    uint32_t _n,
                    float _growth,
                    void*(*_fn_malloc)(DARRAY_SIZE_T),
                    void*(*_fn_realloc)(void*, DARRAY_SIZE_T),
                    void (*_fn_free)(void*))
/**
 * [C99 Implementation]
 *
 * _internal_array_new() - Initialize array.
 * @arg1: ptr to array.
 * @arg2: size of a single index in array.
 * @arg3: size of total indexs in array.
 * @arg4: growth scale for reallocations.
 * @arg5: pointer to malloc function.
 * @arg6: pointer to realloc function.
 * @arg7: pointer to free function.
 *
 * Initially allocated a empty array specified by a size and with defined
 * allocators for usage with allocation pools.
 *
 * Return: returns @arg1 after array allocation.
 */
{
	void* p;
	DARRAY_SIZE_T n;

	if (_p != NULL)
		return _p;

	n = sizeof(_array_header_s) + (_s * _n);
	p = _fn_malloc(n);
	p = (uint8_t*)p + sizeof(_array_header_s);
	array_header(p)->size   = 0;
	array_header(p)->max    = _n;
	array_header(p)->growth = _growth;

	/*set custom allocators*/
	array_header(p)->fn_malloc  = _fn_malloc;
	array_header(p)->fn_realloc = _fn_realloc;
	array_header(p)->fn_free    = _fn_free;
	return p;
}

void*
_internal_array_resize(void* _p, DARRAY_SIZE_T _s, uint32_t _n)
/**
 * [C99 Implementation]
 *
 * _internal_array_resize() - Resize and initialize array.
 * @arg1: ptr to array.
 * @arg2: size of a single index in array.
 * @arg3: size of total indexs in array.
 *
 * New array (unallocated):
 * Allocates array using _internal_array_new() with <DARRAY_START_SIZE> and
 * standard allocators.
 *
 * Existing valid array:
 * uses @arg3 determine size of array reallocation.
 * Uses fn_realloc member to reallocate.
 *
 * Recommended @arg3 = <(old_size * growth_rate) + 1>, to ensure array growth 
 * for <old_size < 2>. 
 *
 * Return: Pointer to resized array, NULL if memory management fails.
 */
{
	void* p;
	DARRAY_SIZE_T n;
	/*Allocate array using default allocators*/
//    printf("RESIZE FN %d\n", __LINE__);
	if (_p == NULL) {
//        printf("ALLOCATING NEW %d\n", __LINE__);
//        printf("size(%d)growth(%f)\n", DARRAY_START_SIZE, DARRAY_GROWTH);
        //return _internal_array_new(_p, 
        void* pnew =  _internal_array_new(_p, 
                                   _s,
                                   DARRAY_START_SIZE, 
                                   DARRAY_GROWTH, 
		                           DARRAY_MALLOC, 
		                           DARRAY_REALLOC, 
		                           DARRAY_FREE);
//        printf("Created new %p\n", pnew);
        return pnew;
	}

    printf("JUST RESIZEING %d\n", __LINE__);
	/*Resize array*/
	n = sizeof(_array_header_s) + (_s * _n);
	p = array_header(_p)->fn_realloc(array_header(_p), n);
	p = (uint8_t*)p + sizeof(_array_header_s);
	array_header(p)->max = _n;
	return p;
}

#endif /*__cplusplus*/

size_t
_internal_array_byte_swapper(uint8_t* _p1, uint8_t* _p2, size_t _s)
/*TODO: Make swap into a macro*/
/**
 * _internal_array_byte_swapper() - swap bytes of array entries.
 * @arg1: ptr to byte array index 1.
 * @arg2: ptr to byte array index 2.
 * @arg3: amount of memory to swap.
 * 
 * Effectively swaps two blocks of memory, can be used to swap elements of
 * array blocks. 
 *
 * Swaps (@arg3) bytes between (@arg1) and (@arg2).
 * (@arg1) and (@arg2) are intended to be entries of the same array, 
 * but it can also be used between arrays of the same type.
 * (Otherwise behaviour is unspecified)
 *
 * Return: size of bytes swapped.
 */
{
	size_t i;
	uint8_t buf;
	if (_p1 == NULL || _p2 == NULL)
		return 0;
	for (i = 0; i < _s; i++) {
		if (_p1 + i == NULL)
			return i;
		buf = _p1[i];
		_p1[i] = _p2[i];
		_p2[i] = buf;
	}
	return i;
}

void*
_internal_array_dequeue_relocate(uint8_t* _p, size_t _s, uint32_t _n)
/**
 * _internal_array_dequeue_relocate() - relocate bytes in array.
 * @arg1: ptr to byte buffer start.
 * @arg2: size of memory to reallocate.
 * @arg3: size of entries of size @arg2 to relocate.
 *
 * Relocates @arg3 bytes of size @arg2, backwards @arg2 bytes, starting from 
 * @arg1.
 *
 * Effectively reallocates blocks of data back a single index in the array of
 * blocks.
 *
 * Return: Pointer to reallocated block (equal to @arg1).
 */
{
	size_t i;
	if (_p == NULL)
		return NULL;
	for (i = 0; i < _n * _s; i++)
		_p[i-_s] = _p[i];
	return _p;
}

#endif /*DARRAY_IMPLEMENTATION*/
#endif /*DARRAY_H*/
