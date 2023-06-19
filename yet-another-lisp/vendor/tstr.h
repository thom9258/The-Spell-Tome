/*
SHORT DESCRIPTION:
tstring.h - simple textstring library written in ansi-c99.

ORIGINAL AUTHOR: Thomas Alexgaard Jensen (https://gitlab.com/Alexgaard)

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

FILOSOPHY:
A simple-to-use, powerful string library that makes working with strings
feel like you are using something like python or std::string in c++.

The implementation adheres to a set of constraints:
1. The library aims to fully replace "string.h"
2. The library makes it easy to create & modify strings
3. Supports compile-time constant tstrings
4. The library will not automatically call tstr_destroy on inputs
5. Support growable buffers for easy string construction.

Future work:

1. Support regex expressions
2. Support a string view using regex


CHANGELOG:
[2.1] Leveraged snprintf for c formatting and simplification of impl.
[2.0] Reformatted library to adhere to newly-defined filosophy
      Shorter name, tstring_s -> tstr
      Added declarations of tstrBuffer, the string construction buffer.
[1.1] Reformatted copy function.
      Added tstr_format() declaration.
      added todo list to complete library.
[1.0] Added core functionality.
[0.0] Initialized library.

TODO: Add error management system or atleast asserting on error.
TODO: change tstr_length to be a null terminator checker within maxlen bounds.

*/

#ifndef TSTR_H
#define TSTR_H

#include <stdint.h> /*uint32_t + uint8_t*/
#include <stdio.h>  /*NULL*/
#include <assert.h>
#include <stdarg.h>

#ifndef TSTR_NO_STDLIB
    #include <stdlib.h> /*malloc + realloc + free*/
    #define TSTR_MALLOC  malloc
    #define TSTR_REALLOC realloc
    #define TSTR_FREE    free
#endif

#ifndef TSTR_API
#define TSTR_API static inline
#endif

#define TSTR_BACKEND static

#ifndef TSTR_CHAR
#define TSTR_CHAR char
#endif

#ifndef TSTR_NEWLINE
#define TSTR_NEWLINE (TSTR_CHAR)'\n'
#endif

#ifndef TSTR_NULLTERM
#define TSTR_NULLTERM (TSTR_CHAR)'\0'
#endif

#define TSTR_UNSAFE 0
#define TSTR_INVALID -1

#define _TSTR_CHAR_TO_LOWER(C) \
	( C += (C >= 'A' && C <= 'Z') ? ('a'-'A') : 0 )

#define _TSTR_CHAR_TO_UPPER(C) \
	( C += (C >= 'a' && C <= 'z') ? -('a'-'A') : 0 )

typedef struct {
	TSTR_CHAR* c_str;
	int maxlen;
	char is_view;
}tstr;

typedef struct {
	TSTR_CHAR* data;
	int top;
	int max;
    float growth_rate;
}tstrBuffer;

TSTR_API char tstr_ok(tstr* _str);
TSTR_API void tstr_destroy(tstr* _str);
TSTR_API tstr tstr_n(TSTR_CHAR* _str, int _n);
TSTR_API tstr tstr_(TSTR_CHAR* _str);
TSTR_API tstr tstr_view(TSTR_CHAR* _str);
TSTR_API tstr tstr_fmt(TSTR_CHAR* _fmt, ...);
TSTR_API int tstr_length(tstr* _str);
TSTR_API tstr tstr_file(tstr* _path);
TSTR_API char tstr_equal(tstr* _a, tstr* _b);
TSTR_API char tstr_equalc(tstr* _a, char* _b);
TSTR_API tstr* tstr_copy(tstr* _src, tstr* _dst);
TSTR_API tstr* tstr_cut(tstr* _str, int _f, int _t);
TSTR_API tstr* tstr_split(tstr* _src, tstr* _lhs, tstr* _rhs, int _n);
TSTR_API int tstr_find(tstr* _str, tstr* _f);
TSTR_API int tstr_findlast(tstr* _str, tstr* _f);
TSTR_API tstr* tstr_add2end(tstr* _dst, tstr* _end);
TSTR_API tstr* tstr_concat(tstr* _str, tstr* _back);
TSTR_API tstr* tstr_to_upper(tstr* _str);
TSTR_API tstr* tstr_to_lower(tstr* _str);
TSTR_API tstr tstr_from_float(float _val);
TSTR_API tstr tstr_from_int(int _val);
TSTR_API float tstr_to_float(tstr* _str);
TSTR_API int tstr_to_int(tstr* _str);
TSTR_API uint64_t tstr_hash(tstr* _str);

/******************************************************************************/
#define TSTR_IMPLEMENTATION
#ifdef TSTR_IMPLEMENTATION

TSTR_BACKEND
int
_rawstr_findchar(const TSTR_CHAR* _start, char _char)
{
    int i = 0;
    if (_start == NULL)
        return TSTR_INVALID;
    while (_start[i] != _char)
        i++;
    return i;
}

TSTR_BACKEND
int
_rawstr_nfindchar(TSTR_CHAR* _start, int _n, char _char)
{
	int i = 0;
	if (_start == NULL || _n < 0)
		return TSTR_INVALID;
	while (_start[i] != _char && i < _n)
		i++;
	return i;
}

TSTR_BACKEND
void
_rawstr_ncopy(TSTR_CHAR* _src, TSTR_CHAR* _dst, int _n)
{
	int i;
	if (_src == NULL || _dst == NULL || _n < 1)
		return;
	for (i = 0; i < _n; i++)
		_dst[i] = _src[i];
}

TSTR_BACKEND
char
_rawstr_is_equal(TSTR_CHAR* _a, TSTR_CHAR* _b, int _n)
{
	int i;
	if (_a == NULL || _b == NULL)
		return 0;
    for (i = 0; i < _n; i++)
		if (_a[i] != _b[i])
			return 0;
	return 1;
}

TSTR_API
char
tstr_ok(tstr* _str)
{
	if (_str == NULL || _str->c_str == NULL)
		return 0;
	return 1;
}

TSTR_API
void
tstr_destroy(tstr* _str)
{
	if (_str == NULL || _str->is_view)
		return;
	if (_str->c_str != NULL)
		TSTR_FREE(_str->c_str);
	_str->c_str = NULL;
	_str->maxlen = 0;
	_str->is_view = 0;
}

TSTR_API
tstr
tstr_view(TSTR_CHAR* _str)
{
    return (tstr) {_str, _rawstr_findchar(_str, '\0'), 1};
}

TSTR_API
tstr
tstr_fmt(char* _fmt, ...)
{
    tstr out = {0};
    va_list va1;
    va_list va2;

    va_copy(va2, va1);
    va_start(va1, _fmt);
    out.maxlen = 1 + vsnprintf(NULL, 0, _fmt, va1);
    va_end(va1);   
    out.c_str = (TSTR_CHAR*)malloc(sizeof(char) * (out.maxlen));
    va_start(va2, _fmt);
    //vsnprintf(out.c_str, out.maxlen, _fmt, va2);
    vsprintf(out.c_str, _fmt, va2);
    //printf("FMT: max(%d), str(%d) [%s]\n", out.maxlen, tstr_length(&out), out.c_str);
    va_end(va2);   
    return out;
}

//TSTR_API
//tstr
//tstr_n(TSTR_CHAR* _str, int _n)
//{
//    tstr out = {0};
//    if (_n < 1)
//        return (tstr){0};
//    out.c_str = (TSTR_CHAR*)TSTR_MALLOC(sizeof(TSTR_CHAR) * _n);
//    if (out.c_str == NULL)
//        return out;
//    out.maxlen = _n;
//    out.is_view = 0;
//    if (_str == NULL)
//        return out;
//
//    _rawstr_ncopy(_str, out.c_str, out.maxlen);
//    out.c_str[out.maxlen-1] = TSTR_NULLTERM;
//    return out;
//}
//TSTR_API
//tstr
//tstr_(TSTR_CHAR* _str)
//{
//    int n;
//    if (_str == NULL)
//        return (tstr){0};
//    n = _rawstr_findchar(_str, TSTR_NULLTERM);
//    if (n == TSTR_INVALID)
//        return (tstr){0};
//    return tstr_n(_str, n+1);
//}
//TSTR_API
//tstr
//tstr_char(TSTR_CHAR _char)
//{
//    tstr out = {0};
//    out = tstr_n(NULL, 2);
//    if (!tstr_ok(&out))
//        return out;
//    out.c_str[0] = _char;
//    out.c_str[1] = TSTR_NULLTERM;
//    return out;
//}

TSTR_API
tstr
tstr_n(TSTR_CHAR* _str, int _n)
{
	return tstr_fmt("%.*s", _n, _str);
}

TSTR_API
tstr
tstr_(TSTR_CHAR* _str)
{
	return tstr_fmt("%s", _str);
}

TSTR_API
tstr
tstr_file(tstr* _path)
/*https://stackoverflow.com/questions/3747086/reading-the-whole-text-file-into-a-char-array-in-c
*/
{
	FILE* fp;
	int maxlen = 0;
	tstr out = {0};
	fp = fopen(_path->c_str, "rb");
	if (!fp)
		return (tstr) {0};

	fseek(fp, 0L, SEEK_END);
	maxlen = ftell(fp);
	rewind(fp);
	out = tstr_n(NULL, maxlen+1);
	if (fread(out.c_str, maxlen, 1, fp) != 1) {
		/*TODO: Do something when aborting*/
	}
	fclose(fp);
	return out;
}

TSTR_API
int
tstr_length(tstr* _str)
{
	int len;
	if (!tstr_ok(_str))
		return TSTR_INVALID;
	//if (_str->is_view)
    //return _rawstr_findchar(_str->c_str, TSTR_NULLTERM);

	len = _rawstr_nfindchar(_str->c_str, _str->maxlen, TSTR_NULLTERM);
	return len;
}

TSTR_API
char
tstr_equal(tstr* _a, tstr* _b)
{
	int a_len;
	int b_len;
	int min;
	int i;
	if (_a == _b)
		return 1;
	if (!tstr_ok(_a) || !tstr_ok(_b))
		return 0;

    //printf("a=%s\nb=%s\n", _a->c_str, _b->c_str);
	a_len = tstr_length(_a);
	b_len = tstr_length(_b);
    min = (a_len < b_len) ? a_len : b_len;
    //printf("alen=%d, blen=%d, min=%d\n", a_len, b_len, min);

    for (i = 0; i < min; i++) {
        //printf("comparing (%c) == (%c)\n", _a->c_str[i], _b->c_str[i]);
        if (_a->c_str[i] != _b->c_str[i])
            return 0;
    }
    return 1;
}

char
tstr_equalc(tstr* _a, char* _b)
{
    tstr b = tstr_view(_b);
    return tstr_equal(_a, &b);
}

TSTR_API
tstr*
tstr_copy(tstr* _src, tstr* _dst)
{
    tstr_destroy(_dst);
    *_dst = tstr_(_src->c_str);
    return _dst;
}

TSTR_API
tstr*
tstr_cut(tstr* _str, int _f, int _t)
{
	int copylen = 0;
	int str_len;
	if (!tstr_ok(_str))
		return NULL;
	str_len = tstr_length(_str);
	if (_f < 0 || _t > str_len || _f > _t)
		return NULL;
	copylen = str_len - _t;
	_rawstr_ncopy(_str->c_str + _t, _str->c_str + _f, copylen);
	_str->c_str[_f + copylen] = TSTR_NULLTERM;
	return _str;
}

TSTR_API
tstr*
tstr_split(tstr* _src, tstr* _lhs, tstr* _rhs, int _split)
{
	int src_len = 0;
	if (!tstr_ok(_src) || _split < 1)
		return NULL;
	src_len = tstr_length(_src);
	if (_split > src_len)
		return NULL;

	tstr_destroy(_lhs);
	*_lhs = tstr_n(_src->c_str, _split);
	tstr_destroy(_rhs);
	//*_rhs = tstr_n(_src->c_str, src_len - _split);
	*_rhs = tstr_(_src->c_str + _split);
	return _src;
}

TSTR_API
int
tstr_find(tstr* _str, tstr* _f)
{
	int i;
	int str_len;
	int f_len;
	if (!tstr_ok(_str) || !tstr_ok(_f))
		return TSTR_INVALID;

	str_len = tstr_length(_str);
	f_len = tstr_length(_f);
	if (str_len < 1 || f_len < 1)
		return TSTR_INVALID;

	for (i = 0; i < str_len - f_len + 1; i++) {
		if (_rawstr_is_equal(_str->c_str + i, _f->c_str, f_len)) {
			return i;
		}
	}
	return TSTR_INVALID;
}

TSTR_API
int
tstr_findlast(tstr* _str, tstr* _f)
{
	int i;
	int str_len;
	int f_len;
	if (!tstr_ok(_str) || !tstr_ok(_f))
		return TSTR_INVALID;

	str_len = tstr_length(_str);
	f_len = tstr_length(_f);
	if (str_len < 1 || f_len < 1)
		return TSTR_INVALID;

	for (i = str_len - f_len; i > 0; i--) {
		if (_rawstr_is_equal(_str->c_str + i, _f->c_str, f_len)) {
			return i;
		}
	}
	return TSTR_INVALID;
}

TSTR_API
tstr*
tstr_add2end(tstr* _dst, tstr* _end)
{
    tstr newstr;
    //printf("trying to create str [%s] + [%s]\n", _dst->c_str, _end->c_str);
    if (!tstr_ok(_dst) && !tstr_ok(_end))
        newstr = tstr_("");
    else if (tstr_ok(_dst) && !tstr_ok(_end))
        newstr = tstr_(_dst->c_str);
    else if (!tstr_ok(_dst) && tstr_ok(_end))
        newstr = tstr_(_end->c_str);
    else
        newstr = tstr_fmt("%s%s", _dst->c_str, _end->c_str);

    tstr_destroy(_dst);
    *_dst = newstr;
    return _dst;
}

TSTR_API
tstr*
tstr_concat(tstr* _str, tstr* _back)
{
	int str_len;
	int back_len;
	tstr combined = {0};
	if (!tstr_ok(_back))
		return NULL;
	if (!tstr_ok(_str))
		return tstr_copy(_back, _str);

	str_len = tstr_length(_str);
	back_len = tstr_length(_back);
	if (str_len == TSTR_INVALID || back_len == TSTR_INVALID)
		return NULL;

	combined = tstr_n(NULL, str_len + back_len + 1);
	if (!tstr_ok(&combined))
		return NULL;

	_rawstr_ncopy(_str->c_str, combined.c_str, str_len);
	_rawstr_ncopy(_back->c_str, combined.c_str + str_len, back_len);
	tstr_destroy(_str);
	*_str = combined;
	return _str;
}

TSTR_API
tstr*
tstr_to_upper(tstr* _str)
{
	int i;
	if (!tstr_ok(_str))
		return NULL;
	for (i = 0; i < _str->maxlen; i++)
		_TSTR_CHAR_TO_UPPER(_str->c_str[i]);
	return _str;
}

TSTR_API
tstr*
tstr_to_lower(tstr* _str)
{
	int i;
	if (!tstr_ok(_str))
		return NULL;
	for (i = 0; i < _str->maxlen; i++)
		_TSTR_CHAR_TO_LOWER(_str->c_str[i]);
	return _str;
}

TSTR_API
uint64_t
tstr_hash(tstr* _str)
/*Refrence, "sdbm" hash.*/
{
	uint32_t c;
	uint64_t out = 0;
	if (!tstr_ok(_str))
		return 0;
	while ((c = *_str->c_str++) != '\0')
		out = c + (out << 6) + (out << 16) - out;
	return out;
}

TSTR_API
tstr
tstr_from_int(int _val)
{
    return tstr_fmt("%d", _val);
}

TSTR_API
tstr
tstr_from_float(float _val)
{
    return tstr_fmt("%f", _val);
}

TSTR_API
int
tstr_to_int(tstr* _str)
{
	if (!tstr_ok(_str))
        return 0;
    return atoi(_str->c_str);
}

TSTR_API
float
tstr_to_float(tstr* _str)
{
	if (!tstr_ok(_str))
        return 0.0f;
    return atof(_str->c_str);
}

#endif /*TSTR_IMPLEMENTATION*/
#endif /*TSTR_H*/
