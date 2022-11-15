#include <stdio.h>
#include <assert.h>

#define ARENAALLOCATOR_IMPLEMENTATION
#define ARENAALLOCATOR_NO_ALIGNMENT
//#define ARENAALLOCATOR_NO_MUTEX
#include "arenaAllocator.h"

#include "testlib.h"

#define memsize 4096
#define scratch_size 5000

typedef struct {
	int x;
	int y;
}pos;

void
pos_print(pos* _pos)
{
    if (_pos == NULL)
	    printf("pos = NULL\n");
    else 
	    printf("pos (%d,%d) at ptr(%p)\n", _pos->x, _pos->y, _pos);
}

void
test_custom_struct(void)
{
    uint8_t memory[memsize] = {0};
	arenaAllocator_s arena = {0};
    uint8_t alignment = 16;
	uint8_t* scratch = NULL;
	arenaAllocator_setup(&arena, memory, memsize, alignment, scratch);

    pos* positions[8] = {0};
    int i;
    uint32_t prev_offset = arena.offset;

    TL_COLOR(TL_BLUE);
    TL_PRINT("Allocation size = %ld\n", sizeof(pos));
    TL_PRINT("Alignment size = %d\n", alignment);

    for (i = 0; i < 8; i++) {
        positions[i] = (pos*)arenaAllocator_alloc(&arena, sizeof(pos));
        *positions[i] = (pos){i, i*2};
        TL_COLOR(TL_BLUE);
        TL_PRINT("Raw shift = %d, aligned shift = %ld\n",
                 (prev_offset),
                 (uint8_t*)positions[i]-memory);
        prev_offset = arena.offset;
    }
        
    for (i = 0; i < 8; i++) {
        TL_TEST(positions[i] != NULL);
        TL_TEST(positions[i]->x == i && positions[i]->y == i*2);
        pos_print(positions[i]);
    }
    arenaAllocator_free_all(&arena);
}

void test_scratch_buffer(void)
{
     uint8_t memory[memsize] = {0};
	arenaAllocator_s arena = {0};
    uint8_t alignment = 16;
	uint8_t scratch[scratch_size] = {0};
	arenaAllocator_setup(&arena, memory, memsize, alignment, scratch);

    const int alloc_count = 10000;
	const int alloc_size = 500;

    int total_alloc_size = 0;
    int invalid_allocs = 0;
    void* allocation = NULL;

    TL_COLOR(TL_BLUE);
    TL_PRINT("memory size = %d\n", memsize);
    TL_PRINT("scratch size = %d\n", scratch_size);
    TL_PRINT("Attempting %d allocations of size %d (being less than scratch buffer)\n",
			 alloc_count, alloc_size);

    int i;
    for (i = 0; i < alloc_count; i++) {
        allocation = arenaAllocator_alloc(&arena, alloc_size);
		if (allocation != NULL)
			total_alloc_size += alloc_size;
		else 
			invalid_allocs++;
    }
	TL_TESTM(invalid_allocs == 0,
			 "All allocations were successful because of scratch buffer!");

	TL_TEST(total_alloc_size > memsize);
    TL_COLOR(TL_BLUE);
    TL_PRINT("total allocation size = %d\n", total_alloc_size);
    arenaAllocator_free_all(&arena);
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;
    TL(test_custom_struct())
    TL(test_scratch_buffer())

    tl_summary();
	return 0;
}
