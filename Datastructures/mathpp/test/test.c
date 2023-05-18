#include <stdio.h>

#define MATHPLUSPLUS_IMPLEMENTATION
#include "../mathpp.h"

int main(void) {

    Mat44 a = mat44(11, 12, 13, 14,
                    21, 22, 23, 24,
                    31, 32, 33, 34,
                    41, 42, 43, 44);

    Mat44 b = mat44_identity();
    b.m13 = 27;
    b.m22 = 4;
    b.m31 = -3.5;
    b.m24 = .22;
    Mat44 c = mat44_multiply(a, b);

    printf("a:\n" MAT44_PSTRING, MAT44_EXPAND(a));
    printf("b:\n" MAT44_PSTRING, MAT44_EXPAND(b));
    printf("c:\n" MAT44_PSTRING, MAT44_EXPAND(c));



    Vec3 t = vec3(1.2, 0, 4.7);
    printf(VEC3_PSTRING "\n", VEC3_EXPAND(t));
    return 0;
}

