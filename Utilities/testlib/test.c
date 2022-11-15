#include "testlib.h"

int
plus(int a, int b)
{
	return a+b;
}

void
test_plus_simple(void)
{
	TL_TEST(2+2==5);
	TL_TEST(2+2==4);
	TL_TEST(4+4==7);
}

void
test_plus(int N)
{
	int a;
	int b;
	int i;
	for (i = 0; i < N; i++) {
		a = tl_rand_ubetween(0, 300);
		b = tl_rand_ubetween(0, 300);
		TL_TESTM(plus(a,b) == a+b, "Sucsesfully calculated a corect result");
	}
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	TLM(test_plus_simple(),
	    "This tests basic math with the implemented \"plus()\" function.");
	TLM(test_plus(10),
	    "Random math tests for \"plus()\" function.");
	tl_summary();
	return 0;
}
