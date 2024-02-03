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

#ifndef MPP_ASSERT
#define MPP_ASSERT assert
#endif
#ifndef MPP_UNIMPLEMENTED
#define MPP_UNIMPLEMENTED(fn) MPP_ASSERT(0 && #fn "Is not implemented!")
#endif
#ifndef MPP_UNUSED
#define MPP_UNUSED(x) (void)x
#endif

/*General Defines*/
#ifndef MPP_PFMT
#define MPP_PFMT "% 16f"
#endif

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
            float x, y;
        };
        float v[2];
    };
}Vec2;

#define VEC2_PSTRING \
    "V2[ " PFMT ", " PFMT " ]"
#define VEC2_EXPAND(v) \
    v.x, v.y

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
Vec2 vec2_zero(void);
Vec2 vec2(float x, float y);
char vec2_equal(Vec2 a, Vec2 b);
Vec2 vec2_plus(Vec2 a, Vec2 b);
Vec2 vec2_minus(Vec2 a, Vec2 b);
Vec2 vec2_multiply(Vec2 a, Vec2 b);

Vec3 vec3_zero(void);
Vec3 vec3(float x, float y, float z);
char vec3_equal(Vec3 a, Vec3 b);
Vec3 vec3_plus(Vec3 a, Vec3 b);
Vec3 vec3_minus(Vec3 a, Vec3 b);
Vec3 vec3_multiply(Vec3 a, Vec3 b);

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

Vec2
vec2_zero(void)
{
    return vec2(0, 0);
}

Vec2
vec2(float x, float y)
{
    Vec2 v = {0};
    v.x = x; v.y = y;
    return v;
}

char
vec2_equal(Vec2 a, Vec2 b)
{
    if (a.x == b.x && a.y == b.y)
        return 1;
    return 0;
}

Vec2
vec2_plus(Vec2 a, Vec2 b)
{
    return vec2(a.x + b.x, a.y + b.y);
}

Vec2
vec2_minus(Vec2 a, Vec2 b)
{
    return vec2(a.x - b.x, a.y - b.y);
}

Vec2
vec2_multiply(Vec2 a, Vec2 b)
{
    return vec2(a.x * b.x, a.y * b.y);
}

Vec3
vec3_zero(void)
{
    return vec3(0, 0, 0);
}

Vec3
vec3(float x, float y, float z)
{
    Vec3 v = {0};
    v.x = x; v.y = y; v.z = z;
    return v;
}

char
vec3_equal(Vec3 a, Vec3 b)
{
    if (a.x == b.x && a.y == b.y && a.z == b.z)
        return 1;
    return 0;
}

Vec3
vec3_plus(Vec3 a, Vec3 b)
{
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3
vec3_minus(Vec3 a, Vec3 b)
{
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3
vec3_multiply(Vec3 a, Vec3 b)
{
    return vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

#endif /*MATHPLUSPLUS_IMPLEMENTATION*/
#endif /*MATHPLUSPLUS_H*/
