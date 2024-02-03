#include "testlib.h"

#define CTUPLE_IMPLEMENTATION
#include "../ctuple.h"


#define _ctuple_offset(TUPLE, INDEX)                                           \
	( (INDEX > 0 && INDEX < TUPLE.count) ? TUPLE.data[INDEX] : 0 )

#define ctuple_set(TUPLE, INDEX, TYPE, VALUE)					\
	( *(TYPE*)(TUPLE + (INDEX * sizeof(CTUPLE_SIZE_T))) = VALUE )

#define ctuple_get()

void
test_ctuple_ro(void)
{
	ctuple t = ctuple_new(3, sizeof(int), sizeof(char), sizeof(int));
	ctuple_set(t, 0, int,  599);
	ctuple_set(t, 1, char, 'h');
	ctuple_set(t, 2, int,  90000);

	TL_TEST(ctuple(t,0) == 599);
	TL_TEST(ctuple(t,1) == 'h');
	TL_TEST(ctuple(t,2) == 90000);
	ctuple_destroy(t);
}

ctuple_def(int, int, int)
foo(int a, int b)
{
	ctuple out = ctuple_new(sizeof(int), sizeof(int), sizeof(int));
	ctuple(out,0) = a;
	ctuple(out,1) = b;
	ctuple(out,2) = a*b;
	return out;
}

void
test_ctuple_function(void)
{
	ctuple t = foo(2,4);
	int a  = ctuple(t,0);
	int b  = ctuple(t,1);
	int ab = ctuple(t,2);
	ctuple_destroy(t);

	TL_TEST(a == 2);
	TL_TEST(b == 4);
	TL_TEST(ab == a*b);
	printf("a*b=ab -> %d*%d=%d\n", a,b,ab);
}

typedef struct {
	float x;
	float y;
}v2;

typedef struct {
	v2 pos;
	v2 size;
}rectangle;

ctuple_def(float x, float y, float w, float h, v2 pos, v2 size, float volume)
rectangle_decompose(rectangle rect)
{
	ctuple out = ctuple_new(sizeof(float),
							sizeof(float),
							sizeof(float),
							sizeof(float),
							sizeof(v2),
							sizeof(v2),
							sizeof(float));
	ctuple(out,0) = rect.pos.x;
	ctuple(out,1) = rect.pos.y;
	ctuple(out,2) = rect.size.x;
	ctuple(out,3) = rect.size.y;
	(v2)ctuple(out,4) = rect.pos;
	ctuple(out,5) = rect.size;
	ctuple(out,6) = (rect.size.x - rect.pos.x) * (rect.size.y - rect.pos.y);
	return out;
}

test_ctuple_rectangle_decompose(void)
{
	rectangle rect = {{1,2}, {3,4}};
	ctuple t = rectangle_decompose(rect);
	ctuple_destroy(t);

	TL_TEST(ctuple(t,0) == 1);
	TL_TEST(ctuple(t,1) == 2);
	TL_TEST(ctuple(t,2) == 3);
	TL_TEST(ctuple(t,3) == 4);
	TL_TEST(ctuple(t,6) == (3-1)*(4-2));
	printf("area of rectangle = %d\n",ctuple(t,6));
}

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	TL(test_ctuple_ro());
	TL(test_ctuple_function());

	tl_summary();
	return 0;
}
