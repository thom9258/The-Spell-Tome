#include <stdio.h>
#include <assert.h>

#include "testlib.h"

#define ALLOCATOR_IMPLEMENTATION
//#define MEMALLOCATOR_NO_ASSERT
#include "../allocator.h"

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
    uint8_t mem[BLKALC_OPTIMAL_MEMSIZE(20, sizeof(pos))];
	TL_PRINT("memsize = %ld\n", BLKALC_OPTIMAL_MEMSIZE(20, sizeof(pos)));
	blkallocator pos_alloc = {0};

	TL_TEST(blkalc_init(&pos_alloc, mem, sizeof(pos), 20) == ALLOCATOR_INVALID_INPUT);
	TL_TEST(blkalc_init(&pos_alloc, mem, _TH_ALIGN(sizeof(pos)), 20) == ALLOCATOR_OK);
	TL_TEST(pos_alloc.region.mem == mem);

	printf("total blocks = (%d)\n", pos_alloc.block_count);
	printf("free blocks = (%d)\n", blkalc_n_available(&pos_alloc));

	blk_handle pos1 = blkalc_take(&pos_alloc);
	TL_TEST(blk_ok(&pos1));
	TL_PRINT("pos1 = %d\n", pos1.blockid);
	TL_TEST(*blk_ptr(pos1, int) == 0);
	blk_handle pos2 = blkalc_take(&pos_alloc);
	TL_TEST(blk_ok(&pos2));
	TL_PRINT("pos1 = %d\n", pos1.blockid);
	TL_TEST(*blk_ptr(pos2, int) == 1);
	TL_TEST(blkalc_n_available(&pos_alloc) == 18);

	pos* pos2_ptr = blk_ptr(pos2, pos);
	TL_TEST(pos2_ptr != NULL);
	TL_TEST(_TH_IS_ALIGNED((unsigned long)pos2_ptr) == 1);
	pos2_ptr->x = 2;
	pos2_ptr->y = 0;
	pos_print(pos2_ptr);

	/*Can access block ptr by access macro*/
	blk_ptr(pos1, pos)->x = 5;
	TL_TEST(blk_ptr(pos1, pos)->x == 5);

	printf("pos1 = %p\n", blk_ptr(pos1, pos));
	printf("pos2 = %p\n", blk_ptr(pos2, pos));
	printf("pos2-aligned_sizeof(pos) = %p\n",
		   ((uint8_t*)blk_ptr(pos2, pos))-pos_alloc.block_size);

	TL_TESTM((uint8_t*)blk_ptr(pos1, pos) == ((uint8_t*)blk_ptr(pos2, pos))-pos_alloc.block_size,
			 "Test if block 1 and block 2 are shifted by sizeof(pos) in memory\n");

	/*Can access temporary pointer variable*/
	printf("free blocks = (%d)\n", blkalc_n_available(&pos_alloc));
    blk_handle_return(pos2);
	printf("free blocks after 1 returned = (%d)\n", blkalc_n_available(&pos_alloc));
	TL_TEST(blkalc_n_available(&pos_alloc) == 19);

	blk_handle pos_new = blkalc_take(&pos_alloc);
	TL_TEST(blkalc_n_available(&pos_alloc) == 18);

	pos* pos_new_ptr = blk_ptr(pos_new, pos);
	TL_TESTM(pos_new_ptr == pos2_ptr,
		"test if newly allocated block takes up space from freed block");

	pos_new_ptr->x = 4;
	pos_new_ptr->y = 4;
	printf("free blocks after 1 more taken = (%d)\n", blkalc_n_available(&pos_alloc));
	pos_print(pos_new_ptr);
}


void
test_odd_block_size(void)
{
	const uint32_t blkcount = 55;
    uint8_t mem[BLKALC_OPTIMAL_MEMSIZE(20, sizeof(pos))];
	TL_PRINT("memsize = %ld\n", BLKALC_OPTIMAL_MEMSIZE(20, sizeof(pos)));
	blkallocator pos_alloc = {0};

	TL_TEST(blkalc_init(&pos_alloc, mem, sizeof(pos), 20) == ALLOCATOR_INVALID_INPUT);
	TL_TEST(blkalc_init(&pos_alloc, mem, _TH_ALIGN(sizeof(pos)), 20) == ALLOCATOR_OK);
	TL_TEST(pos_alloc.region.mem == mem);

	uint32_t i = 0;
	blk_handle tmp[blkcount];
	for (i = 0; i < blkcount; i++) {
		tmp[i] = blkalc_take(&pos_alloc);
	}
	printf("last block = %d, i = %d\n", *blk_ptr(tmp[i], int), i);
	TL_TEST(*blk_ptr(tmp[i], uint32_t) == i);
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
