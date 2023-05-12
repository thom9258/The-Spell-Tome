#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "testlib.h"

#define TH_ALLOCATOR_IMPLEMENTATION
//#define TH_NO_MUTEX
#include "../th_allocator.h"

typedef struct {
	int x;
	int y;
}v2;

v2
v2s(int x, int y)
{
	return (v2) {x,y};
}

void
v2_print(v2* _v)
{
	printf("v2 (%d,%d) at ptr(%p)\n", _v->x, _v->y, _v);
}

void
th_allocator_debug(dynallocator* _da, FILE* _out)
/**
 * dynAllocator_debug_status() - debug dynamic allocator.
 * @arg1: Ptr to dynamic allocator.
 * @arg2: output file for debugging.
 */
{
    int i = 0;
	uint8_t* p = NULL;
	_dynalc_header* ph = NULL;
	if (_da == NULL || _out == NULL)
		return;

	fprintf(_out,"(memsize=%d, start=%p, aloc_curr=%d, aloc_tot=%d)\n",
	              _da->region.len, _da->region.mem,
	              _da->bookkeeper.curr,
                  _da->bookkeeper.total);

	p = _da->first;
	ph = _TH_ALLOCATOR_DATA2HEADER(p);
	while (p != NULL) {
       
		fprintf(_out, "\t[%d] (size=%d, aligned=%s%s, S=%ld, E=%ld, N=%ld)\n",
               i,
		       ph->size,
               (_TH_IS_ALIGNED((unsigned long)ph)) ? "Y": "N",
               (_TH_IS_ALIGNED((unsigned long)p)) ? "Y": "N",
		       (uint8_t*)ph - _da->region.mem,
		       (uint8_t*)p + ph->size - _da->region.mem,
		       (uint8_t*)_TH_ALLOCATOR_DATA2HEADER(ph->next) - _da->region.mem);
		p = ph->next;
		ph = _TH_ALLOCATOR_DATA2HEADER(p);
        i++;
	}
}

void
test_init_destroy(void)
{
    uint8_t memory[TH_MB_2_B(2)] = {0};
	dynallocator da = {0};
	dynalc_init(&da, region(memory, sizeof(memory)));

	TL_TEST(da.region.mem != NULL);
	TL_TEST(da.region.len == sizeof(memory));
	TL_TEST(da.first == NULL);
	TL_TEST(da.last == NULL);
	TL_TEST(da.bookkeeper.curr == 0);
	TL_TEST(da.bookkeeper.total == 0);
}

void
test_first_fit_first_allocation(void)
{
	v2* p = NULL;
    uint8_t memory[TH_MB_2_B(2)] = {0};
	dynallocator da = {0};
	dynalc_init(&da, region(memory, sizeof(memory)));

	p = dynalc_malloc(&da, sizeof(v2)); 
	TL_TEST(p != NULL);
	p->x = 2;
	p->y = 5;
	v2_print(p);
	TL_TEST(da.first != NULL && da.first == (uint8_t*)p);
	TL_TEST(_TH_ALLOCATOR_DATA2HEADER(p)->next == NULL);
	TL_TEST(_TH_ALLOCATOR_DATA2HEADER(p)->size == sizeof(v2));

	TL_TEST(da.first == (uint8_t*)p);
	TL_TEST(da.bookkeeper.curr == 1);
	TL_TEST(da.bookkeeper.total == 1);

	dynalc_free_all(&da);

	TL_TEST(da.first == NULL);
	TL_TEST(da.bookkeeper.curr == 0);
	TL_TEST(da.bookkeeper.total == 1);
}
void
test_first_fit_consecutive_allocations(void)
{
	unsigned int N = 8;

    uint8_t memory[TH_MB_2_B(2)] = {0};
	dynallocator da = {0};
	dynalc_init(&da, region(memory, sizeof(memory)));

	unsigned int i;
	unsigned int correct = 0;
	v2* p[N];

	for (i = 0; i < N; i++) {
		p[i] = dynalc_malloc(&da, sizeof(v2)); 
		if (p[i] != NULL)
			correct++;
		else 
			continue;
		p[i]->x = i;
		p[i]->y = i;
		v2_print(p[i]);
	}

    th_allocator_debug(&da, stdout);

	TL_TEST(da.first == (uint8_t*)p[0]);
	TL_TEST(correct == N);
	TL_TEST(da.bookkeeper.curr == N);
	TL_TEST(da.bookkeeper.total == N);

	dynalc_free_all(&da);
	TL_TEST(da.bookkeeper.curr == 0);
	TL_TEST(da.first == NULL);
}

void
test_first_fit_freeing_of_allocations_first_last(void)
{
	const unsigned int N = 4;
	unsigned int i;

    uint8_t memory[TH_MB_2_B(2)] = {0};
	dynallocator da = {0};
	dynalc_init(&da, region(memory, sizeof(memory)));

	v2* p[N];
	for (i = 0; i < N; i++) {
		p[i] = dynalc_malloc(&da, sizeof(v2)); 
		if (p[i] == NULL)
			continue;
		p[i]->x = i;
		p[i]->y = i;
	}

	dynalc_free(&da, p[0]);
	TL_TESTM(da.first == (uint8_t*)p[1], "first allocation is removed");
	TL_TEST(da.bookkeeper.curr == N - 1);

	dynalc_free(&da, p[N-1]);
	if (p[N-2] != NULL)
		TL_TESTM(_TH_ALLOCATOR_DATA2HEADER(p[N-2])->next == NULL, "last allocation is removed from chain");
	TL_TEST(da.bookkeeper.curr == N - 2);
	th_allocator_debug(&da, stdout);
	dynalc_free_all(&da);
}

void
test_first_fit_freeing_of_allocations_inside_chain(void)
{
	const int N = 4;
	int i;
	uint32_t allocs = 0;

    uint8_t memory[TH_MB_2_B(2)] = {0};
	dynallocator da = {0};
	dynalc_init(&da, region(memory, sizeof(memory)));
    
	v2* p[N];
	for (i = 0; i < N; i++) {
		p[i] = dynalc_malloc(&da, sizeof(v2)); 
		if (p[i] == NULL)
			continue;
		p[i]->x = i;
		p[i]->y = i;
	}
	allocs = da.bookkeeper.curr;
	th_allocator_debug(&da, stdout);

	dynalc_free(&da, p[1]);
	TL_TESTM(_TH_ALLOCATOR_DATA2HEADER(p[0])->next == (uint8_t*)p[2], "sucessfully removed allocation 2");
	TL_TEST(da.bookkeeper.curr == allocs - 1);
	th_allocator_debug(&da, stdout);

	dynalc_free(&da, p[2]);
	TL_TESTM(_TH_ALLOCATOR_DATA2HEADER(p[0])->next == (uint8_t*)p[3], "sucessfully removed allocation 3");

	TL_TEST(da.bookkeeper.curr == allocs - 2);
	th_allocator_debug(&da, stdout);
}

void
test_first_fit_allocation_after_free(void)
{
	const int N = 8;
	int i;
	v2* p_new0 = NULL;
	v2* p_new1 = NULL;
	v2* p_new2 = NULL;

    uint8_t memory[TH_MB_2_B(2)] = {0};
	dynallocator da = {0};
	dynalc_init(&da, region(memory, sizeof(memory)));

    v2* p[N];
	for (i = 0; i < N; i++) {
		p[i] = dynalc_malloc(&da, sizeof(v2)); 
		if (p[i] == NULL)
			continue;
		*p[i] = v2s(i,2*i);
	}
	th_allocator_debug(&da, stdout);

	dynalc_free(&da, p[1]);
	dynalc_free(&da, p[2]);
	th_allocator_debug(&da, stdout);

	p_new1 = dynalc_malloc(&da, sizeof(v2)); 
	TL_TESTM(p_new1 == p[1], "Fit new allocation inside chain");
	TL_TEST(p_new1 != NULL || p[1] != NULL);
	th_allocator_debug(&da, stdout);

	p_new2 = dynalc_malloc(&da, sizeof(v2)); 
	TL_TEST(p_new2 != NULL || p[2] != NULL);
	TL_TESTM(p_new2 == p[2], "Fit new allocation inside chain");
	th_allocator_debug(&da, stdout);

	dynalc_free(&da, p[0]);
	p_new0 = dynalc_malloc(&da, sizeof(v2)); 
	TL_TEST(p_new0 != NULL || p[0] != NULL);
	TL_TESTM(p_new0 == p[0], "Fit new allocation before chain");
	th_allocator_debug(&da, stdout);

	dynalc_free_all(&da);
}

void
test_first_fit_max_allocations(void)
{
	int i;
	int correct = 0;
	v2* p[10];

    const int entrysize = _TH_ALIGN(sizeof(v2)) + _TH_ALIGN(sizeof(_dynalc_header));
    uint8_t memory[entrysize * 8];
	dynallocator da = {0};
	dynalc_init(&da, region(memory, sizeof(memory)));

	for (i = 0; i < 10; i++) {
		p[i] = dynalc_malloc(&da, sizeof(v2)); 
		if (p[i] != NULL)
			correct++;
	}
	TL_PRINT("size of entry = %d\n", entrysize);
    printf("used memory = %d\n", correct * entrysize);
    printf("allocator memory size = %d\n", da.region.len);
    printf("correct allocations=%d\n", correct);
	TL_TESTM(correct == 8, "Check if all allocations were successful.");
	TL_TESTM(p[8] == NULL || p[9] == NULL, "No allocations occours when allocator is full");
	th_allocator_debug(&da, stdout);

	dynalc_free_all(&da);
}

void
test_fragmentation(int iterations, int _min, int _max)
{
	tl_timer_s timer_da = {0};
	tl_timer_s timer_malloc = {0};
	tl_rand_seed(clock());

	int i;
	int mem;
	int access;
    /*Create pointer space*/
	int N = iterations - (int)(iterations * 0.5);
	void* p_da[N];
	void* p_ma[N];

    /*Create memory space for dynamic allocator*/
	uint32_t da_memsize = N * (_TH_ALIGN(_max) + _TH_ALIGN(sizeof(_dynalc_header)));
    uint8_t* memory = (uint8_t*)malloc(da_memsize);
	dynallocator allocator = {0};
	dynalc_init(&allocator, region(memory, sizeof(memory)));

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
			p_da[access] = dynalc_malloc(&allocator, mem); 
		} else {
			p_da[access] = dynalc_free(&allocator, p_da); 
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
	dynalc_free_all(&allocator);
    free(memory);
	/*Cleanup malloc*/
	for (i = 0; i < N; i++)
		if (p_ma[i] != NULL)
			free(p_ma[i]); 

	TL_PRINT("FIRST FIT:\n");
	TL_PRINT("\tRandom size allocations (%d) between %d,%d\n", iterations, _min, _max);
	TL_PRINT("\tDynamic Allocator: %lfs\n", timer_da.elapsed_sec);
	TL_PRINT("\tmalloc():          %lfs\n", timer_malloc.elapsed_sec);
	TL_TEST(timer_da.elapsed_sec < timer_malloc.elapsed_sec);

}

void
test_fragmentation_tests(void)
{
	test_fragmentation(1000000, 500, 5000);
	test_fragmentation(1000000, 500, 5000);
	test_fragmentation(1000000, 5000, 50000);
	test_fragmentation(1000000, 10000, 100000);
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	TL(test_init_destroy());
	TL(test_first_fit_first_allocation(););
    TL(test_first_fit_consecutive_allocations(););
    TL(test_first_fit_freeing_of_allocations_first_last(););
    TL(test_first_fit_freeing_of_allocations_inside_chain(););
    TL(test_first_fit_allocation_after_free(););
    TL(test_first_fit_max_allocations(););

    TL(test_fragmentation_tests(););

	tl_summary();
	return 0;
}

