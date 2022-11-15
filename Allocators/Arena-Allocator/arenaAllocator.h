/*
# ARENA ALLOCATOR
        Simple to use Arena Allocator written in ansi-c99.

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
- [1.0] All core functionality implemented.
- [0.2] Implemented Alignment & mutex compatability.
        Implemented Scratch Buffer.
- [0.1] Implemented Arena Allocator core functionality.
        Wrote function descriptions.
        Rewrote function descriptions.
- [0.0] Initialized library.

## DOCUMENTATION

*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ARENAALLOCATOR_H
#define ARENAALLOCATOR_H

#ifndef ARENAALLOCATOR_API 
#define ARENAALLOCATOR_API static inline
#endif /*ARENAALLOCATOR_API */

#include <stdint.h>

#ifndef NULL
#define NULL (void*)0
#endif /*NULL*/

/*Conversion Macros*/
#define ARENAALLOCATOR_KB_2_B(N) ((N)*1024)
#define ARENAALLOCATOR_MB_2_B(N) ((N)*1024*1024)

/*Code Breakage managmenet*/
#ifdef ARENAALLOCATOR_NO_ASSERT
#define ARENAALLOCATOR_BREAK(expr, ret) ((expr) ? return ret : 0 )
#define ARENAALLOCATOR_BREAK_VOID(expr) ((expr) ? return )
#else
#include <assert.h>
#define ARENAALLOCATOR_BREAK(expr, ret) ( (void)ret, assert(expr) )
#define ARENAALLOCATOR_BREAK_VOID(expr) ( assert(expr) )
#endif /*ARENAALLOCATOR_NO_BREAK*/

/*Alignment Macros*/
#ifndef ARENAALLOCATOR_NO_ALIGNMENT
#define ARENAALLOCATOR_ALIGN(PTR, ALIGN) \
	( ((uintptr_t)(const long*)(PTR)+(ALIGN)-1) & ~((ALIGN)-1) )
#define ARENAALLOCATOR_IS_ALIGNED(PTR, ALIGN) ( ((PTR) & ((ALIGN)-1)) == 0 )
#define ARENAALLOCATOR_DEFAULT_ALIGNMENT 16
#else
#define ARENAALLOCATOR_ALIGN(PTR, ALIGN) (PTR)
#define ARENAALLOCATOR_IS_ALIGNED(PTR, ALIGN) (1)
#define ARENAALLOCATOR_DEFAULT_ALIGNMENT 0
#endif /*ARENAALLOCATOR_NO_ALIGNMENT*/

/*Mutex Macros*/
#ifndef ARENAALLOCATOR_NO_MUTEX
#define ARENAALLOCATOR_MUTEX_TAKE(mutex) ( mutex = 1 )
#define ARENAALLOCATOR_MUTEX_GIVE(mutex) ( mutex = 0 )
#define ARENAALLOCATOR_MUTEX_IS_TAKEN(mutex) ( (mutex) ? 1:0 )
#define ARENAALLOCATOR_MUTEX_WAIT_THEN_TAKE(mutex) \
	do {do {} while (ARENAALLOCATOR_MUTEX_IS_TAKEN(mutex)); \
		ARENAALLOCATOR_MUTEX_TAKE(mutex); } while (0)
#else 
#define ARENAALLOCATOR_MUTEX_TAKE(mutex)
#define ARENAALLOCATOR_MUTEX_GIVE(mutex)
#define ARENAALLOCATOR_MUTEX_IS_TAKEN(mutex) ( 0 )
#define ARENAALLOCATOR_MUTEX_WAIT_THEN_TAKE(mutex)
#endif /*ARENAALLOCATOR_NO_MUTEX*/

typedef struct {
    uint8_t* memory;
	uint32_t offset;
    uint32_t max_size;

    /*Failsafe buffer upon arena being filled
      Only guaranteed to be failsafe if scratch buffer ptr is valid,
	  and size of scratch buffer is larger than max possible allocation.
     */
    uint8_t* scratch_buffer;

    /*Mutex for threading*/
    uint8_t is_in_use;
    /*Memory alignment for data lookup*/
	uint8_t alignment;
}arenaAllocator_s;

ARENAALLOCATOR_API
char
arenaAllocator_setup(arenaAllocator_s* _aa,
					 uint8_t*          _mem,
					 uint32_t          _memsize,
					 uint8_t           _alignment,
					 uint8_t*          _scratch);

ARENAALLOCATOR_API
void*
arenaAllocator_alloc(arenaAllocator_s* _aa, uint32_t _size);

ARENAALLOCATOR_API
void
arenaAllocator_free_all(arenaAllocator_s* _aa);

/******************************************************************************/
#define ARENAALLOCATOR_IMPLEMENTATION
#ifdef ARENAALLOCATOR_IMPLEMENTATION

ARENAALLOCATOR_API
char
arenaAllocator_setup(arenaAllocator_s* _aa,
					 uint8_t*          _mem,
					 uint32_t          _memsize,
					 uint8_t           _alignment,
					 uint8_t*          _scratch)
/**
 * @brief Create arena allocator without automatic allocation of memory.
 *
 * @param Ptr to arena allocator to setup.
 * @param Ptr to memory to be used for arena.
 * @param Size of memory used for arena.
 * @param Alignment Requirement for memory stored.
 * @param Ptr to scratch buffer.
 *
 * Size of scratch buffer is not required to be known, it is exprected that
 * scratch buffer is large enough to contain the maximum size of allocation
 * required. Furthermore, if no scratch buffer is wanted, provide NULL.
 *
 * Alignment is not required, specify 0 or 1 to omit alignment. If specified,
 * make sure alignment [n] is a number in the set [2^n], eg [1, 2, 4, 8, 16].
 * Undefined behaviour is ensured if this is not satisfied.
 *
 * @return -1 on error, 0 on clean exit.
 */
{
	ARENAALLOCATOR_BREAK((_aa != NULL && _mem != NULL && _memsize > 1) &&
						 "Input was invalid",
						 -1);

    _aa->memory = _mem;
    _aa->offset = 0;
    _aa->max_size = _memsize;
	if (_alignment < 1) _alignment = 1;
    _aa->alignment = _alignment;
    _aa->scratch_buffer = _scratch;
    _aa->is_in_use = 0;
    return 0;
}

ARENAALLOCATOR_API
void*
arenaAllocator_alloc(arenaAllocator_s* _aa, uint32_t _size)
/**
 * @brief Allocate memory block from arena.
 *
 * @param Ptr to arena allocator.
 * @param Size of requested memory
 *
 * @return if allocation was possible: ptr to allocated block.
 *         NULL otherwise.
 */
{
    void* out = NULL;
	ARENAALLOCATOR_BREAK(_aa != NULL && "Input is invalid", NULL);

	ARENAALLOCATOR_MUTEX_WAIT_THEN_TAKE(_aa->is_in_use);

    if ((_aa->offset + _size) < _aa->max_size) {
		/*Provide slice of arena*/
		out = (void*)ARENAALLOCATOR_ALIGN(_aa->memory + _aa->offset, _aa->alignment);
		_aa->offset = (uint8_t*)out - _aa->memory + _size;
	} else {
		/*Provide scratch buffer on arena being full*/
        out = _aa->scratch_buffer;
	}

    ARENAALLOCATOR_MUTEX_GIVE(_aa->is_in_use);
    return out;
}

ARENAALLOCATOR_API
void
arenaAllocator_free_all(arenaAllocator_s* _aa)
/**
 * @brief Free all allocated memory from arena 
 *
 * @param Ptr to arena allocator.
 *
 * @return void.
 */
{
	ARENAALLOCATOR_BREAK_VOID(_aa != NULL && "Input is invalid");

	ARENAALLOCATOR_MUTEX_WAIT_THEN_TAKE(_aa->is_in_use);
    _aa->offset = 0;
    ARENAALLOCATOR_MUTEX_GIVE(_aa->is_in_use);
}

#endif /*ARENAALLOCATOR_IMPLEMENTATION*/
#endif /*ARENAALLOCATOR_H*/

#ifdef __cplusplus
}
#endif
