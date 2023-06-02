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
[2.0] Reformatted library to adhere to newly-defined filosophy
      Shorter name, tstring_s -> tstr
[1.1] Reformatted copy function.
      Added tstr_format() declaration.
      added todo list to complete library.
[1.0] Added core functionality.
[0.0] Initialized library.

*/

#ifndef TSTR_H
#define TSTR_H

#include <stdint.h> /*uint32_t + uint8_t*/
#include <stdio.h>  /*NULL*/
#include <stdarg.h>  /*va_arg*/
#include <assert.h>

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

#define tstr_const(STR) (tstr) {STR, sizeof(STR)/sizeof(STR[0]), 1}

typedef struct {
	TSTR_CHAR* c_str;
	int maxlen;
	char is_const;
}tstr;

TSTR_API char tstr_ok(tstr* _str);
TSTR_API void tstr_destroy(tstr* _str);
TSTR_API tstr tstr_n(TSTR_CHAR* _str, int _n);
TSTR_API tstr tstr_(TSTR_CHAR* _str);
TSTR_API tstr tstr_fmt(TSTR_CHAR* _fmt, ...);
TSTR_API tstr tstr_char(TSTR_CHAR _char);
TSTR_API tstr tstr_file(tstr* _path);
TSTR_API char tstr_equal(tstr* _a, tstr* _b);
TSTR_API tstr* tstr_substr(tstr* _src, tstr* _dst, int _f, int _n);
TSTR_API tstr* tstr_duplicate(tstr* _src, tstr* _dst);
TSTR_API tstr* tstr_cut(tstr* _str, int _f, int _t);
TSTR_API tstr* tstr_split(tstr* _src, tstr* _lhs, tstr* _rhs, int _n);
TSTR_API int tstr_find(tstr* _str, tstr* _f); 
TSTR_API int tstr_findlast(tstr* _str, tstr* _f);
TSTR_API tstr* tstr_concat(tstr* _str, tstr* _back);
TSTR_API tstr* tstr_concatva(tstr* _str, int _n, ...);
TSTR_API tstr* tstr_to_upper(tstr* _str);
TSTR_API tstr* tstr_to_lower(tstr* _str);
TSTR_API uint64_t tstr_hash(tstr* _str);
TSTR_API tstr* tstr_from_float(tstr* _str, float _val);
TSTR_API tstr* tstr_from_int(tstr* _str, int _val);
TSTR_API float tstr_to_float(tstr* _str);
TSTR_API int tstr_to_int(tstr* _str);

/******************************************************************************/
#define TSTR_IMPLEMENTATION
#ifdef TSTR_IMPLEMENTATION

TSTR_BACKEND
int
_rawstr_findchar(TSTR_CHAR* _start, char _char)
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
	if (_str == NULL || _str->is_const)
		return;
	if (_str->c_str != NULL)
		TSTR_FREE(_str->c_str);
	_str->c_str = NULL;
	_str->maxlen = 0;
	_str->is_const = 0;
}

TSTR_API
tstr
tstr_n(TSTR_CHAR* _str, int _n)
{
	tstr out = {0};
	if (_n < 1)
		return (tstr){0};
	out.c_str = (TSTR_CHAR*)TSTR_MALLOC(sizeof(TSTR_CHAR) * _n);
	if (out.c_str == NULL)
		return out;
    out.maxlen = _n;
	out.is_const = 0;
    if (_str == NULL)
        return out;

    _rawstr_ncopy(_str, out.c_str, out.maxlen);
	out.c_str[out.maxlen-1] = TSTR_NULLTERM;
	return out;
}

TSTR_API
tstr
tstr_(TSTR_CHAR* _str)
{
	int n;
	if (_str == NULL)
		return (tstr){0};
	n = _rawstr_findchar(_str, TSTR_NULLTERM);
	if (n == TSTR_INVALID)
		return (tstr){0};
	return tstr_n(_str, n+1);
}

TSTR_API
tstr
tstr_fmt(TSTR_CHAR* _fmt, ...)
{
    tstr out = {0};
    va_list argptr;
    va_start(argptr, _fmt);
    out.maxlen = sprintf(NULL, _fmt, argptr);
    out.c_str = (TSTR_CHAR*)malloc(sizeof(TSTR_CHAR) * (out.maxlen+1));
    sprintf(out.c_str, _fmt, argptr);
    va_end(argptr);   
    out.c_str[out.maxlen] = '\n';
    return out;
}

TSTR_API
tstr
tstr_char(TSTR_CHAR _char)
{
	tstr out = {0};
	out = tstr_n(NULL, 2);
	if (!tstr_ok(&out))
		return out;
	out.c_str[0] = _char;
	out.c_str[1] = TSTR_NULLTERM;
	return out;
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
	//if (_str->is_const)
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

	a_len = tstr_length(_a);
	b_len = tstr_length(_b);

    min = (a_len < b_len) ? a_len : b_len;
    //printf("alen=%d, blen=%d\n", a_len, b_len);
    for (i = 0; i < min; i++) {
        //printf("comparing (%c) == (%c)\n", _a->c_str[i], _b->c_str[i]);
        if (_a->c_str[i] != _b->c_str[i])
            return 0;
    }
    return 1;
}

TSTR_API
tstr*
tstr_substr(tstr* _src, tstr* _dst, int _f, int _n)
/*TODO: copy cannot create a copy of a const tstring*/
{
	int src_len;
	if (!tstr_ok(_src))
		return NULL;

	src_len = tstr_length(_src);
	if (_n < 1 ||  _n > src_len) {
		_n = src_len;
		return NULL;
	}

	tstr_destroy(_dst);
	*_dst = tstr_n(NULL, _n + 1);
	if (!tstr_ok(_dst))
		return NULL;

	_rawstr_ncopy(_src->c_str + _f, _dst->c_str, _n);
	_dst->c_str[_n] = TSTR_NULLTERM;
	return _dst;
}

TSTR_API
tstr*
tstr_duplicate(tstr* _src, tstr* _dst)
{
	return tstr_substr(_src, _dst, 0, 0);
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
	tstr_destroy(_rhs);
	tstr_substr(_src, _lhs, 0, _split);
	tstr_substr(_src, _rhs, _split, src_len - _split);
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
tstr_concat(tstr* _str, tstr* _back)
{
	int str_len;
	int back_len;
	tstr combined = {0};
	if (!tstr_ok(_back))
		return NULL;
	if (!tstr_ok(_str))
		return tstr_duplicate(_back, _str);

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
tstr_concatva(tstr* _str, int _n, ...)
{
	int i;
    tstr* tmp;
	va_list ap;
	va_start(ap, _n);
	if (!tstr_ok(_str))
		*_str = tstr_n(NULL, 1);
	for (i = 0; i < _n; i++) {
        tmp = va_arg(ap, tstr*);
        //printf("%d) [%s]\n", i, tmp->c_str);
		tstr_concat(_str, tmp);
    }
	va_end(ap);
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
tstr*
tstr_from_int(tstr* _str, int _val)
{
    char buf[32] = {0};
	if (tstr_ok(_str))
		tstr_destroy(_str);
    sprintf(buf, "%d", _val);
    *_str = tstr_(buf);
	return _str;
}

TSTR_API
tstr*
tstr_from_float(tstr* _str, float _val)
{
    char buf[32] = {0};
	if (tstr_ok(_str))
		tstr_destroy(_str);
    sprintf(buf, "%f", _val);
    *_str = tstr_(buf);
	return _str;
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


#endif /*TSTR_IMPLEMENTATION*/
#endif /*TSTR_H*/
