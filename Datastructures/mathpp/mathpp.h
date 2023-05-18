/*
SHORT DESCRIPTION:
tsarray.h - typesafe array library written in ansi-c99.

ORIGINAL AUTHOR: Thomas Alexgaard Jensen (https://gitlab.com/Alexgaard)

   __________________________
  :##########################:
  *##########################*
           .:######:.
          .:########:.
         .:##########:.
        .:##^:####:^##:.
       .:##  :####:  ##:.
      .:##   :####:   ##:.
     .:##    :####:    ##:.
    .:##     :####:     ##:.
   .:##      :####:      ##:.
  .:#/      .:####:.      \#:.


LICENSE:
Copyright (c) <Thomas Alexgaard Jensen>
This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from
the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software in
   a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.

For more information, please refer to
<https://choosealicense.com/licenses/zlib/>

CHANGELOG:

[0.1] Added Mat44 datatype
[0.0] Initialized library.

*/

#ifndef MATHPLUSPLUS_H
#define MATHPLUSPLUS_H

#include <assert.h>
#include <math.h>

#define UNIMPLEMENTED(fn) assert(0 && #fn "Is not implemented!")
#define UNUSED(x) (void)x

#ifndef MPP_ASSERT
#define MPP_ASSERT assert
#endif

/*General Defines*/
#define PFMT "% 16f"

typedef struct {
    float w, x, y, z;
}Quat;

#define QUAT_PSTRING \
    "Q[ " PFMT ", " PFMT ", " PFMT ", " PFMT " ]"
#define QUAT_EXPAND(q) \
    q.w, q.x, q.y, q.z

typedef struct {
    union {
        struct {    
            float x, y, z;
        };
        float v[3];
    };
}Vec3;

#define VEC3_PSTRING \
    "V3[ " PFMT ", " PFMT ", " PFMT " ]"
#define VEC3_EXPAND(v) \
    v.x, v.y, v.z

typedef struct {
    union {
        /*Named accessors*/
        struct {    
            float m11, m12, m13, m14;
            float m21, m22, m23, m24;
            float m31, m32, m33, m34;
            float m41, m42, m43, m44;
        };
        /*Iterateable accessors*/
        float m[4][4];
    };
}Mat44;

#define MAT44_PSTRING \
    "M44| " PFMT ", " PFMT ", " PFMT ", " PFMT " |\n" \
    "   | " PFMT ", " PFMT ", " PFMT ", " PFMT " |\n" \
    "   | " PFMT ", " PFMT ", " PFMT ", " PFMT " |\n" \
    "   | " PFMT ", " PFMT ", " PFMT ", " PFMT " |\n"
#define MAT44_EXPAND(mat44) \
    mat44.m11, mat44.m12, mat44.m13, mat44.m14, \
    mat44.m21, mat44.m22, mat44.m23, mat44.m24, \
    mat44.m31, mat44.m32, mat44.m33, mat44.m34, \
    mat44.m41, mat44.m42, mat44.m43, mat44.m44

typedef struct {
    Vec3 pos;
    Quat rot;
}Transform;

/*General purpose math*/
float absf(float a);
float minf(float a, float b);
float maxf(float a, float b);
float clampf(float min, float max, float val);

/*Quaternions*/
Quat quat(float w, float x, float y, float z);

/*Vectors*/
Vec3 vec3_zero(void);
Vec3 vec3(float x, float y, float z);
char vec3_equal(Vec3 a, Vec3 b);
Vec3 vec3_plus(Vec3 a, Vec3 b);
Vec3 vec3_minus(Vec3 a, Vec3 b);
Vec3 vec3_multiply(Vec3 a, Vec3 b);

/*Matrices*/
Mat44 mat44_zero(void);
Mat44 mat44(float v11, float v12, float v13, float v14,
            float v21, float v22, float v23, float v24,
            float v31, float v32, float v33, float v34,
            float v41, float v42, float v43, float v44);
void mat44_setrow(Mat44* a, int row, float v1, float v2, float v3, float v4);
Mat44 mat44_identity(void);
Mat44 mat44_transpose(Mat44 a);
char mat44_equal(Mat44 a, Mat44 b);
Mat44 mat44_plus(Mat44 a, Mat44 b);
Mat44 mat44_minus(Mat44 a, Mat44 b);
Mat44 mat44_multiply(Mat44 a, Mat44 b);

/*Transformations*/
void transform_translate(Transform* dst, Vec3 v);
void transform_rotate(Transform* dst, Quat q);

/*Conversions*/
Mat44 transform2mat44(Transform t);
Transform mat442transform(Mat44 m);
Vec3 quat2euler(Quat q);
Quat euler2quat(Vec3 e);

/********************************************************************************/
/********************************************************************************/
#ifdef MATHPLUSPLUS_IMPLEMENTATION

float
absf(float a)
{
    return (a > 0) ? a : -a;
}

float
minf(float a, float b)
{
    return (a < b) ? a : b;
}

float
maxf(float a, float b)
{
    return (a < b) ? b : a;
}

float
clampf(float min, float max, float val)
{
    return minf(maxf(val, min), max);
}

Mat44
mat44_zero(void)
{
    return (Mat44) {0};
}

Mat44
mat44(float v11, float v12, float v13, float v14,
      float v21, float v22, float v23, float v24,
      float v31, float v32, float v33, float v34,
      float v41, float v42, float v43, float v44)
{
    Mat44 c = mat44_zero();
    c.m11 =  v11;
    c.m21 =  v21;
    c.m31 =  v31;
    c.m41 =  v41;
    c.m12 =  v12;
    c.m22 =  v22;
    c.m32 =  v32;
    c.m42 =  v42;
    c.m13 =  v13;
    c.m23 =  v23;
    c.m33 =  v33;
    c.m43 =  v43;
    c.m14 =  v14;
    c.m24 =  v24;
    c.m34 =  v34;
    c.m44 =  v44;
    return c;
}

void
mat44_setrow(Mat44* a, int row, float v1, float v2, float v3, float v4)
{
    a->m[row][0] = v1;
    a->m[row][1] = v2;
    a->m[row][2] = v3;
    a->m[row][3] = v4;
}

Mat44
mat44_transpose(Mat44 a)
{
    UNUSED(a);
    UNIMPLEMENTED(mat44_transpose);
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

#endif /*MATHPLUSPLUS_IMPLEMENTATION*/
#endif /*MATHPLUSPLUS_H*/
