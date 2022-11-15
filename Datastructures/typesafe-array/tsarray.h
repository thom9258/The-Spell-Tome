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
TODO: Add doxygen documentation.
TODO: Add sorting function, requires "t_operator_largest"

[0.3] Added namespace changeability.
[0.2] Prefixed CAT with _ to hide definition.
      Implemented call safety for t_operator_print
      made defines for better readability of function names.
      added copy operator
      implemented quality of life functions:
          resize()
          concatenate()
          duplicate()
          peek()
          push_front()
          remove()
          insert()
      added todos for functions:
          reverse()
          sort()
      added comments where nessecary
[0.1] Added core functionality and primary overloads
[0.0] Initialized library.
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifndef _C_MACRO_MAGIC_
#define _CAT_(A,B) A##B
#define _CAT(A,B) _CAT_(A, B)
#endif /*_C_MACRO_MAGIC_*/

/*required type definition. 
 * If none given, default to int in order to not emit errors*/
#ifndef t_type
#define t_type int
#endif

/*Sane default constructors*/
#ifndef t_operator_init
#define t_operator_init() (t_type) {0}
#endif

#ifndef t_operator_destroy
#define t_operator_destroy(t)
#endif

#ifndef t_operator_copy
#define t_operator_copy(t) t
#endif

#ifndef t_operator_print
#define t_operator_print(t)                                                    \
    assert(!"#define t_operator_print is used but"                             \
            " not defined for specified type")
#endif

#ifndef t_operator_largest
#define t_operator_largest(a, b) ( (a < b) ? b : a )
#endif

#ifndef TSARRAY_NAME_OVERRIDE
#define arr_t_name _CAT(arr_, t_type)
#endif

/*defines for better readability of function names*/
#define arr_t_initn           _CAT(arr_t_name, _initn)
#define arr_t_destroy         _CAT(arr_t_name, _destroy)
#define arr_t_destroy_members _CAT(arr_t_name, _destroy_members)
#define arr_t_len             _CAT(arr_t_name, _len)
#define arr_t_resize          _CAT(arr_t_name, _resize)
#define arr_t_push            _CAT(arr_t_name, _push)
#define arr_t_push_front      _CAT(arr_t_name, _push_front)
#define arr_t_insert          _CAT(arr_t_name, _insert)
/* TODO: implement
#define arr_t_reverse         _CAT(arr_t_name, _reverse)
#define arr_t_sort            _CAT(arr_t_name, _sort)
 * */
#define arr_t_peek            _CAT(arr_t_name, _peek)
#define arr_t_get             _CAT(arr_t_name, _get)
#define arr_t_pop             _CAT(arr_t_name, _pop)
#define arr_t_pop_front       _CAT(arr_t_name, _pop_front)
#define arr_t_remove          _CAT(arr_t_name, _remove)
#define arr_t_print           _CAT(arr_t_name, _print)
#define arr_t_duplicate       _CAT(arr_t_name, _duplicate)
#define arr_t_concatenate     _CAT(arr_t_name, _concatenate)
#define arr_t_struct          arr_t_name

typedef struct {
    t_type* members;
    int count;
    int max;
    float growth_rate;
}arr_t_struct;

arr_t_struct
arr_t_initn(int _n)
{
    if (_n < 1) _n = 3;
    arr_t_struct out = {0};
    out.members = (t_type*)malloc(sizeof(t_type) * _n);
    if (out.members == NULL)
        return out;
    out.count = 0;
    out.max = _n;
    out.growth_rate = 1.5f;
    return out;
}

void
arr_t_destroy(arr_t_struct* _arr)
{
    if (_arr == NULL || _arr->members == NULL)
        return;
    free(_arr->members);
    *_arr = (arr_t_struct) {0};
}

void
arr_t_destroy_members(arr_t_struct* _arr)
{
    int i;
    if (_arr == NULL || _arr->members == NULL)
        return;
    for (i = 0; i < _arr->count; i++) {
        t_operator_destroy(_arr->members[i]);
    }
}

int
arr_t_len(arr_t_struct* _arr)
{
    if (_arr == NULL || _arr->members == NULL)
        return -1;
    return _arr->count;
}

arr_t_struct*
arr_t_resize(arr_t_struct* _dst, int _n)
{
    if (_dst == NULL)
        return NULL;
    if (_dst->members == NULL) {
        *_dst = arr_t_initn(_n);
        return _dst;
    }
    _dst->members = (t_type*)realloc(_dst->members, _n * sizeof(t_type));
    if (_dst->members == NULL)
        return NULL;
    _dst->max = _n;
    return _dst;
}

t_type*
arr_t_push(arr_t_struct* _dst, t_type _push)
{
    int n = 0;
    if (_dst == NULL)
        return NULL;
    if (_dst->members == NULL) {
        *_dst = arr_t_initn(3);
    }
    if (_dst->count >= _dst->max) {
        n = 1 + (_dst->max * _dst->growth_rate);
        arr_t_resize(_dst, n);
    }
    _dst->members[_dst->count] = _push;
    return &_dst->members[_dst->count++];
}

t_type*
arr_t_push_front(arr_t_struct* _dst, t_type _push)
{
    int i;
    int n = 0;
    if (_dst == NULL || _dst->members == NULL)
        return NULL;
    if (_dst->count >= _dst->max) {
        n = 1 + (_dst->max * _dst->growth_rate);
        arr_t_resize(_dst, n);
    }
    for (i = _dst->count; i > 0; i--)
        _dst->members[i] = _dst->members[i-1];
    _dst->members[0] = _push;
    return &_dst->members[0];
}

t_type*
arr_t_insert(arr_t_struct* _dst, int _idx, t_type _push)
{
    int i;
    int n = 0;
    if (_dst == NULL || _dst->members == NULL)
        return NULL;
    if (_dst->count >= _dst->max) {
        n = 1 + (_dst->max * _dst->growth_rate);
        arr_t_resize(_dst, n);
    }
    for (i = _dst->count; i > _idx; i--)
        _dst->members[i] = _dst->members[i-1];
    _dst->members[_idx] = _push;
    return &_dst->members[_idx];
}

t_type*
arr_t_peek(arr_t_struct* _arr, int _idx)
{
    if (_arr == NULL || _arr->members == NULL || _arr->count < 1)
        return NULL;
    if (_idx > _arr->count)
        return NULL;
    if (_idx < -_arr->count)
        return NULL;
    if (_idx < 0)
        _idx += _arr->count;

    return &_arr->members[_idx];
}

t_type
arr_t_get(arr_t_struct* _arr, int _idx)
{
    int i;
    t_type out = (t_type){0};
    if (_arr == NULL || _arr->members == NULL || _arr->count < 1)
        return t_operator_init();

    if (_idx > _arr->count)
        return t_operator_init();
    if (_idx < -_arr->count)
        return t_operator_init();
    if (_idx < 0)
        _idx += _arr->count;

    out = _arr->members[_idx];
    for (i = _idx; i < _arr->count-1; i++)
        _arr->members[i] = _arr->members[i+1];
    _arr->count--;
    return out;
}

t_type
arr_t_pop(arr_t_struct* _arr)
{
    return arr_t_get(_arr, -1);
}

t_type
arr_t_pop_front(arr_t_struct* _arr)
{
    return arr_t_get(_arr, 0);
}

void
arr_t_remove(arr_t_struct* _arr, int _idx)
{
    int i;
    if (_arr == NULL || _arr->members == NULL || _arr->count < 1)
        return;
    if (_idx > _arr->count || _idx < -_arr->count)
        return;
    if (_idx < 0)
        _idx += _arr->count;

    t_operator_destroy(_arr->members[_idx]);
    for (i = _idx; i < _arr->count-1; i++)
        _arr->members[i] = _arr->members[i+1];
    _arr->count--;
}

void
arr_t_print(arr_t_struct* _arr)
{
    int i;
    if (_arr == NULL || _arr->members == NULL)
        return;
    for (i = 0; i < _arr->count; i++) {
        t_operator_print(_arr->members[i]);
    }
}

arr_t_struct
arr_t_duplicate(arr_t_struct* _arr)
{
    int i;
    arr_t_struct dub = {0};
    if (_arr == NULL || _arr->members == NULL)
        return dub;
    
    dub = arr_t_initn(_arr->count);
    for (i = 0; i < _arr->count; i++)
        arr_t_push(&dub, t_operator_copy(_arr->members[i]));

    return dub;
}

arr_t_struct*
arr_t_concatenate(arr_t_struct* _dst, arr_t_struct* _src)
{
    int i;
    if (_dst == NULL || _dst->members == NULL)
        return _src;
    if (_src == NULL || _src->members == NULL)
        return _dst;

    for (i = 0; i < _src->count; i++)
        arr_t_push(_dst, _src->members[i]);
    return _dst;
}

#undef t_type
#undef arr_t_name
#undef t_operator_init
#undef t_operator_destroy
#undef t_operator_copy
#undef t_operator_print
#undef TSARRAY_NAME_OVERRIDE
