/*
# TH ALLOCATOR

Simple memory allocators written in ansi-c99.

## ORIGINAL AUTHOR

Thomas Alexgaard Jensen (![github/thom9258](https://github.com/thom9258))

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
- [0.0] Initialized library.
- [0.1] Ported seperated implementation into a single header file
        implementation.
        Redefined MEMALLOCATOR_ to TH_ALLOCATOR_


TODO: either do:
1. Pull scratch buffer out of basic-allocator, it does not need to exist in
all allocators.
2. Make base-allocator macro that tests the returned memory ptr for NULL and
it it is, return scratch_buffer instead. This way scratch is used automatically,
and it will still just return NULL if scratch is empty, thus no downsides.
something like:

_BASEALLOCATOR_RETURN_SCRATCH_ON_NULL(basealloc, ret_ptr) \
    ( (ret_ptr == NULL) ? basealloc->scratch_buffer : ret_ptr )

then just use this for all returns in allocators.

*/
#ifndef TH_ALLOCATOR_H
#define TH_ALLOCATOR_H

#ifndef TH_ALLOCATOR_API 
#define TH_ALLOCATOR_API static inline
#endif /*TH_ALLOCATOR_API */

#ifndef TH_ALLOCATOR_BACKEND 
#define TH_ALLOCATOR_BACKEND static
#endif /*TH_ALLOCATOR_BACKEND */

#include <stdint.h>

#ifndef NULL
#define NULL (0)
#endif /*NULL*/

/* *****************************************************************************
 * Common Allocator defs
 * ****************************************************************************/

#define TH_KB_2_B(N) ((N)*1024)
#define TH_MB_2_B(N) ((N)*1024*1024)

#ifndef _TH_ALIGNMENT
#define _TH_ALIGNMENT 16
#endif /*_TH_ALIGNMENT*/
#define _TH_ALIGN(IDX) \
	( ((IDX)+(_TH_ALIGNMENT)-1) & ~((_TH_ALIGNMENT)-1) )
#define _TH_ALIGNMENT_OFFSET(IDX) \
	( (((IDX)+(_TH_ALIGNMENT)-1) & ~((_TH_ALIGNMENT)-1)) - (IDX) )
#define _TH_IS_ALIGNED(IDX) \
	( ((IDX) & ((_TH_ALIGNMENT)-1)) == 0 )

/*Mutex Macros*/
typedef char _th_mutex;
#ifndef TH_DISABLE_MUTEX
#define _TH_MUTEX_TAKE(mutex) ( mutex = 1 )
#define _TH_MUTEX_GIVE(mutex) ( mutex = 0 )
#define _TH_MUTEX_IS_TAKEN(mutex) ( (mutex) ? 1:0 )
#define _TH_MUTEX_WAIT_THEN_TAKE(mutex) \
	do {do {} while (_TH_MUTEX_IS_TAKEN(mutex)); _TH_MUTEX_TAKE(mutex); \
	} while (0)
#else 
#define _TH_MUTEX_TAKE(mutex)
#define _TH_MUTEX_GIVE(mutex)
#define _TH_MUTEX_IS_TAKEN(mutex) ( 0 )
#define _TH_MUTEX_WAIT_THEN_TAKE(mutex)
#endif /*TH_NO_MUTEX*/

typedef enum {
	ALLOCATOR_OK = 0,
	ALLOCATOR_INVALID_INPUT,
	ALLOCATOR_INVALID_REGION,

	ALLOCATOR_STATUS_COUNT
}allocator_status;
	
typedef struct {
	uint32_t total;
	uint32_t curr;
}_allocation_bookkeeper;

#define _ALLOCATION_BOOKKEEPER_NEW(book) ( (book) = (_allocation_bookkeeper) {0,0} )
#define _ALLOCATION_BOOKKEEPER_CLEAR(book) ((book).curr = 0)
#define _ALLOCATION_BOOKKEEPER_INCREMENT(book) ( (book).curr++, (book).total++ )
#define _ALLOCATION_BOOKKEEPER_DECREMENT(book) ( (book).curr--, (book).total++ )

/* *****************************************************************************
 * Memory Region
 * ****************************************************************************/

typedef struct {
    uint8_t* mem;
    uint32_t len;
    uint8_t freeable;
}memregion;

TH_ALLOCATOR_API
memregion region(void* _mem, uint32_t _len);

TH_ALLOCATOR_API
char region_ok(memregion* _r);

/* *****************************************************************************
 * Stack Allocator
 * ****************************************************************************/

typedef struct {
	memregion region;
    void* scratch_buffer;
	_allocation_bookkeeper bookkeeper;
	uint32_t offset;
}stackallocator;

TH_ALLOCATOR_API
allocator_status stackalc_new(stackallocator* _a, memregion _mem, void* _scratch);

TH_ALLOCATOR_API
void* stackalc_alloc(stackallocator* _a, uint32_t _n);

TH_ALLOCATOR_API
void stackalc_reset(stackallocator* _a);

/* *****************************************************************************
 * Dynamic Allocator
 * ****************************************************************************/

typedef struct {
	uint8_t* next;
	uint32_t size;
}_dynalc_header;
 
#define _TH_ALLOCATOR_DATA2HEADER(data) \
	(((_dynalc_header*)(data)) - 1)

#define _TH_ALLOCATOR_HEADER2DATA(header) \
    ((uint8_t*)(header)) + sizeof(_dynalc_header)

typedef struct {
    memregion region;
    _allocation_bookkeeper bookkeeper;
    _th_mutex is_in_use;
    uint8_t* first;
    uint8_t* last;
}dynallocator;

TH_ALLOCATOR_API
allocator_status dynalc_init(dynallocator* _da, memregion _region);

TH_ALLOCATOR_API
void* dynalc_malloc(dynallocator* _da, int _size);

TH_ALLOCATOR_API
void* dynalc_free(dynallocator* _da, void* _ptr);

TH_ALLOCATOR_API
allocator_status dynalc_free_all(dynallocator* _da);

/* *****************************************************************************
 * Block Allocator
 * ****************************************************************************/

typedef struct {
	memregion region;
	_allocation_bookkeeper bookkeeper;
    _th_mutex is_in_use;
	uint32_t block_size;
	uint32_t block_count;
}blkallocator;

typedef struct {
    blkallocator* blkalc;
    uint32_t blockid;
}blk_handle;

#define BLKHANDLE_INVALID (blk_handle) {NULL, 0}

#define BLKHANDLE_ACCESS(handle, type) \
	((type*)blkalc_ptr(handle))

#define BLKALC_OPTIMAL_MEMSIZE(n_blocks, block_size)                    \
	( ((n_blocks) * (_TH_ALIGN(block_size))) + ((sizeof(char) * (n_blocks))) )

TH_ALLOCATOR_API
allocator_status blkalc_init(blkallocator* _ba, uint8_t* _memory, uint32_t _block_size, uint32_t _block_count);

TH_ALLOCATOR_API
blk_handle blkalc_take(blkallocator* _ba);

TH_ALLOCATOR_API
uint32_t blkalc_n_available(blkallocator* _ba);

TH_ALLOCATOR_API
void blk_handle_return(blk_handle _handle);

TH_ALLOCATOR_API
void* blkalc_ptr(blk_handle _handle);


/* *****************************************************************************
 * Implenentation - define before include:
 * #define TH_ALLOCATOR_IMPLEMENTATION
 * #include "th_allocator.h"
 * ****************************************************************************/

#define TH_ALLOCATOR_IMPLEMENTATION
#ifdef TH_ALLOCATOR_IMPLEMENTATION

TH_ALLOCATOR_API
memregion
region(void* _mem, uint32_t _len)
{
    return (memregion){(uint8_t*)_mem, _len, 0};
}

TH_ALLOCATOR_API
char
region_ok(memregion* _r)
{
    if (_r->mem != NULL && _r->len > 0)
        return 1;
    return 0;
}

TH_ALLOCATOR_API
allocator_status
stackalc_new(stackallocator* _a, memregion _mem, void* _scratch)
/**
 * @brief Create stack allocator from provided memory.
 * @param Ptr to stack allocator.
 * @param memory region.
 * @param scratch buffer.
 * @return error code on error, TH_OK on clean exit.
 */
{
    if (_a == NULL || !region_ok(&_mem))
        return ALLOCATOR_INVALID_INPUT;
    _a->scratch_buffer = _scratch;
    _a->region = _mem;
	_ALLOCATION_BOOKKEEPER_NEW(_a->bookkeeper);
    return ALLOCATOR_OK;
}

TH_ALLOCATOR_API
void*
stackalc_alloc(stackallocator* _a, uint32_t _n)
/**
 * @brief Allocate block from stack.
 *
 * @param Ptr to stack allocator.
 * @param Size of requested memory.
 * @return if allocation was possible: ptr to allocated block.
 *         NULL otherwise.
 */
{
    uint8_t* out = NULL;

    if (_a == NULL)
        return NULL;

    if ((_a->offset + _n) < _a->region.len) {
		/*Provide a block of aligned memory*/
		out = (uint8_t*)_TH_ALIGN((unsigned long)_a->region.mem +
								  _a->offset);
		_a->offset = out - _a->region.mem + _n;
	} else {
		/*Provide scratch buffer on arena being full*/
        out = (uint8_t*)_a->scratch_buffer;
	}
	if (out != NULL)
	   _ALLOCATION_BOOKKEEPER_INCREMENT(_a->bookkeeper);
	return out;
}

TH_ALLOCATOR_API
void
stackalc_reset(stackallocator* _a)
/**
 * @brief Free all allocated memory from arena 
 * @param Ptr to stack allocator.
 */
{
    if (_a == NULL)
        return;
    _a->offset = 0;
}

TH_ALLOCATOR_API
allocator_status
dynalc_init(dynallocator* _da, memregion _region)
/**
 * dynAllocator_set() - Initialize Dynamic Allocator without allocation.
 * @arg1: Ptr to allocator to initialize.
 * @arg2: Size of memory pool for initialized allocator.
 * @arg3: Pointer to existing memory.
 *
 * Initialize dynamic allocator using specified memory field.
 *
 * Return: error code on error, TH_OK on clean exit.
 */
{
	if (_da == NULL || !region_ok(&_region))
		return ALLOCATOR_INVALID_INPUT;
    _da->region = _region;
    _ALLOCATION_BOOKKEEPER_NEW(_da->bookkeeper);
    _da->first = NULL;
    _da->last = NULL;
    return ALLOCATOR_OK;
}

TH_ALLOCATOR_BACKEND
uint8_t*
_dynalc_entry_create(uint8_t* _start, uint8_t* _next, uint32_t _size)
/**
 * _th_allocator_entry_create() - create chain entry at specified location.
 * @arg1: Ptr to start entry.
 * @arg2: Ptr to next entry in chain.
 * @arg3: Size of entry data.
 *
 * Creates an aligned chain entry at specified location.
 * If entry is last in chain, specify NULL at @arg2.
 *
 * Return: NULL on error, entry data ptr on clean exit.
 */
{
	_dynalc_header* hptr = NULL;
	if (_start == NULL || _size < 1)
		return NULL;

	hptr = (_dynalc_header*)_TH_ALIGN((unsigned long)_start);
	hptr->next = _next;
	hptr->size = _size;
	return _TH_ALLOCATOR_HEADER2DATA(hptr);
}

TH_ALLOCATOR_API
void*
dynalc_malloc(dynallocator* _da, int _size)
/**
 * dynAllocator_malloc() - create entry.
 * @arg1: Ptr to dynamic allocator.
 * @arg2: size of entry to create.
 *
 * Creates an allocation. Follows fit methodology of "first fit".
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
	uint32_t total_size = _size + sizeof(_dynalc_header);

	if (_da == NULL || _size < 1 ||
        total_size > _da->region.len)
		return NULL;

    _TH_MUTEX_WAIT_THEN_TAKE(_da->is_in_use);
	/* Allocation fit as first element in chain
	 * */
	if (_da->first == NULL) {
		fit = _dynalc_entry_create(_da->region.mem, NULL, _size);
		_da->first = fit;
		_da->last = _da->first;
        goto FOUND_FIT;
	}
	/* Allocation fit before first element in chain
	 * */
	if (total_size < _da->first - _da->region.mem) {
		fit = _dynalc_entry_create(_da->region.mem, _da->first, _size);
		_da->first = fit;
        goto FOUND_FIT;
	}
	/* Allocation between element ptrs in chain
	 * */
	next = _da->first;
	while (next != NULL) {
		prev = next;
		next = _TH_ALLOCATOR_DATA2HEADER(next)->next;
		prev_data_end = prev + _TH_ALLOCATOR_DATA2HEADER(prev)->size;
		next_header_start = (uint8_t*)_TH_ALLOCATOR_DATA2HEADER(next);
		if (next_header_start - prev_data_end >= total_size) {
			fit = _dynalc_entry_create(prev_data_end, next, _size);
			_TH_ALLOCATOR_DATA2HEADER(prev)->next = fit;
            goto FOUND_FIT;
		}
	}
	/* Allocation as last element in chain
	 * */
	if (prev_data_end + total_size <= _da->region.mem + _da->region.len) {
		fit = _dynalc_entry_create(prev_data_end, NULL, _size);
		_TH_ALLOCATOR_DATA2HEADER(prev)->next = fit;
		_da->last = fit;
        goto FOUND_FIT;
	}

FOUND_FIT:
    _ALLOCATION_BOOKKEEPER_INCREMENT(_da->bookkeeper);
    _TH_MUTEX_GIVE(_da->is_in_use);
    return fit;
}

TH_ALLOCATOR_BACKEND
void*
_dynalc_find_entry(dynallocator* _da, void* _ptr, void** _prev_entry)
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
		if (_ptr == _TH_ALLOCATOR_DATA2HEADER(prev)->next) {
			if (_prev_entry != NULL)
				*_prev_entry = prev;
			return _TH_ALLOCATOR_DATA2HEADER(prev)->next;
		}
		prev = _TH_ALLOCATOR_DATA2HEADER(prev)->next;
	}
	if (_prev_entry != NULL)
		*_prev_entry = NULL;
	return NULL;
}

TH_ALLOCATOR_API
void*
dynalc_free(dynallocator* _da, void* _ptr)
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
    void* prev = NULL;
	void* entry = NULL;

    _TH_MUTEX_WAIT_THEN_TAKE(_da->is_in_use);

	if (_da == NULL || _ptr == NULL            ||
        (uint8_t*)_ptr < _da->region.mem ||
        (uint8_t*)_ptr > _da->region.mem + _da->region.len)
        goto HAS_FREED;


	entry = _dynalc_find_entry(_da, _ptr, &prev);
	/*Check if entry is part of chain*/
	if (prev == NULL && entry == NULL)
        goto HAS_FREED;

    /*Remove found entry from allocator
     */
    _ALLOCATION_BOOKKEEPER_DECREMENT(_da->bookkeeper);
	if (prev == NULL && entry == _da->first) {
        /*entry is first in chain*/
		_da->first = _TH_ALLOCATOR_DATA2HEADER(_da->first)->next; 
		if (_da->last == _da->first)
			_da->last = NULL;
        goto HAS_FREED;
	}
	/*Remove entry from inside chain*/
	_TH_ALLOCATOR_DATA2HEADER(prev)->next = _TH_ALLOCATOR_DATA2HEADER(entry)->next;


HAS_FREED:
    _TH_MUTEX_GIVE(_da->is_in_use);
    return NULL;
}

TH_ALLOCATOR_API
allocator_status
dynalc_free_all(dynallocator* _da)
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
		return ALLOCATOR_INVALID_INPUT;

	/*TODO: Might cause problems as the internal header->next pointers are the
	 *      untouched. Solution: iterate though entries, and unlink manually.*/
    _TH_MUTEX_WAIT_THEN_TAKE(_da->is_in_use);
	_da->first = NULL;
	_da->last = NULL;
    _ALLOCATION_BOOKKEEPER_CLEAR(_da->bookkeeper);

    _TH_MUTEX_GIVE(_da->is_in_use);
	return ALLOCATOR_OK;
}

TH_ALLOCATOR_API
allocator_status
th_blockallocator_init(blkallocator* _ba,
                       uint8_t* _memory,
                       uint32_t _block_size,
                       uint32_t _block_count)
/**
 * th_BlockAlloc_init() - Setuo block allocator with provided  memory.
 *
 * @arg1: Ptr to block allocator.
 * @arg2: Ptr to memory for allocator.
 * @arg3: size of single block specified in bytes.
 * @arg4: amount blocks to be allocated.
 *
 * Creates blockallocator with provided memory and block definitions.
 *
 * Warning: It is expected that the provided block size is aligned according
 *          to _TH_ALIGN(), and that the provided memory is largar than, or
 *          equal to TH_BLOCKALLOC_OPTIMAL_MEMSIZE().
 *
 * Return: error management code, TH_INVALID_INPUT for error, TH_OK for clean
 *         exit.
 */
{
	if (_ba == NULL || _memory == NULL ||
        _block_size < 1 || _block_count < 1 ||
        _block_size != _TH_ALIGN(_block_size))
		return ALLOCATOR_INVALID_INPUT;

    _TH_MUTEX_WAIT_THEN_TAKE(_ba->is_in_use);

	_ba->block_size = _TH_ALIGN(_block_size);
	_ba->block_count = _block_count;
    _ALLOCATION_BOOKKEEPER_NEW(_ba->bookkeeper);
    /*NOTE: We set memlen in macro to 0 because we wont use it..*/
    _ba->region = region(_memory, _block_size * _block_count);
    _TH_MUTEX_GIVE(_ba->is_in_use);
    return ALLOCATOR_OK;
}

TH_ALLOCATOR_API
blk_handle
th_blockallocator_take(blkallocator* _ba)
/**
 * th_blockallocator_take() - Take a block from block allocator.
 *
 * @arg1: Ptr to block allocator.
 *
 * Return: Taken block index + 1, BLKALLOC_INVALID_BLOCK on invalid block.
 */
{
	uint32_t i;
	if (_ba == NULL)
		return BLKHANDLE_INVALID;

    _TH_MUTEX_WAIT_THEN_TAKE(_ba->is_in_use);
	for (i = 0; i < _ba->block_count; i++) {
		if (_ba->region.mem[i] == 0) {
			_ba->region.mem[i] = 1;
            _ALLOCATION_BOOKKEEPER_INCREMENT(_ba->bookkeeper);
            _TH_MUTEX_GIVE(_ba->is_in_use);
			return (blk_handle) {_ba, i};
		}
	}
    _TH_MUTEX_GIVE(_ba->is_in_use);
	return BLKHANDLE_INVALID;
}

TH_ALLOCATOR_API
void
blk_handle_return(blk_handle _handle)
/**
 * BLKalloc_return() - free valid block from memory.
 * @arg1: Ptr to block allocator.
 * @arg2: specified block to return.
 */
{
	if (_handle.blkalc == NULL ||
        _handle.blockid < 1 ||
        _handle.blockid > _handle.blkalc->block_count)
		return;

    _TH_MUTEX_WAIT_THEN_TAKE(_handle.blkalc->is_in_use);
	if (_handle.blkalc->region.mem[_handle.blockid] != 0) {
		_handle.blkalc->region.mem[_handle.blockid] = 0;
        _ALLOCATION_BOOKKEEPER_DECREMENT(_handle.blkalc->bookkeeper);
	}
    _TH_MUTEX_GIVE(_handle.blkalc->is_in_use);
}


TH_ALLOCATOR_API
uint32_t
th_blockallocator_n_available(blkallocator* _ba)
/**
 * BLKalloc_n_available() - get available blocks.
 *
 * @arg1: Ptr to block allocator.
 *
 * Return: amount of available blocks.
 */
{
    uint32_t n = 0;
    _TH_MUTEX_WAIT_THEN_TAKE(_ba->is_in_use);
	if (_ba == NULL)
        n = 0;
    else
        n = _ba->block_count - _ba->bookkeeper.curr;
    _TH_MUTEX_GIVE(_ba->is_in_use);
	return n;
}

TH_ALLOCATOR_API
void*
blkalc_ptr(blk_handle _handle)
/**
 * th_blockallocator_ptr() - convert valid block to memory ptr.
 *
 * @arg1: Ptr to block allocator.
 * @arg2: specified handle to block for conversion.
 *
 * Warning: Do not save returned ptr if you know block memory 
 *          is liable to be moved, serialized, etc.
 *
 * Return: Taken block ptr, NULL on invalid block.
 */
{
	uint32_t offset = 0;
	if (_handle.blkalc == NULL ||
        _handle.blockid < 1 ||
        _handle.blockid > _handle.blkalc->block_count)
		return NULL;
    blkallocator* ba = _handle.blkalc;

    _TH_MUTEX_WAIT_THEN_TAKE(_handle.blkalc->is_in_use);
	if (_handle.blkalc->region.mem[_handle.blockid] == 0)
		return NULL;
	offset = ba->block_count + (ba->block_size * (_handle.blockid));
    _TH_MUTEX_GIVE(ba->is_in_use);
	return (void*)_TH_ALIGN((unsigned long)(ba->region.mem + offset));
}

#endif /*TH_ALLOCATOR_IMPLEMENTATION*/
#endif /*TH_ALLOCATOR_H*/
