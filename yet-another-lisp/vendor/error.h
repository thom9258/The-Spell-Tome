#ifndef YAL_ERROR_H
#define YAL_ERROR_H

#include <stdio.h>
#include <stdint.h>

#ifndef ERRORSTACK_SIZE
#define ERRORSTACK_SIZE 32
#endif /*ERRORSTACK_SIZE*/

enum ERRORCODE {
    ERROR_NULL = 0,
    ERROR_WARNING,
    ERROR_ERROR,
    ERROR_FATAL,

    ERROR_COUNT
};

typedef struct {
	char code;
	char* msg;

	/*Automatic stuff no need to set manually*/
	int _LINE_;
	char* _FILE_;
	uint32_t index;
}Error;

#define ERROR(code, msg) (Error) {code, msg, __LINE__, __FILE__, 0}

typedef struct ErrorManager ErrorManager;

struct ErrorManager {
	Error errors[ERRORSTACK_SIZE];
	uint32_t top;
	uint32_t total;
	void (*on_error_callback)(ErrorManager*);
};


int
Error_put(ErrorManager* _ef, Error _err)
{
	if (_ef->top >= ERRORSTACK_SIZE)
		_ef->top = 0;
	_err.index = _ef->total++;
	_ef->errors[_ef->top++] = _err;

    /*Do a callback on the error stack*/
	if (_ef->on_error_callback != NULL)
		_ef->on_error_callback(_ef);
	return _ef->top;
}

Error
Error_pop(ErrorManager* _ef)
{
	if (_ef->top > 0)
		return _ef->errors[--_ef->top];
	return (Error) {0};
}

void
Errors_clear(ErrorManager* _ef)
{
	while (_ef->top > 0)
		Error_pop(_ef);
}

#endif /*YAL_ERROR_H*/
