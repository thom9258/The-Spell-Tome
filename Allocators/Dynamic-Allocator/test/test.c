#include "testlib.h"
#include "test_first_fit.c"
#include "test_speeds.c"

#include "../dynamicAllocator.h"

void
test_first_fit(void)
{
	internal_test_first_fit_first_allocation();
    internal_test_first_fit_consecutive_allocations();
    internal_test_first_fit_freeing_of_allocations_first_last();
    internal_test_first_fit_freeing_of_allocations_inside_chain();
    internal_test_first_fit_allocation_after_free();
    internal_test_first_fit_max_allocations();
}

void
test_fragmentation(void)
{
	internal_test_fragmentation(1000000, 500, 5000, 0, dynAllocator_first_fit);
	internal_test_fragmentation(1000000, 500, 5000, 16, dynAllocator_first_fit);
	internal_test_fragmentation(1000000, 5000, 50000, 16, dynAllocator_first_fit);
	internal_test_fragmentation(1000000, 5000, 100000, 16, dynAllocator_first_fit);
}

void
test_best_fit(void)
{
    /*
	internal_test_best_fit_first_allocation();
	internal_test_best_fit_consecutive_allocations();
	internal_test_best_fit_freeing_of_allocations_first_last();
	internal_test_best_fit_freeing_of_allocations_inside_chain();
	internal_test_best_fit_allocation_after_free();
	internal_test_best_fit_max_allocations();
     * */
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	TLM( test_init_destroy(),
	     "Tests initialization and destruction of"
	     "the allocator implementation.")
	TLM( test_first_fit(),
	     "Allocator First Fit implementation test.")

	TL( test_fragmentation())

	tl_summary();
	return 0;
}
