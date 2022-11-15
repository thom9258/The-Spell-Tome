#include "test_datatypes.c"
#include "testlib.h"

#include "../dynamicAllocator.h"

#include <stdlib.h>

void
internal_test_fragmentation(int iterations,
                            int _min,
                            int _max,
                            uint8_t _alignment,
                            void* (*_fit)(dynAllocator_s*, uint32_t))
{
	int i;
	int mem;
	int access;
	int N = iterations - (int)(iterations * 0.8);
	uint32_t size = N * (_max + sizeof(_dynAllocator_dataheader_s) + 16);
	void* p_da[N];
	void* p_ma[N];
	tl_timer_s timer_da = {0};
	tl_timer_s timer_malloc = {0};
	tl_rand_seed(clock());

    uint8_t* memory = (uint8_t*)malloc(size);
	dynAllocator_s allocator = {0};
	dynAllocator_init(&allocator, memory, sizeof(memory), _alignment);
	allocator.fit_algorithm = _fit;

	/*Initialize ptrs*/
	for (i = 0; i < N; i++) {
		p_da[i] = NULL;
		p_ma[i] = NULL;
	}

	for (i = 0; i < iterations; i++) {
		access = tl_rand_ubetween(0, N-2);
		mem = tl_rand_ubetween( _min, _max);
//		printf("i=%d, mem=%d idx=%d\n", i, mem, access);
		/*Dynamic alloator*/
		tl_timer_start(&timer_da);
		if (p_da[access] == NULL) {
			p_da[access] = dynAllocator_malloc(&allocator, mem); 
		} else {
			p_da[access] = dynAllocator_free(&allocator, p_da); 
		}
		tl_timer_stop(&timer_da);
		/*malloc*/
		tl_timer_start(&timer_malloc);
		if (p_ma[access] == NULL) {
			p_ma[access] = malloc(mem);
		}
		else {
			free(p_ma[access]); 
			p_ma[access] = NULL;
		}
		tl_timer_stop(&timer_malloc);
	}

	/*Cleanup dynamic allocator*/
	dynAllocator_free_all(&allocator);
    free(memory);
	/*Cleanup malloc*/
	for (i = 0; i < N; i++)
		if (p_ma[i] != NULL)
			free(p_ma[i]); 

	if (allocator.fit_algorithm == dynAllocator_first_fit)
		TL_PRINT("FIRST FIT:\n");
//else if (allocator.fit_algorithm == dynAllocator_END_FIT)
//	TL_PRINT("END FIT:\n");
//else if (allocator.fit_algorithm == dynAllocator_BEST_FIT)
//	TL_PRINT("BEST FIT:\n");
	TL_PRINT("\tAlignment: %d\n", _alignment);
	TL_PRINT("\tRandom size allocations (%d) between %d,%d\n", iterations, _min, _max);
	TL_PRINT("\tDynamic Allocator: %lfs\n", timer_da.elapsed_sec);
	TL_PRINT("\tmalloc():          %lfs\n", timer_malloc.elapsed_sec);
	TL_TEST(timer_da.elapsed_sec < timer_malloc.elapsed_sec);

}
