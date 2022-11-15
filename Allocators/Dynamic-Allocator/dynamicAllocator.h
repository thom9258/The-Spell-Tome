/*
# DYNAMICALLOCATOR
	Simple dynamic-size Allocator written in ansi-c99.

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
![Zlib License](https://choosealicense.com/licenses/zlib/)

## CHANGELOG

TODO: fit algorithm speedups required before actually being useable.

- [0.3] Started implementation of optional c++ wrapper.
        Redesigning some parts for better useability, removing all stdlib.
- [0.2] Implemented Book keeping & allignment macros.
        Implemented end_fit algorithm
        Implemented best_fit algorithm (unfinished)
        Revamped free algorithm to be better formulated.
- [0.1] Implemented Core utils:
        - init/destroy of allocator.
        - Debugging of allocator.
        - Freeing of allocations in allocator.
        - first fit implemented.
        - setup for fit algorithms implemented.
- [0.0] Initialized library.

## DOCUMENTATION

### MACROS
	If Defined:
		DYNAMICALLOCATOR_IMPLEMENTATION
            Include definitions for header.
        DYNAMICALLOCATOR_DEBUG
            Include Debug utilities.

		DYNAMICALLOCATOR_NO_ALIGNMENT
			Force no allignment.
		DYNAMICALLOCATOR_NO_ASSERT
			Force no assertions.
		DYNAMICALLOCATOR_NO_BOOK_KEEPING
			Force no memory usage book keeping.
        DYNAMICALLOCATOR_NO_THREAD_SAFETY
            Force no thread safety when allocating/freeing blocks.

*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DYNAMICALLOCATOR_H
#define DYNAMICALLOCATOR_H

#include "stdio.h"
#include "stdint.h"
#include "assert.h"

#ifndef DYNAMICALLOCATOR_API
#define DYNAMICALLOCATOR_API static inline
#endif /*DYNAMICALLOCATOR_API*/

#ifndef DYNAMICALLOCATOR_BACKEND
#define DYNAMICALLOCATOR_BACKEND static
#endif /*DYNAMICALLOCATOR_BACKEND*/

#ifndef DYNAMICALLOCATOR_NO_ASSERT
#define DYNAMICALLOCATOR_ASSERT(MSG) assert(MSG)
#else /*NOT DYNAMICALLOCATOR_NO_ASSERT*/
#define DYNAMICALLOCATOR_ASSERT(MSG)
#endif /*DYNAMICALLOCATOR_NO_ASSERT*/

#define DYNAMICALLOCATOR_KB(N) ((N)*1024)
#define DYNAMICALLOCATOR_MB(N) ((N)*1024*1024)

typedef struct {
	uint8_t* next;
	uint32_t size;
}_dynAllocator_dataheader_s;

typedef struct dynAllocator dynAllocator_s;
struct dynAllocator {
	/*Dynamic allocator*/
	uint8_t  alignment;
	uint8_t* memory;
	uint32_t memsize;

    /*Mutex*/
	uint8_t is_in_use;

	/*Book-Keeping*/
    struct {
        uint32_t alloc_count_current;
        uint32_t alloc_count_total;
        uint32_t used_memory;
    }book_keep;

    /*data header pointers that may be used by management functions*/
    uint8_t* first;
    uint8_t* last;

    /*User-defineable memory management functions*/
    void* (*fit_algorithm)(dynAllocator_s* _da, uint32_t _size);
    void* (*free_algorithm)  (dynAllocator_s* _da, void* _ptr);
};

typedef struct {
	dynAllocator_s* allocator;
	uint8_t  is_initialized;

	uint32_t alloc_count_current;
	uint32_t alloc_count_total;
	uint32_t max_memory;
	uint32_t used_memory;
	uint32_t free_memory;
}dynAllocator_info_s;


DYNAMICALLOCATOR_API
char
dynAllocator_init(dynAllocator_s* _da,
                  void* _memory,
                  uint32_t _memsize,
                  uint8_t _alignment);

DYNAMICALLOCATOR_API
void*
dynAllocator_malloc(dynAllocator_s* _da, int _size);

DYNAMICALLOCATOR_API
void*
dynAllocator_free(dynAllocator_s* _da, void* _ptr);

DYNAMICALLOCATOR_API
void*
dynAllocator_free_all(dynAllocator_s* _da);

DYNAMICALLOCATOR_API
dynAllocator_info_s
dynAllocator_info(dynAllocator_s* _da);

/* Standard Fit and Free functions
 * */

DYNAMICALLOCATOR_BACKEND
void*
dynAllocator_first_fit(dynAllocator_s* _da, uint32_t _size);

DYNAMICALLOCATOR_BACKEND
void*
dynAllocator_secure_free(dynAllocator_s* _da, void* _ptr);

/******************************************************************************/
#ifdef DYNAMICALLOCATOR_IMPLEMENTATION

DYNAMICALLOCATOR_BACKEND
_dynAllocator_dataheader_s*
_dynAllocator_data2header(void* _p)
/**
 * _dynAllocator_data2header() - return header from raw ptr.
 * @arg1: Ptr to raw entry.
 *
 * Context: Assumes passed ptr is valid and pointing to existing entry.
 */
{
	return ((_dynAllocator_dataheader_s*)_p) - 1;
}

DYNAMICALLOCATOR_BACKEND
uint8_t*
_dynAllocator_header2data(_dynAllocator_dataheader_s* _h)
/**
 * _dynAllocator_header2data() - return raw ptr from header.
 * @arg1: Ptr to haeder.
 *
 * Context: Assumes passed ptr is valid and pointing to existing entry.
 * Return: NULL on error, raw data ptr on clean exit.
 */
{
	return ((uint8_t*)_h) + sizeof(_dynAllocator_dataheader_s);
}

DYNAMICALLOCATOR_BACKEND
void*
_dynAllocator_align(uint8_t* _ptr, uint8_t _alignment)
{
#ifndef DYNAMICALLOCATOR_NO_ALIGNMENT
	uint8_t offset = 0;
	if (_ptr == NULL)
		return NULL;
	if (_alignment == 0)
		return _ptr;
	offset = ((uintptr_t)(const long*)(_ptr)) & (_alignment-1);
	return _ptr + offset;
#else /*DEFINED DYNAMICALLOCATOR_NO_ALIGNMENT*/
	(void)_alignment;
	return _ptr;
#endif /*DYNAMICALLOCATOR_NO_ALIGNMENT*/
}

DYNAMICALLOCATOR_BACKEND
void*
_dynAllocator_find_entry(dynAllocator_s* _da, void* _ptr, void** _prev_entry)
/**
 * dynAllocator_find_entry() - Find entry in chain.
 * @arg1: Ptr to dynamic allocator.
 * @arg2: Ptr to entry to find.
 * @arg3: (Output) PtrPtr to prev entry.
 *
 * Enforces explicit freeing of all chain entries.
 * Sets internal allocation counter to 0.
 *
 * Return: found entry on clean exit, NULL on error.
 *         If entry was found, return ptr to that entry's previous entry,
 *         return NULL if found entry is first, or if @arg3 is NULL.
 */
{
	uint8_t* prev = NULL;
	/*Freeable entry is first entry*/
	if (_ptr == _da->first) {
		if (_prev_entry != NULL)
			*_prev_entry = NULL;
		return _da->first;
	}

	/*Find freeable entry inside entry chain*/
	prev = _da->first;
	while (prev != NULL) {
		if (_ptr == _dynAllocator_data2header(prev)->next) {
			if (_prev_entry != NULL)
				*_prev_entry = prev;
			return _dynAllocator_data2header(prev)->next;
		}
		prev = _dynAllocator_data2header(prev)->next;
	}
	if (_prev_entry != NULL)
		*_prev_entry = NULL;
	return NULL;
}

DYNAMICALLOCATOR_BACKEND
uint8_t*
_dynAllocator_entry_create(uint8_t* _start,
                           uint8_t* _next,
                           uint32_t _size,
                           uint8_t _alignment)
/**
 * _dynAllocator_entry_create() - create chain entry at specified location.
 * @arg1: Ptr to start entry.
 * @arg2: Ptr to next entry in chain.
 * @arg3: Size of entry data.
 * @arg4: alignment of entry data.
 *
 * Creates an aligned chain entry at specified location.
 * If entry is last in chain, specify NULL at @arg2.
 *
 * Return: NULL on error, entry data ptr on clean exit.
 */
{
	_dynAllocator_dataheader_s* hptr = NULL;
	if (_start == NULL) {
		DYNAMICALLOCATOR_ASSERT("INPUT PTR INVALID");
		return NULL;
	}
	if (_size < 1) {
		DYNAMICALLOCATOR_ASSERT("CANNOT CREATE ALLOCATION WITH SIZE < 1");
		return NULL;
	}
	hptr = (_dynAllocator_dataheader_s*)_dynAllocator_align(_start, _alignment);
	hptr->next = _next;
	hptr->size = _size;
	return _dynAllocator_header2data(hptr);
}

DYNAMICALLOCATOR_API
char
dynAllocator_init(dynAllocator_s* _da,
                  void* _memory,
                  uint32_t _memsize,
                  uint8_t _alignment)
/**
 * dynAllocator_set() - Initialize Dynamic Allocator without allocation.
 * @arg1: Ptr to allocator to initialize.
 * @arg2: Size of memory pool for initialized allocator.
 * @arg3: Alignment requirement.
 * @arg4: Pointer to existing memory.
 *
 * Initialize dynamic allocator existing of memory pool.
 * Does not allocate heap memory.
 *
 * Return: 1 on error, 0 on clean exit.
 */
{
	if (_da == NULL || _memory == NULL || _memsize < 1)
		return 1;
	_da->memory    = _memory;
	_da->memsize   = _memsize;
	_da->alignment = _alignment;

	_da->book_keep.alloc_count_total   = 0;
	_da->book_keep.alloc_count_current = 0;
	_da->book_keep.used_memory         = 0;

	_da->fit_algorithm = dynAllocator_first_fit;
	_da->free_algorithm = dynAllocator_secure_free;
    return 0;
}

DYNAMICALLOCATOR_API
void*
dynAllocator_malloc(dynAllocator_s* _da, int _size)
/**
 * dynAllocator_malloc() - create entry.
 * @arg1: Ptr to dynamic allocator.
 * @arg2: size of entry to create.
 *
 * Creates a chain entry. Follows fit methodology of specified fit function.
 * increments internal allocation counter.
 *
 * Return: NULL if entry cannot be created, created entry data ptr on clean 
 *         exit.
 */
{
	void* entry = NULL;
	if (_da == NULL || _size < 1)
		return NULL;

    #ifndef DYNAMICALLOCATOR_NO_THREAD_SAFETY
    while (_da->is_in_use)
        ;
    _da->is_in_use = 1;
    #endif /*DYNAMICALLOCATOR_NO_THREAD_SAFETY*/

	entry = _da->fit_algorithm(_da, _size);

	#ifndef DYNAMICALLOCATOR_NO_BOOK_KEEPING
	if (entry != NULL) {
		_da->book_keep.alloc_count_total++;
		_da->book_keep.alloc_count_current++;
		_da->book_keep.used_memory += _size;
	}
	#endif /*DYNAMICALLOCATOR_NO_BOOK_KEEPING*/

    #ifndef DYNAMICALLOCATOR_NO_THREAD_SAFETY
    _da->is_in_use = 0;
    #endif /*DYNAMICALLOCATOR_NO_THREAD_SAFETY*/
    
	return entry;
}

DYNAMICALLOCATOR_API
void*
dynAllocator_free(dynAllocator_s* _da, void* _ptr)
/**
 * dynAllocator_free() - remove entry.
 * @arg1: Ptr to dynamic allocator.
 * @arg2: Ptr to entry.
 *
 * Removes a chain entry.
 * Decrements internal allocation counter.
 *
 * Return: NULL.
 */
{
	if (_da == NULL || _ptr == NULL)
		return NULL;

    #ifndef DYNAMICALLOCATOR_NO_THREAD_SAFETY
    while (_da->is_in_use)
        ;
    _da->is_in_use = 1;
    #endif /*DYNAMICALLOCATOR_NO_THREAD_SAFETY*/

    _da->free_algorithm(_da, _ptr);

    #ifndef DYNAMICALLOCATOR_NO_THREAD_SAFETY
    _da->is_in_use = 0;
    #endif /*DYNAMICALLOCATOR_NO_THREAD_SAFETY*/
    return NULL;
}

DYNAMICALLOCATOR_API
void*
dynAllocator_free_all(dynAllocator_s* _da)
/**
 * dynAllocator_reset() - remove all entries.
 * @arg1: Ptr to dynamic allocator.
 *
 * Enforces explicit freeing of all chain entries.
 * Sets internal allocation counter to 0.
 *
 * Return: NULL.
 */
{
	if (_da == NULL)
		return NULL;
	/*TODO: Might cause problems as the internal header->next pointers are the
	 *      untouched. Solution: iterate though entries, and unlink manually.*/

    #ifndef DYNAMICALLOCATOR_NO_THREAD_SAFETY
    while (_da->is_in_use)
        ;
    _da->is_in_use = 1;
    #endif /*DYNAMICALLOCATOR_NO_THREAD_SAFETY*/

	_da->first = NULL;
	_da->last = NULL;
	_da->book_keep.alloc_count_current = 0;
	_da->book_keep.used_memory = 0;

    #ifndef DYNAMICALLOCATOR_NO_THREAD_SAFETY
    _da->is_in_use = 0;
    #endif /*DYNAMICALLOCATOR_NO_THREAD_SAFETY*/

	return NULL;
}

DYNAMICALLOCATOR_API
dynAllocator_info_s
dynAllocator_info(dynAllocator_s* _da)
/**
 * dynAllocator_info() - Get common dynamic allocator stats.
 * @arg1: Ptr to dynamic allocator.
 *
 * Return: useful information.
 */
{
	dynAllocator_info_s info = {0};
	if (_da == NULL)
		return info;
	if (_da->memory != NULL && _da->memsize > 0 &&
        _da->fit_algorithm != NULL && _da->free_algorithm != NULL)
		info.is_initialized = 1;

    #ifndef DYNAMICALLOCATOR_NO_THREAD_SAFETY
    while (_da->is_in_use)
        ;
    _da->is_in_use = 1;
    #endif /*DYNAMICALLOCATOR_NO_THREAD_SAFETY*/

	info.allocator           = _da;
	info.alloc_count_current = _da->book_keep.alloc_count_current;
	info.alloc_count_total   = _da->book_keep.alloc_count_total;
	info.max_memory          = _da->memsize;
	info.used_memory         = _da->book_keep.used_memory;
	info.free_memory         = info.max_memory - info.used_memory;

    #ifndef DYNAMICALLOCATOR_NO_THREAD_SAFETY
    _da->is_in_use = 0;
    #endif /*DYNAMICALLOCATOR_NO_THREAD_SAFETY*/

	return info;
}

DYNAMICALLOCATOR_API
void*
dynAllocator_first_fit(dynAllocator_s* _da, uint32_t _size)
/**
 * dynAllocator_first_fit() - create entry at first fit.
 * @arg1: Ptr to dynamic allocator.
 * @arg2: size of entry to create.
 *
 * Creates a chain entry. Follows fit methodology of "first fit".
 * Finds first fitting entry location from index zero.
 *
 * Order of fit search:             Big'Oh:
 * - First element in chain         - O(1)
 * - Before first element in chain  - O(1)
 * - Between 2 elements in chain    - O(N)
 * - As last element in chain       - O(N)
*
 * Return: NULL if entry cannot be created, created entry data ptr on clean 
 *         exit.
 */
{
	uint8_t* fit = NULL;
	uint8_t* next = NULL;
	uint8_t* next_header_start = NULL;
	uint8_t* prev = NULL;
	uint8_t* prev_data_end = NULL;
	uint32_t total_size = _size + sizeof(_dynAllocator_dataheader_s);
	if (total_size > _da->memsize) {
		DYNAMICALLOCATOR_ASSERT("CANNOT FIT ALLOCATION");
		return NULL;
	}
	/* Allocation fit as first element in chain
	 * */
	if (_da->first == NULL) {
		fit = _dynAllocator_entry_create(_da->memory, NULL, _size, _da->alignment);
		_da->first = fit;
		_da->last = _da->first;
		return fit;
	}
	/* Allocation fit before first element in chain
	 * */
	if (total_size < _da->first - _da->memory) {
		fit = _dynAllocator_entry_create(_da->memory, _da->first, _size, _da->alignment);
		_da->first = fit;
		return fit;
	}
	/* Allocation between element ptrs in chain
	 * */
	next = _da->first;
	while (next != NULL) {
		prev = next;
		next = _dynAllocator_data2header(next)->next;
		prev_data_end = prev + _dynAllocator_data2header(prev)->size;
		next_header_start = (uint8_t*)_dynAllocator_data2header(next);
		if (next_header_start - prev_data_end >= total_size) {
			fit = _dynAllocator_entry_create(prev_data_end, next, _size, _da->alignment);
			_dynAllocator_data2header(prev)->next = fit;
			return fit;
		}
	}
	/* Allocation as last element in chain
	 * */
	if (prev_data_end + total_size <= _da->memory + _da->memsize) {
		fit = _dynAllocator_entry_create(prev_data_end, NULL, _size, _da->alignment);
		_dynAllocator_data2header(prev)->next = fit;
		_da->last = fit;
		return fit;
	}
	DYNAMICALLOCATOR_ASSERT("ALLOCATION WAS NOT POSSIBLE");
	return NULL;
}

DYNAMICALLOCATOR_API
void*
dynAllocator_secure_free(dynAllocator_s* _da, void* _ptr)
/**
 * dynAllocator_secure_free() - remove entry.
 * @arg1: Ptr to dynamic allocator.
 * @arg2: Ptr to entry.
 *
 * Removes a chain entry.
 * Decrements internal allocation counter.
 *
 * Return: NULL.
 */
{
	void* prev = NULL;
	void* entry = NULL;
	if (_da == NULL || _ptr == NULL)
		return NULL;
	if ((uint8_t*)_ptr < _da->memory || (uint8_t*)_ptr > _da->memory + _da->memsize)
		return NULL;

	entry = _dynAllocator_find_entry(_da, _ptr, &prev);

	/*entry was not part of chain*/
	if (prev == NULL && entry == NULL)
		return NULL;

	#ifndef DYNAMICALLOCATOR_NO_BOOK_KEEPING
	_da->book_keep.alloc_count_current--;
	_da->book_keep.used_memory -= _dynAllocator_data2header(entry)->size;
	#endif /*DYNAMICALLOCATOR_NO_BOOK_KEEPING*/

	/*entry is first in chain*/
	if (prev == NULL && entry == _da->first) {
		_da->first = _dynAllocator_data2header(_da->first)->next; 
		if (_da->last == _da->first)
			_da->last = NULL;
		return NULL;
	}

	/*Remove entry from chain*/
	_dynAllocator_data2header(prev)->next = _dynAllocator_data2header(entry)->next;
	return NULL;
}

#ifdef DYNAMICALLOCATOR_DEBUG

#define _dynAllocator_is_aligned(POINTER, BYTE_COUNT) \
    ( ((unsigned long)(POINTER)) % (BYTE_COUNT) == 0 )

    //(((uintptr_t)(const void *)(POINTER)) % (BYTE_COUNT-1) == 0)

DYNAMICALLOCATOR_API
void
dynAllocator_debug(dynAllocator_s* _da, FILE* _out)
/**
 * dynAllocator_debug_status() - debug dynamic allocator.
 * @arg1: Ptr to dynamic allocator.
 * @arg2: output file for debugging.
 */
{
    int i = 0;
	uint8_t* p = NULL;
	_dynAllocator_dataheader_s* ph = NULL;
	if (_da == NULL || _out == NULL)
		return;

	fprintf(_out,"(memsize=%d, start=%p, aloc_curr=%d, aloc_tot=%d)\n",
	              _da->memsize, _da->memory,
	              _da->book_keep.alloc_count_current,
                  _da->book_keep.alloc_count_total);

	p = _da->first;
	ph = _dynAllocator_data2header(p);
	while (p != NULL) {
       
		fprintf(_out, "\t[%d] (size=%d, aligned=%s%s, S=%ld, E=%ld, N=%ld)\n",
               i,
		       ph->size,
		       (_dynAllocator_is_aligned(ph, _da->alignment)) ? "Y": "N",
		       (_dynAllocator_is_aligned(p, _da->alignment)) ? "Y": "N",
		       (uint8_t*)ph - _da->memory,
		       (uint8_t*)p + ph->size - _da->memory,
		       (uint8_t*)_dynAllocator_data2header(ph->next) - _da->memory);
		p = ph->next;
		ph = _dynAllocator_data2header(p);
        i++;
	}
}

#endif /*DYNAMICALLOCATOR_DEBUG*/

#endif /*DYNAMICALLOCATOR_IMPLEMENTATION*/
#endif /*DYNAMICALLOCATOR_H*/

#ifdef __cplusplus
}
#endif
