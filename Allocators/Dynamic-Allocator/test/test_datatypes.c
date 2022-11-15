#ifndef V2_TEST
#define V2_TEST

#include <stdio.h>

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

#endif /*V2_TEST*/
