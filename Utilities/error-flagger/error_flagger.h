/*
SHORT DESCRIPTION:
        error_flagger.h - Easy to use soft error management tool
        written in ansi-c99.

ORIGINAL AUTHOR:
        Thomas Alexgaard Jensen (https://gitlab.com/Alexgaard)

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
        This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages arising
from the use of this software. Permission is granted to anyone to use this
software for any purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

        1. The origin of this software must not be misrepresented; you must not
           claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
           appreciated but is not required.

        2. Altered source versions must be plainly marked as such, and must not
be misrepresented as being the original software.

        3. This notice may not be removed or altered from any source
distribution.

        For more information, please refer to
        <https://choosealicense.com/licenses/zlib/>

DESCRIPTION:
        A simple to use, and barebones error management tool inspired by the
OpenGL <glGetError()> function.

        error_flagger.h is an unintrusive alternative to context-breaking
asserts in library code.

        The tool can simply be included and used in any library.

CHANGELOG:
        [0.2] Rewritten to header-only-library.
              Updated cookbook.
              Updated Documentation.
        [0.1] Implemented core utilities.
        [0.0] Initialized library.

DOCUMENTATION:
        To use library, define ERROR_FLAGGER_IMPLEMENTATION before include.

        The tool is designed to work around the idea of non-intrusive &
		voulentary error management and checking. The idea is that the library designer,
        defines a set of error codes that the library is able to emit at runtime
        without explicit assertion, and then it is up to the user of that
		library to make use of those emitted errors.
		The user of the library specifies a error callback function for this purpose.

        WHY USE THIS LIBRARY?

        direct assertions are liable to directly mess up code context when
		running code that executes for a long time. what do you do if a assertions
		happens after executing for an hour and the break is highly context dependent?
        On top of this assertions are stripped when releasing code anyway, they
		only serve a purpose for debugging, but we can do better.

COOKBOOK:

#include <stdio.h>
#include <assert.h>

#define ERROR_FLAGGER_IMPLEMENTATION
#define ERROR_FLAGGER_STACK_SIZE 4
#include "../error_flagger.h"

enum _ERROR_FLAGS {
	NO_ERROR = 0,
	INVALID_SIZE_OF_INPUT = 4,
	UNKNOWN_ERROR = 7,
	FATAL_ERROR = 100,
};

// Callback function that decides what happens on error.
void
conv_error_callback(struct error_flagger_manager* _ef)
{
	printf("[ERROR] (code=%d, idx=%d) : %s->%d)\tmsg = \"%s\"\n",
		   _ef->errors[i].code,
		   _ef->errors[i].error_index,
		   _ef->errors[i]._FILE_,
		   _ef->errors[i]._LINE_,
		   _ef->errors[i].msg);
	assert(_ef->errors[i].code != FATAL_ERROR);
}


// A Global "static" error manager is defined for error management in the
// library.
static struct error_flagger_manager EF = ERROR_FLAGGER_NEW(conv_error_callback);

// Convenience defines for library
#define CONV_ERROR_PUSH(code, msg) \
	error_flagger_push(&EF, ERROR_FLAG_NEW(code, msg));

unsigned char
conv_int2uchar(int _v)
{
	if (_v > 255 || _v < 0) {
		CONV_ERROR_PUSH(INVALID_SIZE_OF_INPUT,
					    "inalid input occoured in conv_int2uchar(),"
						" values between 0 -> 255 accepted!");
	}
	else {
		CONV_ERROR_PUSH(NO_ERROR,
					    "Good Job! conv_int2uchar() was called correctly");
	}
	return _v;
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;
	
	unsigned char t;
	t = conv_int2uchar(45);
	printf("val = (%d)\n", t);

	t = conv_int2uchar(419);
	printf("val = (%d)\n", t);

	CONV_ERROR_PUSH(UNKNOWN_ERROR, "Unknown error who knows?")
	CONV_ERROR_PUSH(UNKNOWN_ERROR, "Unknown error who knows?")
	CONV_ERROR_PUSH(UNKNOWN_ERROR, "Unknown error who knows?")
	CONV_ERROR_PUSH(FATAL_ERROR, "Finish Him!")
	return 0;
}

*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ERROR_FLAGGER_H
#define ERROR_FLAGGER_H

#ifndef ERROR_FLAGGER_STACK_SIZE
#define ERROR_FLAGGER_STACK_SIZE 128
#endif /*error_flagger_manager_SIZE*/

#ifndef ERROR_FLAGGER_ERROR_TYPE
#define ERROR_FLAGGER_ERROR_TYPE int
#endif /*ERROR_FLAGGER_ERROR_TYPE*/

#ifndef ERROR_FLAGGER_DUMMY_ERROR
#define ERROR_FLAGGER_DUMMY_ERROR                                               \
	ERROR_FLAG_NEW(-1,"This is a \"Dummy Error\", no errors were in the flagger")
#endif /*ERROR_FLAGGER_DUMMY_ERROR*/

struct _error_flag {
	ERROR_FLAGGER_ERROR_TYPE code;
	char* msg;

	/*Automatic stuff no need to set manually*/
	int _LINE_;
	char* _FILE_;
	int error_index;
};
#define ERROR_FLAG_NEW(code, msg)					\
	(struct _error_flag) {code, msg, __LINE__, __FILE__, -1}

struct error_flagger_manager {
	struct _error_flag errors[ERROR_FLAGGER_STACK_SIZE];
	int head;
	void (*callback)(struct error_flagger_manager*);
	int total_error_count;
};
#define ERROR_FLAGGER_NEW(callback) \
	(struct error_flagger_manager) {{{0}}, 0, callback, 0}

int
error_flagger_push(struct error_flagger_manager* _ef,
                   struct _error_flag _err);

struct _error_flag
error_flagger_pop(struct error_flagger_manager* _ef);

void
error_flagger_clear(struct error_flagger_manager* _ef);

#endif /*ERROR_FLAGGER_H*/
/******************************************************************************/
#ifdef ERROR_FLAGGER_IMPLEMENTATION

int
error_flagger_push(struct error_flagger_manager* _ef,
                   struct _error_flag _err)
{
	int i;
	if (_ef->head >= ERROR_FLAGGER_STACK_SIZE) {
		/*Relocate stack downwards to fit new error*/
		for (i = 1; i < ERROR_FLAGGER_STACK_SIZE; i++)
			_ef->errors[i-1] = _ef->errors[i];
		_ef->head--;
	}

	_err.error_index = _ef->total_error_count++;
	_ef->errors[_ef->head++] = _err;

    /*Do a callback on the error stack.
	 *Determine what to do with pushed error*/
	if (_ef->callback != NULL)
		_ef->callback(_ef);
	return _ef->head;
}

struct _error_flag
error_flagger_pop(struct error_flagger_manager* _ef)
{
	if (_ef->head > 0)
		return _ef->errors[--_ef->head];
	return ERROR_FLAGGER_DUMMY_ERROR;
}

void
error_flagger_clear(struct error_flagger_manager* _ef)
{
	while (_ef->head > 0)
		error_flagger_pop(_ef);
}

#endif /*ERROR_FLAGGER_IMPLEMENTATION*/

#ifdef __cplusplus
}
#endif
