/*
SHORT DESCRIPTION:
tstring.h - textstring library written in ansi-c99.
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
[1.1] Reformatted copy function.
      Added tstring_format() declaration.
      added todo list to complete library.
[1.0] Added core functionality.
[0.0] Initialized library.

*/

/*TODO: implement these functions
TSTRING_API
tstring_s*
tstring_copy_fn(tstring_s* _src, tstring_s* _dst, int _f, int _n);

TSTRING_API
int
tstring_find_char_fn(tstring_s* _str, TSTRING_CHAR _char);

TSTRING_API
int
tstring_find_fn(tstring_s* _str, tstring_s* _f);

TSTRING_API
tstring_s*
tstring_search_replace(tstring_s* _str, tstring_s* _old, tstring_s* _new);

TSTRING_API
tstring_s*
tstring_search_replace_fn(tstring_s* _str, tstring_s* _old, tstring_s* _new, int _f, int _n);
*/

#ifndef TSTRING_H
#define TSTRING_H

#include <stdint.h> /*uint32_t + uint8_t*/
#include <stdio.h>  /*NULL*/

#ifndef TSTRING_NO_STDLIB
    #include <stdlib.h> /*malloc + realloc + free*/
    #define TSTRING_MALLOC  malloc
    #define TSTRING_REALLOC realloc
    #define TSTRING_FREE    free
#endif

#ifndef TSTRING_API
#define TSTRING_API static inline
#endif

#define TSTRING_BACKEND static

#ifndef TSTRING_CHAR
#define TSTRING_CHAR char
#endif

#ifndef TSTRING_NEWLINE
#define TSTRING_NEWLINE (TSTRING_CHAR)'\n'
#endif

#ifndef TSTRING_NULLTERM
#define TSTRING_NULLTERM (TSTRING_CHAR)'\0'
#endif

#define TSTRING_UNSAFE 0
#define TSTRING_INVALID -1

#define _TSTRING_CHAR_TO_LOWER(C) \
	( C += (C >= 'A' && C <= 'Z') ? ('a'-'A') : 0 )

#define _TSTRING_CHAR_TO_UPPER(C) \
	( C += (C >= 'a' && C <= 'z') ? -('a'-'A') : 0 )

#define tstring_const(STR) (tstring_s) {STR, sizeof(STR)/sizeof(STR[0]), 1}

typedef struct {
	TSTRING_CHAR* c_str;
	int maxlen;
	char is_const;
}tstring_s;

TSTRING_API
char
tstring_invalid(tstring_s* _str);

TSTRING_API
void
tstring_destroy(tstring_s* _str);

TSTRING_API
tstring_s
tstring_n(TSTRING_CHAR* _str, int _n);

TSTRING_API
tstring_s
tstring(TSTRING_CHAR* _str);

TSTRING_API
tstring_s
tstring_char(TSTRING_CHAR _char);

TSTRING_API
tstring_s
tstring_load_FILE(tstring_s* _path);

TSTRING_API
tstring_s
tstring_format(TSTRING_CHAR* _str, ...);

TSTRING_API
char
tstring_invalid(tstring_s* _str);

TSTRING_API
char
tstring_equal(tstring_s* _a, tstring_s* _b);

TSTRING_API
tstring_s*
tstring_copy(tstring_s* _src, tstring_s* _dst);

TSTRING_API
tstring_s*
tstring_cut(tstring_s* _str, int _f, int _t);

TSTRING_API
tstring_s*
tstring_split(tstring_s* _src, tstring_s* _lhs, tstring_s* _rhs, int _n);

TSTRING_API
int
tstring_find_char(tstring_s* _str, TSTRING_CHAR _char);

TSTRING_API
int
tstring_findlast_char(tstring_s* _str, TSTRING_CHAR _char);

TSTRING_API
int
tstring_find(tstring_s* _str, tstring_s* _f);

TSTRING_API
int
tstring_findlast(tstring_s* _str, tstring_s* _f);

TSTRING_API
tstring_s
tstring_combine(tstring_s* _a, tstring_s* _b);

TSTRING_API
tstring_s*
tstring_add_back(tstring_s* _str, tstring_s* _back);

TSTRING_API
tstring_s*
tstring_add_back_va(tstring_s* _str, int _n, ...);

TSTRING_API
tstring_s*
tstring_to_upper(tstring_s* _str);

TSTRING_API
tstring_s*
tstring_to_lower(tstring_s* _str);

TSTRING_API
uint64_t
tstring_hash(tstring_s* _str);

/******************************************************************************/
#ifdef TSTRING_IMPLEMENTATION

TSTRING_BACKEND
int
_rawstr_findchar(TSTRING_CHAR* _start, char _char)
{
	int i = 0;
	if (_start == NULL)
		return TSTRING_INVALID;
	while (_start[i] != _char)
		i++;
	return i;
}

TSTRING_BACKEND
int
_rawstr_nfindchar(TSTRING_CHAR* _start, int _n, char _char)
{
	int i = 0;
	if (_start == NULL || _n < 0)
		return TSTRING_INVALID;
	while (_start[i] != _char && i < _n)
		i++;
	return i;
}

TSTRING_BACKEND
int
_rawstr_nfindlastchar(TSTRING_CHAR* _start, int _n, char _char)
{
	int i = _n;
	if (_start == NULL || _n < 0)
		return TSTRING_INVALID;
	while (i >= 0 && _start[i] != _char)
		i--;
	return i;
}

TSTRING_BACKEND
void
_rawstr_ncopy(TSTRING_CHAR* _src, TSTRING_CHAR* _dst, int _n)
{
	int i;
	if (_src == NULL || _dst == NULL || _n < 1)
		return;
	for (i = 0; i < _n; i++)
		_dst[i] = _src[i];
}

TSTRING_BACKEND
char
_rawstr_is_equal(TSTRING_CHAR* _a, TSTRING_CHAR* _b, int _n)
{
	int i;
	if (_a == NULL || _b == NULL)
		return 0;
    for (i = 0; i < _n; i++)
		if (_a[i] != _b[i])
			return 0;
	return 1;
}

TSTRING_BACKEND
void
_tstring_allocate_buffer(tstring_s* _str, int _n)
{
	int i;
	if (_str == NULL || _str->is_const == 1 || _n < 1)
		return;

	tstring_destroy(_str);
	_str->c_str = TSTRING_MALLOC(sizeof(TSTRING_CHAR) * _n);
	if (_str->c_str == NULL)
		return;
	_str->maxlen = _n;
	_str->is_const = 0;
	/*Force null-termination for allocated memory*/
	for (i = 0; i < _str->maxlen; i++)
		_str->c_str[i] = TSTRING_NULLTERM;
}

TSTRING_API
char
tstring_invalid(tstring_s* _str)
{
	if (_str == NULL || _str->c_str == NULL)
		return 1;
	return 0;
}

TSTRING_API
void
tstring_destroy(tstring_s* _str)
{
	if (_str == NULL)
		return;
	if (_str->is_const)
		return;
	if (_str->c_str != NULL)
		TSTRING_FREE(_str->c_str);
	_str->c_str = NULL;
	_str->maxlen = 0;
	_str->is_const = 0;
}

TSTRING_API
tstring_s
tstring_reserve(int _n)
{
	tstring_s out = {0};
	if (_n < 1)
		return (tstring_s){0};

	_tstring_allocate_buffer(&out, _n+1);
	if (tstring_invalid(&out))
		return (tstring_s){0};
	return out;
}

TSTRING_API
tstring_s
tstring_n(TSTRING_CHAR* _str, int _n)
{
	tstring_s out = {0};
	out = tstring_reserve(_n);
	if (tstring_invalid(&out))
		return out;

	_rawstr_ncopy(_str, out.c_str, out.maxlen);
	out.c_str[out.maxlen-1] = TSTRING_NULLTERM;
	out.is_const = 0;
	return out;
}

TSTRING_API
tstring_s
tstring(TSTRING_CHAR* _str)
/*TODO: strings starting with \0 does not work:
  str = tstring("");
*/
{
	int n;
	if (_str == NULL)
		return (tstring_s){0};
	n = _rawstr_findchar(_str, TSTRING_NULLTERM);
	if (n == TSTRING_INVALID)
		return (tstring_s){0};
	return tstring_n(_str, n);
}

TSTRING_API
tstring_s
tstring_char(TSTRING_CHAR _char)
{
	tstring_s out = {0};
	out = tstring_reserve(2);
	if (tstring_invalid(&out))
		return out;

	out.c_str[0] = _char;
	out.c_str[1] = TSTRING_NULLTERM;
	out.is_const = 0;
	return out;
}

TSTRING_API
tstring_s
tstring_load_FILE(tstring_s* _path)
/*https://stackoverflow.com/questions/3747086/reading-the-whole-text-file-into-a-char-array-in-c
*/
{
	FILE* fp;
	int maxlen = 0;
	tstring_s out = {0};
	fp = fopen(_path->c_str, "rb");
	if (!fp)
		return (tstring_s) {0};

	fseek(fp, 0L, SEEK_END);
	maxlen = ftell(fp);
	rewind(fp);
	out = tstring_reserve(maxlen+1);
	if (fread(out.c_str, maxlen, 1, fp) != 1) {
		/*TODO: Do something when aborting*/
	}
	fclose(fp);
	return out;
}

TSTRING_API
tstring_s
tstring_format(TSTRING_CHAR* _str, ...)
{
    (void)(_str);
    return (tstring_s) {0};
}

TSTRING_API
int
tstring_length(tstring_s* _str)
{
	int len;
	if (tstring_invalid(_str))
		return TSTRING_INVALID;
	if (_str->is_const) {
		len = _rawstr_findchar(_str->c_str, TSTRING_NULLTERM);
	} else {
		len = _rawstr_nfindchar(_str->c_str, _str->maxlen-1, TSTRING_NULLTERM);
		if (len == TSTRING_INVALID) {
			_str->c_str[_str->maxlen-1] = '\0';
			return _str->maxlen;
		}
	}
	return len;
}

TSTRING_API
char
tstring_equal(tstring_s* _a, tstring_s* _b)
{
	int a_len;
	int b_len;
	if (_a == _b)
		return 1;
	if (_a == NULL || _b == NULL)
		return 0;
	a_len = tstring_length(_a);
	b_len = tstring_length(_b);
	if (a_len != b_len)
		return 0;

	return _rawstr_is_equal(_a->c_str, _b->c_str, a_len);
}
/*
TSTRING_API
tstring_s*
tstring_copyft(tstring_s* _src, tstring_s* _dst, int _f, int _t)
//TODO: copy cannot create a copy of a const tstring
{
	int n = _t - _f;
	int src_len;
	if (tstring_invalid(_src))
		return NULL;

	src_len = tstring_length(_src);
	if (n < 1 || _f > _t || _t > src_len) {
		printf("CAUGHT IN BUG AREA\n");
		return NULL;
	}

	tstring_destroy(_dst);
	*_dst = tstring_n(_src->c_str + _f, n+1);
	printf("copied to [%s]\n", _dst->c_str);
	_dst->c_str[n] = TSTRING_NULLTERM;
	return _dst;
}
*/

TSTRING_API
tstring_s*
tstring_copy_fn(tstring_s* _src, tstring_s* _dst, int _f, int _n)
/*TODO: copy cannot create a copy of a const tstring*/
{
	int src_len;
	if (tstring_invalid(_src))
		return NULL;

	src_len = tstring_length(_src);
	if (_n < 1 ||  _n > src_len) {
		_n = src_len;
		return NULL;
	}

	tstring_destroy(_dst);
	*_dst = tstring_reserve(_n + 1);
	if (tstring_invalid(_dst))
		return NULL;

	_rawstr_ncopy(_src->c_str + _f, _dst->c_str, _n);
	_dst->c_str[_n] = TSTRING_NULLTERM;
	return _dst;
}

TSTRING_API
tstring_s*
tstring_copy(tstring_s* _src, tstring_s* _dst)
{
	return tstring_copy_fn(_src, _dst, 0, tstring_length(_src));
}

TSTRING_API
tstring_s*
tstring_cut(tstring_s* _str, int _f, int _t)
{
	int copylen = 0;
	int str_len;
	if (tstring_invalid(_str))
		return NULL;
	str_len = tstring_length(_str);
	if (_f < 0 || _t > str_len || _f > _t)
		return NULL;
	copylen = str_len - _t;
	_rawstr_ncopy(_str->c_str + _t, _str->c_str + _f, copylen);
	_str->c_str[_f + copylen] = TSTRING_NULLTERM;
	return _str;
}

TSTRING_API
tstring_s*
tstring_split(tstring_s* _src, tstring_s* _lhs, tstring_s* _rhs, int _split)
{
	int src_len = 0;
	if (tstring_invalid(_src) || _split < 1)
		return NULL;
	src_len = tstring_length(_src);
	if (_split > src_len)
		return NULL;
	tstring_destroy(_lhs);
	tstring_destroy(_rhs);
	tstring_copy_fn(_src, _lhs, 0, _split);
	tstring_copy_fn(_src, _rhs, _split, src_len - _split);
	return _src;
}

TSTRING_API
int
tstring_find_char(tstring_s* _str, TSTRING_CHAR _char)
{
	if (tstring_invalid(_str))
		return TSTRING_INVALID;
	return _rawstr_nfindchar(_str->c_str, _str->maxlen-1, _char);
}

TSTRING_API
int
tstring_findlast_char(tstring_s* _str, TSTRING_CHAR _char)
{
	if (tstring_invalid(_str))
		return TSTRING_INVALID;
	return _rawstr_nfindlastchar(_str->c_str, _str->maxlen-1, _char);
}

TSTRING_API
int
tstring_find(tstring_s* _str, tstring_s* _f)
{
	int i;
	int str_len;
	int f_len;
	if (tstring_invalid(_str) || tstring_invalid(_f))
		return TSTRING_INVALID;

	str_len = tstring_find_char(_str, '\0');
	f_len = tstring_find_char(_f, '\0');
	if (str_len < 1 || f_len < 1)
		return TSTRING_INVALID;

	for (i = 0; i < str_len - f_len + 1; i++) {
		if (_rawstr_is_equal(_str->c_str + i, _f->c_str, f_len)) {
			return i;
		}
	}
	return TSTRING_INVALID;
}

TSTRING_API
int
tstring_findlast(tstring_s* _str, tstring_s* _f)
{
	int i;
	int str_len;
	int f_len;
	if (tstring_invalid(_str) || tstring_invalid(_f))
		return TSTRING_INVALID;

	str_len = tstring_find_char(_str, '\0');
	f_len = tstring_find_char(_f, '\0');
	if (str_len < 1 || f_len < 1)
		return TSTRING_INVALID;

	for (i = str_len - f_len; i > 0; i--) {
		if (_rawstr_is_equal(_str->c_str + i, _f->c_str, f_len)) {
			return i;
		}
	}
	return TSTRING_INVALID;
}

TSTRING_API
tstring_s
tstring_combine(tstring_s* _a, tstring_s* _b)
{
	int a_len;
	int b_len;
	tstring_s out = {0};
	if (tstring_invalid(_b))
		return (tstring_s) {0};
	if (tstring_invalid(_a)) {
		tstring_copy(_b, &out);
		return out;
	}
	a_len = tstring_length(_a);
	b_len = tstring_length(_b);
	if (a_len == TSTRING_INVALID || b_len == TSTRING_INVALID)
		return (tstring_s) {0};

	out = tstring_reserve(a_len + b_len + 1);
	if (tstring_invalid(&out))
		return (tstring_s) {0};
	_rawstr_ncopy(_a->c_str, out.c_str, a_len);
	_rawstr_ncopy(_b->c_str, out.c_str + a_len, b_len);
	out.c_str[a_len + b_len] = TSTRING_NULLTERM;
	return out;
}

TSTRING_API
tstring_s*
tstring_add_back(tstring_s* _str, tstring_s* _back)
{
	int str_len;
	int back_len;
	tstring_s combined = {0};
	if (tstring_invalid(_back))
		return NULL;
	if (tstring_invalid(_str))
		return tstring_copy(_back, _str);

	str_len = tstring_length(_str);
	back_len = tstring_length(_back);
	if (str_len == TSTRING_INVALID || back_len == TSTRING_INVALID)
		return NULL;

	combined = tstring_reserve(str_len + back_len + 1);
	if (tstring_invalid(&combined))
		return NULL;

	_rawstr_ncopy(_str->c_str, combined.c_str, str_len);
	_rawstr_ncopy(_back->c_str, combined.c_str + str_len, back_len);
	tstring_destroy(_str);
	*_str = combined;
	return _str;
}

TSTRING_API
tstring_s*
tstring_add_back_va(tstring_s* _str, int _n, ...)
/*TODO: Is unable to accpet tstring_const() defines*/
{
	int i;
	va_list ap;
	va_start(ap, _n);
	if (tstring_invalid(_str))
		*_str = tstring_reserve(1);
	for (i = 0; i < _n; i++)
		tstring_add_back(_str, va_arg(ap, tstring_s*));
	va_end(ap);
	return _str;
}

TSTRING_API
tstring_s*
tstring_to_upper(tstring_s* _str)
{
	int i;
	if (tstring_invalid(_str))
		return NULL;
	for (i = 0; i < _str->maxlen; i++)
		_TSTRING_CHAR_TO_UPPER(_str->c_str[i]);
	return _str;
}

TSTRING_API
tstring_s*
tstring_to_lower(tstring_s* _str)
{
	int i;
	if (tstring_invalid(_str))
		return NULL;
	for (i = 0; i < _str->maxlen; i++)
		_TSTRING_CHAR_TO_LOWER(_str->c_str[i]);
	return _str;
}

TSTRING_API
uint64_t
tstring_hash(tstring_s* _str)
/*Refrence, "sdbm" hash.*/
{
	uint32_t c;
	uint64_t out = 0;
	if (tstring_invalid(_str))
		return 0;
	while ((c = *_str->c_str++) != '\0')
		out = c + (out << 6) + (out << 16) - out;
	return out;
}

#endif /*TSTRING_IMPLEMENTATION*/
#endif /*TSTRING_H*/
