#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#ifndef YAL_ALLOC
#define YAL_ALLOC

#define REGIONLEN 4096

typedef struct {
    void* mem;
    size_t len;
}memRegion;

memRegion
memRegion_new(void)
{
    memRegion res = (memRegion) {0};
    res.mem = malloc(REGIONLEN);
    res.len = REGIONLEN;
    return res;
}

void
memRegion_destroy(memRegion* _m)
{
    assert(_m != NULL);
    if (_m->mem != NULL) free(_m->mem);
    _m->mem = NULL;
    _m->len = 0;
}

#define t_type void*
#define arr_t_name ptrStack
#include "tsarray.h"

/*sb - stack block*/
typedef struct {
    memRegion region;
    size_t member_memlen;
    unsigned int member_count;
    ptrStack stack;
}sbAllocator;

sbAllocator*
sbAllocator_init(sbAllocator* _a, size_t _blksize)
{
    assert(_a != NULL);

    _a->region = memRegion_new();
    _a->member_memlen = _blksize;
    _a->member_count = _a->region.len / _blksize;
    // printf("sbAllocator can hold %d members of size %ld\n",
    //     _a->member_count,
    //     _a->member_memlen);

    /*Setup pointer stack for big oh 1 allocation*/
    ptrStack_initn(&_a->stack, _a->member_count);
    unsigned int i;
    void* p = NULL;
    for (i = 0; i < _a->member_count; i++) {
        p = (void*)((uint8_t*)_a->region.mem + (i * _a->member_memlen));
        ptrStack_push(&_a->stack, p);
    }
    return _a;
}

void
sbAllocator_destroy(sbAllocator* _a)
{
    assert(_a != NULL);
    memRegion_destroy(&_a->region);
    ptrStack_destroy(&_a->stack);
    *_a = (sbAllocator) {0};
}

void*
sbAllocator_get(sbAllocator* _a)
{
    assert(_a != NULL);
    assert(ptrStack_len(&_a->stack) > 0 && "Could not allocate more expr's\n");
    /*TODO: Make a new allocator and throw its ptrs into stack*/
    return ptrStack_pop(&_a->stack);
}

void
sbAllocator_return(sbAllocator* _a, void* _p)
{
    assert(_a != NULL);
    ptrStack_push(&_a->stack, _p);
}

#endif /*YAL_ALLOC*/
