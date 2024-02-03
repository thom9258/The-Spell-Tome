#include <bits/stdint-uintn.h>
#include <stdio.h>

#define DARRAY_IMPLEMENTATION
#include "../dynamic-array.h"

#define ARRINTPRINT(vars)                                                      \
	do {                                                                       \
	int32_t* v = 0;                                                            \
	array_foreach(v, vars) { printf("%d ", *v);} printf("\n");                 \
	} while (0)

#define LINE \
	printf("---------------------------------------------------------------\n")

void
test_usage(void)
{
	int i;

	array_new(int32_t) arr = NULL;
    array_init(arr);
	printf("array pointer (%p)\n", arr);

	printf("size = (%d) max = (%d)\n", array_size(arr), array_max(arr));
	array_push(arr, 69);
	printf("array pointer (%p)\n", arr);
	printf("size = (%d) max = (%d)\n", array_size(arr), array_max(arr));
	array_push(arr, 420);
	printf("size = (%d) max = (%d)\n", array_size(arr), array_max(arr));
	array_push(arr, 255);
	printf("size = (%d) max = (%d)\n", array_size(arr), array_max(arr));
	array_push(arr, 0);
	printf("size = (%d) max = (%d)\n", array_size(arr), array_max(arr));
	array_push(arr, 11);
	printf("size = (%d) max = (%d)\n", array_size(arr), array_max(arr));

	for (i = 0; i < 10; i++) {
		array_push(arr, i);
		printf("size = (%d) max = (%d)\n", array_size(arr), array_max(arr));
	}
	ARRINTPRINT(arr);

	for (i = 0; i < 8; i++)
		array_pop(arr);
	ARRINTPRINT(arr);

	for (i = 0; i < 2; i++)
		array_dequeue(arr);
	ARRINTPRINT(arr);

	printf("free ptr = (%p)\n", array_header(arr)->fn_free);

	array_free(arr);
}

void
test_dequeue(void)
{
	int i;
	array_new(int32_t) arr = {0};

	for (i = 0; i < 10; i++)
		array_push(arr, i);
	ARRINTPRINT(arr);

	for (i = 0; i < 2; i++)
		array_dequeue(arr);
	ARRINTPRINT(arr);

	array_free(arr);
}

void
test_swap(void)
{
	array_new(int32_t) arr = {0};

	array_push(arr, 69);
	array_push(arr, 420);
	array_push(arr, 255);
	array_push(arr, 0);
	array_push(arr, 11);
	ARRINTPRINT(arr);

	array_swap(arr, 0, 1);
	ARRINTPRINT(arr);

	array_swap(arr, 2, 4);
	ARRINTPRINT(arr);

	array_free(arr);
}

void
test_iterators(void)
{
	uint32_t i;
	int32_t* number;
	int32_t* it;
	array_new(int32_t) numbers = {0};

	array_push(numbers, 69);
	array_push(numbers, 420);
	array_push(numbers, 255);
	array_push(numbers, 0);
	array_push(numbers, 11);

	for (i = 0; i < array_size(numbers); i++)
		printf("%d ", numbers[i]);
	printf("\n");

	it = numbers;
	array_iterator(it, numbers)
		printf("%d ", *it++);
	printf("\n");

	it = numbers + 2;
	array_iterator(it, numbers)
		printf("%d ", *it++);
	printf("\n");

	array_foreach(number, numbers)
		printf("%d ", *number);
	printf("\n");

	array_free(numbers);
}

typedef struct PAGE_4096 {
	uint64_t id;
	uint8_t  data[4096];
}PAGE_4096;

void
test_custom_large_data(void)
{
	uint32_t i;
	array_new(PAGE_4096) pages = {0};

	PAGE_4096 PAGE_TEMPLATE = {0,{0}};
	for (i = 0; i < 1000; i++) {
		array_push(pages, PAGE_TEMPLATE);
		pages[i].id = i;
	}
	printf("Pushed (%d) pages to array\n", i);

	PAGE_4096* it = pages;
	array_iterator(it, pages) {
		//printf("page (id = %ld, size = %ld)\n", it->id, sizeof(it->data));
		it++;
	}

	array_free(pages);
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

#ifdef __cplusplus
	printf("-----------------------------------\n");
	printf("COMPILED IN C++\n");
	printf("-----------------------------------\n");
#else /*not __cplusplus*/ 
	printf("-----------------------------------\n");
	printf("COMPILED IN C99\n");
	printf("-----------------------------------\n");
#endif /*__cplusplus*/

	printf("usage\n");
	test_usage();
	LINE;
	printf("dequeue\n");
	test_dequeue();
	LINE;
	printf("swap\n");
	test_swap();
	LINE;
	printf("iterators\n");
	test_iterators();
	LINE;
	printf("large scale data\n");
	test_custom_large_data();
	LINE;
	return 0;
}
