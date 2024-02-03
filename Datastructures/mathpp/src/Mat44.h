#ifndef MATHPLUSPLUS_H
#define MATHPLUSPLUS_H

// Additional math i want to implement:
// https://github.com/coreh/gl-matrix.c/blob/master/mat4.c#L114C1-L126C2

#ifndef MPP_NO_ASSERT
#  include <assert.h>
#  ifndef MPP_ASSERT
#    define MPP_ASSERT assert
#  endif
#endif

#ifndef MPP_UNIMPLEMENTED
#  define MPP_UNIMPLEMENTED(fn) MPP_ASSERT(0 && #fn "Is not implemented!")
#endif
#ifndef MPP_UNUSED
#  define MPP_UNUSED(x) (void)x
#endif

/*Formatting decimal size*/
#ifndef MPP_PFMT
#  define MPP_PFMT "% 16f"
#endif

typedef struct {
    union {
        /*Named accessors*/
        struct {    
            float m11, m12, m13, m14;
            float m21, m22, m23, m24;
            float m31, m32, m33, m34;
            float m41, m42, m43, m44;
        };
        /*Double Indexed Accessor*/
        float m[4][4];
        /*Single Indexed Accessor*/
        float raw[16];
    };
}Mat44;

#define MAT44_FMTSTRING \
    "M44| " MPP_PFMT ", " MPP_PFMT ", " MPP_PFMT ", " MPP_PFMT " |\n" \
    "   | " MPP_PFMT ", " MPP_PFMT ", " MPP_PFMT ", " MPP_PFMT " |\n" \
    "   | " MPP_PFMT ", " MPP_PFMT ", " MPP_PFMT ", " MPP_PFMT " |\n" \
    "   | " MPP_PFMT ", " MPP_PFMT ", " MPP_PFMT ", " MPP_PFMT " |\n"

#define MAT44_EXPAND(M) \
    M.m11, M.m12, M.m13, M.m14, \
    M.m21, M.m22, M.m23, M.m24, \
    M.m31, M.m32, M.m33, M.m34, \
    M.m41, M.m42, M.m43, M.m44

/*Matrices*/
Mat44 mat44_zero(void);
Mat44 mat44_filled(float fill);
Mat44 mat44(float v11, float v12, float v13, float v14,
            float v21, float v22, float v23, float v24,
            float v31, float v32, float v33, float v34,
            float v41, float v42, float v43, float v44);
Mat44 mat44_copy(Mat44 a);
Mat44 mat44_setrow(Mat44 a, int row, float v1, float v2, float v3, float v4);
Mat44 mat44_setcol(Mat44 a, int col, float v1, float v2, float v3, float v4);
void mat44_psetrow(Mat44* a, int row, float v1, float v2, float v3, float v4);
void mat44_psetcol(Mat44* a, int col, float v1, float v2, float v3, float v4);
Mat44 mat44_identity(void);
Mat44 mat44_transpose(Mat44 a);
char mat44_equal(Mat44 a, Mat44 b);
Mat44 mat44_plus(Mat44 a, Mat44 b);
Mat44 mat44_minus(Mat44 a, Mat44 b);
Mat44 mat44_multiply(Mat44 a, Mat44 b);

Mat44 mat44_inverse(Mat44 a);
float mat44_determinant(Mat44 a);
Mat44 mat44_rotate(Mat44 a, float angle, float ax, float ay, float az);

#ifdef MATHPLUSPLUS_IMPLEMENTATION

Mat44
mat44_zero(void)
{
    return (Mat44) {0};
}

Mat44
mat44_filled(float fill)
{
    Mat44 m = mat44_zero();
    int i;
    for (i = 0; i < 16; i++)
        m.raw[i] = fill;
    return m;
}

Mat44
mat44(float v11, float v12, float v13, float v14,
      float v21, float v22, float v23, float v24,
      float v31, float v32, float v33, float v34,
      float v41, float v42, float v43, float v44)
{
    Mat44 c = mat44_zero();
    c.m11 = v11; c.m21 = v21; c.m31 = v31; c.m41 = v41;
    c.m12 = v12; c.m22 = v22; c.m32 = v32; c.m42 = v42;
    c.m13 = v13; c.m23 = v23; c.m33 = v33; c.m43 = v43;
    c.m14 = v14; c.m24 = v24; c.m34 = v34; c.m44 = v44;
    return c;
}


Mat44 mat44_setrow(Mat44 a, int row, float v1, float v2, float v3, float v4)
{
    Mat44 m = a; 
    m.m[row][0] = v1;
    m.m[row][1] = v2;
    m.m[row][2] = v3;
    m.m[row][3] = v4;
    return m;
}
    

Mat44 mat44_setcol(Mat44 a, int col, float v1, float v2, float v3, float v4)
{
    Mat44 m = a; 
    m.m[0][col] = v1;
    m.m[1][col] = v2;
    m.m[2][col] = v3;
    m.m[3][col] = v4;
    return m;
}

void
mat44_psetrow(Mat44* a, int row, float v1, float v2, float v3, float v4)
{
    a->m[row][0] = v1;
    a->m[row][1] = v2;
    a->m[row][2] = v3;
    a->m[row][3] = v4;
}

void
mat44_psetcol(Mat44* a, int col, float v1, float v2, float v3, float v4)
{
    a->m[0][col] = v1;
    a->m[1][col] = v2;
    a->m[2][col] = v3;
    a->m[3][col] = v4;
}

Mat44
mat44_transpose(Mat44 a)
{
    //MPP_UNUSED(a);
    //MPP_UNIMPLEMENTED(mat44_transpose);
    Mat44 dst = mat44_zero();
    dst.raw[0]  = a.raw[0];
    dst.raw[1]  = a.raw[4];
    dst.raw[2]  = a.raw[8];
    dst.raw[3]  = a.raw[12];
    dst.raw[4]  = a.raw[1];
    dst.raw[5]  = a.raw[5];
    dst.raw[6]  = a.raw[9];
    dst.raw[7]  = a.raw[13];
    dst.raw[8]  = a.raw[2];
    dst.raw[9]  = a.raw[6];
    dst.raw[10] = a.raw[10];
    dst.raw[11] = a.raw[14];
    dst.raw[12] = a.raw[3];
    dst.raw[13] = a.raw[7];
    dst.raw[14] = a.raw[11];
    dst.raw[15] = a.raw[15];
    return dst;
}

Mat44
mat44_identity(void)
{
    Mat44 c = mat44_zero();
    c.m11 = 1;
    c.m22 = 1;
    c.m33 = 1;
    c.m44 = 1;
    return c;
}

char
mat44_equal(Mat44 a, Mat44 b)
{
    if (a.m11 == b.m11 && a.m21 == b.m21 && a.m31 == b.m31 && a.m41 == b.m41 &&
        a.m12 == b.m12 && a.m22 == b.m22 && a.m32 == b.m32 && a.m42 == b.m42 &&
        a.m13 == b.m13 && a.m23 == b.m23 && a.m33 == b.m33 && a.m43 == b.m43 &&
        a.m14 == b.m14 && a.m24 == b.m24 && a.m34 == b.m34 && a.m44 == b.m44 )
        return 1;
    return 0;
}

Mat44
mat44_copy(Mat44 a)
{
    return a;
}

Mat44
mat44_plus(Mat44 a, Mat44 b)
{
    Mat44 c = mat44_zero();
    c.m11 = a.m11 + b.m11;
    c.m21 = a.m21 + b.m21;
    c.m31 = a.m31 + b.m31;
    c.m41 = a.m41 + b.m41;

    c.m12 = a.m12 + b.m12;
    c.m22 = a.m22 + b.m22;
    c.m32 = a.m32 + b.m32;
    c.m42 = a.m42 + b.m42;

    c.m13 = a.m13 + b.m13;
    c.m23 = a.m23 + b.m23;
    c.m33 = a.m33 + b.m33;
    c.m43 = a.m43 + b.m43;

    c.m14 = a.m14 + b.m14;
    c.m24 = a.m24 + b.m24;
    c.m34 = a.m34 + b.m34;
    c.m44 = a.m44 + b.m44;

    return c;
}

Mat44
mat44_minus(Mat44 a, Mat44 b)
{
    Mat44 c = mat44_zero();
    c.m11 = a.m11 - b.m11;
    c.m21 = a.m21 - b.m21;
    c.m31 = a.m31 - b.m31;
    c.m41 = a.m41 - b.m41;

    c.m12 = a.m12 - b.m12;
    c.m22 = a.m22 - b.m22;
    c.m32 = a.m32 - b.m32;
    c.m42 = a.m42 - b.m42;

    c.m13 = a.m13 - b.m13;
    c.m23 = a.m23 - b.m23;
    c.m33 = a.m33 - b.m33;
    c.m43 = a.m43 - b.m43;

    c.m14 = a.m14 - b.m14;
    c.m24 = a.m24 - b.m24;
    c.m34 = a.m34 - b.m34;
    c.m44 = a.m44 - b.m44;

    return c;
}

Mat44
mat44_multiply(Mat44 a, Mat44 b)
{
    Mat44 c = mat44_zero();
    c.m11 = a.m11 * b.m11 + a.m12 * b.m21 + a.m13 * b.m31 + a.m14 * b.m41;
    c.m21 = a.m21 * b.m11 + a.m22 * b.m21 + a.m23 * b.m31 + a.m24 * b.m41;
    c.m31 = a.m31 * b.m11 + a.m32 * b.m21 + a.m33 * b.m31 + a.m34 * b.m41;
    c.m41 = a.m41 * b.m11 + a.m42 * b.m21 + a.m43 * b.m31 + a.m44 * b.m41;

    c.m12 = a.m11 * b.m12 + a.m12 * b.m22 + a.m13 * b.m32 + a.m14 * b.m42;
    c.m22 = a.m21 * b.m12 + a.m22 * b.m22 + a.m23 * b.m32 + a.m24 * b.m42;
    c.m32 = a.m31 * b.m12 + a.m32 * b.m22 + a.m33 * b.m32 + a.m34 * b.m42;
    c.m42 = a.m41 * b.m12 + a.m42 * b.m22 + a.m43 * b.m32 + a.m44 * b.m42;

    c.m13 = a.m11 * b.m13 + a.m12 * b.m23 + a.m13 * b.m33 + a.m14 * b.m43;
    c.m23 = a.m21 * b.m13 + a.m22 * b.m23 + a.m23 * b.m33 + a.m24 * b.m43;
    c.m33 = a.m31 * b.m13 + a.m32 * b.m23 + a.m33 * b.m33 + a.m34 * b.m43;
    c.m43 = a.m41 * b.m13 + a.m42 * b.m23 + a.m43 * b.m33 + a.m44 * b.m43;

    c.m14 = a.m11 * b.m14 + a.m12 * b.m24 + a.m13 * b.m34 + a.m14 * b.m44;
    c.m24 = a.m21 * b.m14 + a.m22 * b.m24 + a.m23 * b.m34 + a.m24 * b.m44;
    c.m34 = a.m31 * b.m14 + a.m32 * b.m24 + a.m33 * b.m34 + a.m34 * b.m44;
    c.m44 = a.m41 * b.m14 + a.m42 * b.m24 + a.m43 * b.m34 + a.m44 * b.m44;

    return c;
}

Mat44 mat44_inverse(Mat44 a)
{
    MPP_UNIMPLEMENTED(mat44_inverse);
    MPP_UNUSED(a);
    return mat44_zero();
}

float mat44_determinant(Mat44 a)
{
    return a.m41 * a.m32 * a.m23 * a.m14 - a.m31 * a.m42 * a.m23 * a.m14 -
           a.m41 * a.m22 * a.m33 * a.m14 + a.m21 * a.m42 * a.m33 * a.m14 +
           a.m31 * a.m22 * a.m43 * a.m14 - a.m21 * a.m32 * a.m43 * a.m14 -
           a.m41 * a.m32 * a.m13 * a.m24 + a.m31 * a.m42 * a.m13 * a.m24 +
           a.m41 * a.m12 * a.m33 * a.m24 - a.m11 * a.m42 * a.m33 * a.m24 -
           a.m31 * a.m12 * a.m43 * a.m24 + a.m11 * a.m32 * a.m43 * a.m24 +
           a.m41 * a.m22 * a.m13 * a.m34 - a.m21 * a.m42 * a.m13 * a.m34 -
           a.m41 * a.m12 * a.m23 * a.m34 + a.m11 * a.m42 * a.m23 * a.m34 +
           a.m21 * a.m12 * a.m43 * a.m34 - a.m11 * a.m22 * a.m43 * a.m34 -
           a.m31 * a.m22 * a.m13 * a.m44 + a.m21 * a.m32 * a.m13 * a.m44 +
           a.m31 * a.m12 * a.m23 * a.m44 - a.m11 * a.m32 * a.m23 * a.m44 -
           a.m21 * a.m12 * a.m33 * a.m44 + a.m11 * a.m22 * a.m33 * a.m44;
}    



#endif /*MATHPLUSPLUS_IMPLEMENTATION*/
#endif /*MATHPLUSPLUS_H*/
