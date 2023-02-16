#include <stdio.h>
#include <assert.h>

#include "testlib.h"

#define TH_ALLOCATOR_IMPLEMENTATION
//#define MEMALLOCATOR_NO_ASSERT
#include "../th_allocator.h"

typedef struct {
	int x;
	int y;
}pos;

void
pos_print(pos* _pos)
{
	printf("pos (%d,%d) at ptr(%p)\n", _pos->x, _pos->y, _pos);
}

void
test_custom_struct(void)
{
    uint8_t mem[TH_BLOCKALLOCATOR_OPTIMAL_MEMSIZE(20, sizeof(pos))];

	TL_PRINT("memsize = %ld\n", TH_BLOCKALLOCATOR_OPTIMAL_MEMSIZE(20, sizeof(pos)));

	th_blockallocator pos_alloc = {0};

	TL_TEST(th_blockallocator_init(&pos_alloc, mem, sizeof(pos), 20) == TH_INVALID_INPUT);
	TL_TEST(th_blockallocator_init(&pos_alloc, mem, _TH_ALIGN(sizeof(pos)), 20) == TH_OK);
	TL_TEST(pos_alloc.allocator.memory == mem);

	printf("total blocks = (%d)\n", pos_alloc.block_count);
	printf("free blocks = (%d)\n", th_blockallocator_n_available(&pos_alloc));

	th_block pos1 = th_blockallocator_take(&pos_alloc);
	TL_PRINT("pos1 = %d\n", pos1);
	TL_TEST(pos1 == 1);
	th_block pos2 = th_blockallocator_take(&pos_alloc);
	TL_PRINT("pos2 = %d\n", pos2);
	TL_TEST(pos2 == 2);

	TL_TEST(th_blockallocator_n_available(&pos_alloc) == 18);

	pos* pos2_ptr = (pos*)th_blockallocator_ptr(&pos_alloc, pos2);
	TL_TEST(pos2_ptr != NULL);
	TL_TEST(_TH_IS_ALIGNED((unsigned long)pos2_ptr) == 1);
	pos2_ptr->x = 2;
	pos2_ptr->y = 0;
	pos_print(pos2_ptr);

	/*Can access block ptr by access macro*/
	TL_TEST(TH_BLOCK_ACCESS(&pos_alloc, pos1, pos) != NULL);

	TH_BLOCK_ACCESS(&pos_alloc, pos1, pos)->x = 5;

	/*Can access block ptr either by direct pointer conversion*/
	((pos*)th_blockallocator_ptr(&pos_alloc, pos1))->y = 10;
	pos_print(th_blockallocator_ptr(&pos_alloc, pos1));


	printf("pos1 = %p\n", TH_BLOCK_ACCESS(&pos_alloc, pos1, pos));
	printf("pos2 = %p\n", TH_BLOCK_ACCESS(&pos_alloc, pos2, pos));
	printf("pos2-aligned_sizeof(pos) = %p\n",
		   ((uint8_t*)TH_BLOCK_ACCESS(&pos_alloc, pos2, pos))-pos_alloc.block_size);

	TL_TESTM((uint8_t*)TH_BLOCK_ACCESS(&pos_alloc, pos1, pos) ==
			 ((uint8_t*)TH_BLOCK_ACCESS(&pos_alloc, pos2, pos))-pos_alloc.block_size,
			 "Test if block 1 and block 2 are shifted by sizeof(pos) in memory\n");

	/*Can access temporary pointer variable*/
	th_blockallocator_return(&pos_alloc, pos2);

	printf("free blocks after 1 returned = (%d)\n", th_blockallocator_n_available(&pos_alloc));

	TL_TEST(th_blockallocator_n_available(&pos_alloc) == 19);

	th_block pos_new = th_blockallocator_take(&pos_alloc);

	TL_TEST(th_blockallocator_n_available(&pos_alloc) == 18);

	pos* pos_new_ptr = (pos*)th_blockallocator_ptr(&pos_alloc, pos_new);
	TL_TESTM(pos_new_ptr == pos2_ptr,
		"test if newly allocated block takes up space from freed block");

	pos_new_ptr->x = 4;
	pos_new_ptr->y = 4;

	printf("free blocks after 1 more taken = (%d)\n", th_blockallocator_n_available(&pos_alloc));
	pos_print(pos_new_ptr);
}


void
test_odd_block_size(void)
{
	const uint32_t blkcount = 55;

    uint8_t mem[TH_BLOCKALLOCATOR_OPTIMAL_MEMSIZE(20, sizeof(pos))];

	TL_PRINT("memsize = %ld\n", TH_BLOCKALLOCATOR_OPTIMAL_MEMSIZE(20, sizeof(pos)));

	th_blockallocator pos_alloc = {0};

	TL_TEST(th_blockallocator_init(&pos_alloc, mem, sizeof(pos), 20) == TH_INVALID_INPUT);
	TL_TEST(th_blockallocator_init(&pos_alloc, mem, _TH_ALIGN(sizeof(pos)), 20) == TH_OK);
	TL_TEST(pos_alloc.allocator.memory == mem);

	uint32_t i = 0;
	th_block tmp[blkcount];
	for (i = 0; i < blkcount; i++) {
		tmp[i] = th_blockallocator_take(&pos_alloc);
	}
	printf("last block = %d, i = %d\n", tmp[i-1], i);
	TL_TEST(tmp[i-1] == i);
}

#define IDX_ALIGN(IDX, ALIGN) \
	( ((IDX)+(ALIGN)-1) & ~((ALIGN)-1) )
#define IDX_ALIGNMENT_OFFSET(IDX, ALIGN) \
	( (((IDX)+(ALIGN)-1) & ~((ALIGN)-1)) - IDX )
#define IDX_IS_ALIGNED(IDX, ALIGN) \
	( ((IDX) & ((ALIGN)-1)) == 0 )

void alignment_test(void) {
	const uint32_t align = 16;
	uint32_t idx = sizeof(pos);
	uint32_t ofs = 0;

	printf("sizeof pos = %d, alignment = %d\n", idx, align);
	ofs = IDX_ALIGNMENT_OFFSET(idx, align);
	idx = IDX_ALIGN(idx, align);
	printf("aligned pos = %d, offset = %d, is aligned = %d\n",
		   idx, ofs, IDX_IS_ALIGNED(idx, align));

	idx = 17;
	printf("sizeof pos = %d, alignment = %d\n", idx, align);
	ofs = IDX_ALIGNMENT_OFFSET(idx, align);
	idx = IDX_ALIGN(idx, align);
	printf("aligned pos = %d, offset = %d, is aligned = %d\n",
		   idx, ofs, IDX_IS_ALIGNED(idx, align));

}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	TL(test_custom_struct(););
	//TL(test_odd_block_size(););
	//TL(alignment_test(););
	tl_summary();
	return 0;
}
