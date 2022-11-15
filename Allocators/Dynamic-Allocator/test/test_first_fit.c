#include "test_datatypes.c"
#include "testlib.h"

#ifndef DYNAMICALLOCATOR_IMPLEMENTATION
#define DYNAMICALLOCATOR_IMPLEMENTATION
#define DYNAMICALLOCATOR_DEBUG
#endif /*DYNAMICALLOCATOR_IMPLEMENTATION*/
#include "../dynamicAllocator.h"

void
test_init_destroy(void)
{
    uint8_t memory[DYNAMICALLOCATOR_MB(2)] = {0};
	dynAllocator_s allocator = {0};
	dynAllocator_init(&allocator, &memory, sizeof(memory), 16);
	TL_TEST(allocator.memory != NULL);
	TL_TEST(allocator.memsize == sizeof(memory));
	TL_TEST(allocator.first == NULL);
	TL_TEST(allocator.last == NULL);
	TL_TEST(allocator.book_keep.alloc_count_current == 0);
	TL_TEST(allocator.book_keep.alloc_count_total == 0);
}

void
internal_test_first_fit_first_allocation(void)
{
	v2* p = NULL;
	uint8_t memory[4016] = {0};
	dynAllocator_s allocator = {0};
	dynAllocator_init(&allocator, &memory, sizeof(memory), 16);
	allocator.fit_algorithm = dynAllocator_first_fit;

	p = dynAllocator_malloc(&allocator, sizeof(v2)); 
	TL_TEST(p != NULL);
	p->x = 2;
	p->y = 5;
	v2_print(p);
	TL_TEST(allocator.first != NULL && allocator.first == (uint8_t*)p);
	TL_TEST(_dynAllocator_data2header(p)->next == NULL);
	TL_TEST(_dynAllocator_data2header(p)->size == sizeof(v2));

	dynAllocator_info_s info = dynAllocator_info(&allocator);

	TL_TEST(info.allocator->first == (uint8_t*)p);
	TL_TEST(info.alloc_count_current == 1);
	TL_TEST(info.alloc_count_total == 1);


	dynAllocator_free_all(&allocator);
}
void
internal_test_first_fit_consecutive_allocations(void)
{
	unsigned int N = 8;

	uint8_t memory[4016] = {0};
	dynAllocator_s allocator = {0};
	dynAllocator_init(&allocator, &memory, sizeof(memory), 16);
	allocator.fit_algorithm = dynAllocator_first_fit;

	unsigned int i;
	unsigned int correct = 0;
	v2* p[N];

	for (i = 0; i < N; i++) {
		p[i] = dynAllocator_malloc(&allocator, sizeof(v2)); 
		if (p[i] != NULL)
			correct++;
		else 
			continue;
		p[i]->x = i;
		p[i]->y = i;
		v2_print(p[i]);
	}

    dynAllocator_debug(&allocator, stdout);
	dynAllocator_info_s info = dynAllocator_info(&allocator);

	TL_TEST(info.allocator->first == (uint8_t*)p[0]);
	TL_TEST(correct == N);
	TL_TEST(info.alloc_count_current == N);
	TL_TEST(info.alloc_count_total == N);

	dynAllocator_free_all(&allocator);
    info = dynAllocator_info(&allocator);
	TL_TEST(info.alloc_count_current == 0);
	TL_TEST(info.allocator->first == NULL);
}

void
internal_test_first_fit_freeing_of_allocations_first_last(void)
{
	const unsigned int N = 4;
	unsigned int i;

	uint8_t memory[DYNAMICALLOCATOR_MB(1)] = {0};
	dynAllocator_s allocator = {0};
	dynAllocator_init(&allocator, &memory, sizeof(memory), 16);
	allocator.fit_algorithm = dynAllocator_first_fit;

	v2* p[N];
	for (i = 0; i < N; i++) {
		p[i] = dynAllocator_malloc(&allocator, sizeof(v2)); 
		if (p[i] == NULL)
			continue;
		p[i]->x = i;
		p[i]->y = i;
	}

	dynAllocator_free(&allocator, p[0]);
	TL_TESTM(allocator.first == (uint8_t*)p[1], "first allocation is removed");
	TL_TEST(allocator.book_keep.alloc_count_current == N - 1);

	dynAllocator_free(&allocator, p[N-1]);
	if (p[N-2] != NULL)
		TL_TESTM(_dynAllocator_data2header(p[N-2])->next == NULL, "last allocation is removed from chain");
	TL_TEST(allocator.book_keep.alloc_count_current == N - 2);
	dynAllocator_debug(&allocator, stdout);

	dynAllocator_free_all(&allocator);
}

void
internal_test_first_fit_freeing_of_allocations_inside_chain(void)
{
	const int N = 4;
	int i;
	uint32_t allocs = 0;

    uint8_t memory[DYNAMICALLOCATOR_MB(1)] = {0};
	dynAllocator_s allocator = {0};
	dynAllocator_init(&allocator, &memory, sizeof(memory), 16);
	allocator.fit_algorithm = dynAllocator_first_fit;

	v2* p[N];
	for (i = 0; i < N; i++) {
		p[i] = dynAllocator_malloc(&allocator, sizeof(v2)); 
		if (p[i] == NULL)
			continue;
		p[i]->x = i;
		p[i]->y = i;
	}
	allocs = allocator.book_keep.alloc_count_current;
	dynAllocator_debug(&allocator, stdout);

	dynAllocator_free(&allocator, p[1]);
	TL_TESTM(_dynAllocator_data2header(p[0])->next == (uint8_t*)p[2], "sucessfully removed allocation 2");
	TL_TEST(allocator.book_keep.alloc_count_current == allocs - 1);
	dynAllocator_debug(&allocator, stdout);

	dynAllocator_free(&allocator, p[2]);
	TL_TESTM(_dynAllocator_data2header(p[0])->next == (uint8_t*)p[3], "sucessfully removed allocation 3");
	TL_TEST(allocator.book_keep.alloc_count_current == allocs - 2);
	dynAllocator_debug(&allocator, stdout);

	dynAllocator_free_all(&allocator);
}

void
internal_test_first_fit_allocation_after_free(void)
{
	const int N = 8;
	int i;
	v2* p_new0 = NULL;
	v2* p_new1 = NULL;
	v2* p_new2 = NULL;

    uint8_t memory[DYNAMICALLOCATOR_MB(1)] = {0};
	dynAllocator_s allocator = {0};
	dynAllocator_init(&allocator, &memory, sizeof(memory), 16);
	allocator.fit_algorithm = dynAllocator_first_fit;

	v2* p[N];
	for (i = 0; i < N; i++) {
		p[i] = dynAllocator_malloc(&allocator, sizeof(v2)); 
		if (p[i] == NULL)
			continue;
		*p[i] = v2s(i,2*i);
	}
	dynAllocator_debug(&allocator, stdout);

	dynAllocator_free(&allocator, p[1]);
	dynAllocator_free(&allocator, p[2]);
	dynAllocator_debug(&allocator, stdout);

	p_new1 = dynAllocator_malloc(&allocator, sizeof(v2)); 
	TL_TESTM(p_new1 == p[1], "Fit new allocation inside chain");
	TL_TEST(p_new1 != NULL || p[1] != NULL);
	dynAllocator_debug(&allocator, stdout);

	p_new2 = dynAllocator_malloc(&allocator, sizeof(v2)); 
	TL_TEST(p_new2 != NULL || p[2] != NULL);
	TL_TESTM(p_new2 == p[2], "Fit new allocation inside chain");
	dynAllocator_debug(&allocator, stdout);

	dynAllocator_free(&allocator, p[0]);
	p_new0 = dynAllocator_malloc(&allocator, sizeof(v2)); 
	TL_TEST(p_new0 != NULL || p[0] != NULL);
	TL_TESTM(p_new0 == p[0], "Fit new allocation before chain");
	dynAllocator_debug(&allocator, stdout);

	dynAllocator_free_all(&allocator);
}

void
internal_test_first_fit_max_allocations(void)
{
    #define min_entry_size sizeof(v2) + sizeof(_dynAllocator_dataheader_s) + 32 
	int i;
	int correct = 0;
	v2* p[10];

    uint8_t memory[min_entry_size * 8] = {0};
	dynAllocator_s allocator = {0};
	dynAllocator_init(&allocator, &memory, sizeof(memory), 16);
	allocator.fit_algorithm = dynAllocator_first_fit;

	for (i = 0; i < 10; i++) {
		p[i] = dynAllocator_malloc(&allocator, sizeof(v2)); 
		if (p[i] != NULL)
			correct++;
	}
	TL_PRINT("size of entry = %ld\n", min_entry_size);
    printf("used memory = %ld\n", correct * min_entry_size);
    printf("allocator memory size = %d\n", allocator.memsize);
    printf("correct allocations=%d\n", correct);
	TL_TESTM(correct == 8, "Check if all allocations were successful.");
	TL_TESTM(p[8] == NULL || p[9] == NULL, "No allocations occours when allowator is full");
	dynAllocator_debug(&allocator, stdout);

	dynAllocator_free_all(&allocator);
}
