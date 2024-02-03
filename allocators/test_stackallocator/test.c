#include <stdio.h>
#include <assert.h>
#include "testlib.h"
#define TH_ALLOCATOR_IMPLEMENTATION
#include "../th_allocator.h"

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
    uint8_t scratch[scratch_size] = {0};
    uint8_t memory[memsize] = {0};
	stackallocator sa = {0};
	allocator_status status;
	status = stackalc_new(&sa, region(memory, memsize), scratch);

	printf("memory ptr = %p\n", memory);
	printf("scratch ptr = %p\n", scratch);
	TL_TEST(sa.region.mem = memory);
	TL_TEST(sa.scratch_buffer = scratch);
	TL_TEST(status == ALLOCATOR_OK);

    pos* positions[8] = {0};
    int i;
    uint32_t prev_offset = sa.offset;

    TL_COLOR(TL_BLUE);
    TL_PRINT("Allocation size = %ld\n", sizeof(pos));
    TL_PRINT("Alignment size = %d\n", _TH_ALIGNMENT);

    for (i = 0; i < 8; i++) {
        positions[i] = (pos*)stackalc_alloc(&sa, sizeof(pos));
		TL_TEST(positions[i] != NULL);
		positions[i]->x = i;
		positions[i]->y = i*2;
        TL_COLOR(TL_BLUE);
        TL_PRINT("Raw shift = %d, aligned shift = %ld\n",
                 (prev_offset),
                 (uint8_t*)positions[i]-memory);
        prev_offset = sa.offset;
    }
        
    for (i = 0; i < 8; i++) {
        TL_TEST(positions[i] != NULL && positions[i]->x == i && positions[i]->y == i*2);
        pos_print(positions[i]);
    }
    stackalc_reset(&sa);
}

void test_scratch_buffer(void)
{
    const int alloc_count = 10000;
	const int alloc_size = 500;

    uint8_t scratch[scratch_size] = {0};
    uint8_t memory[memsize] = {0};
	stackallocator sa = {0};
	stackalc_new(&sa, region(memory, memsize), scratch);

    void* allocation = NULL;
    int invalid_allocs = 0;
    int memory_allocs = 0;
    int scratch_allocs = 0;

    TL_COLOR(TL_BLUE);
    TL_PRINT("memory size = %d\n", memsize);
    TL_PRINT("scratch size = %d\n", scratch_size);
    TL_PRINT("Attempting %d allocations of size %d (being less than scratch buffer)\n",
			 alloc_count, alloc_size);
    TL_PRINT("total memory  to allocate = %d\n", alloc_count * alloc_size);

    int i;
    for (i = 0; i < alloc_count; i++) {
        allocation = stackalc_alloc(&sa, alloc_size);
		if (allocation == NULL)
			invalid_allocs++;
		else if (allocation == scratch)
			scratch_allocs++;
		else 
			memory_allocs++;
    }
	TL_TESTM(invalid_allocs == 0,
			 "All allocations were successful because of scratch buffer!");

    TL_COLOR(TL_BLUE);
    TL_PRINT("scratch allocation count = %d\n", scratch_allocs);
    TL_PRINT("memory allocation count  = %d\n", memory_allocs);
    stackalc_reset(&sa);
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;
    TL(test_custom_struct());
	TL(test_scratch_buffer());

    tl_summary();
	return 0;
}
