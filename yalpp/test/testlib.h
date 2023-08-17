/* 
# TESTLIB
	testlib.h - Testing utilities for testing of c libraries,
	written in ansi-c99.

## ORIGINAL AUTHOR

Thomas Alexgaard Jensen (![gitlab/Alexgaard](https://gitlab.com/Alexgaard))

```txt
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

```

## LICENSE

Copyright (c) Thomas Alexgaard Jensen
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
![Zlib License](https://choosealicense.com/licenses/zlib/)

## CHANGELOG
- [1.0] First full release with all intended functionality.
- [0.2] Implemented random utils, fixed structure of core utils.
        Implemented Summary function.
- [0.1] Implemented core utilities.
- [0.0] Initialized library.

## DOCUMENTATION

*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h> /*stdout*/
#include <stdarg.h> /*va_list*/
#include <stdint.h> /*uint32_t, uint8_t*/
#include <time.h> /*clock_t, clock(), CLOCKS_PER_SEC*/

#ifndef TL_H
#define TL_H

#ifndef TL_TARGET 
#define TL_TARGET stdout
#endif /*TL_TARGET*/

#ifdef TL_NO_COLOR
#define TL_COLOR(COLOR)
#else /*NOT TL_NO_COLOR*/
#define TL_COLOR(COLOR) fprintf(TL_TARGET, COLOR)
#endif /*TL_NO_COLOR*/

#define TL_RED     "\033[0;31m"
#define TL_GREEN   "\033[0;32m"
#define TL_BLUE    "\033[0;34m"
#define TL_RESET   "\033[0m"
#define TL_DEFAULT TL_BLUE

#define TL_TEST_SIZE 128

#define TL_PRINT(MSG, ...)                                                     \
	fprintf(TL_TARGET, MSG, ##__VA_ARGS__)

#define TL_FUNCTIONSTRING(FN) (const char*)#FN


#define TL_LINE                                                                \
"----------------------------------------------------------------------------\n"

#define TL_BOLDLINE                                                            \
"============================================================================\n"

#define TL_IGNORE_VAR(_VARIABLE) (void)_VARIABLE;

#define TL_TEST(_EXPR)                                                         \
	_TL_TEST_FUNCTION(!(_EXPR), #_EXPR, __LINE__, NULL)

#define TL_TESTM(_EXPR, _MSG)                                                  \
	_TL_TEST_FUNCTION(!(_EXPR), #_EXPR, __LINE__, _MSG)

#define TLM(FUNCTION, MSG)                                                     \
	_TL_PRE_FUNCTION(__FILE__, #FUNCTION, MSG);                        \
	FUNCTION;                                                                  \
	_TL_POST_FUNCTION(#FUNCTION);                                              \

#define TL(FUNCTION) TLM(FUNCTION, NULL)

#ifndef TL_SEED_ADVANCE
	#define TL_SEED_ADVANCE randseed++
#endif /*TL_SEED_ADVANCE*/

#define TL_IS_MEMALIGNED(POINTER, BYTE_COUNT) \
	( (((uintptr_t)(const void*)(POINTER)) & (BYTE_COUNT-1)) == 0)

typedef struct {
	time_t start;
	double elapsed_sec;
}tl_timer_s;


static char* test_names[TL_TEST_SIZE] = {0};
static char  test_status[TL_TEST_SIZE] = {0};
static int tests_encountered = 0;

static int errors_encountered = 0;
static int errors_total = 0;

static uint32_t randseed = 220999;

static tl_timer_s timer = {0, 0};

void
tl_timer_start(tl_timer_s* _t)
{
	if (_t == NULL)
		return;
	_t->start = clock();
}

void
tl_timer_stop(tl_timer_s* _t)
{
	if (_t == NULL)
		return;
	_t->elapsed_sec += (double)(clock() - _t->start) / CLOCKS_PER_SEC;
}

void
tl_timer_reset(tl_timer_s* _t)
{
	if (_t == NULL)
		return;
	_t->elapsed_sec = 0;
	_t->start = 0;
}

void
_TL_TEST_FUNCTION(char _expr, const char* _expr_str, int _line, const char* _msg)
/**
 * _TL_TEST_FUNCTION() - Test expression for pass/fail 
 * @arg1: Expression result
 * @arg2: Expression string
 * @arg3: Line number
 * @arg4: External message for test
 *
 * Tests function expression.
 */
{
	errors_total++;                                                   
	char* outcome;
	char* msg;
	if (_expr) {                                                   
		errors_encountered++;                                         
		outcome = (char*)"failed";
		TL_COLOR(TL_RED);
	}                                                                 
	else {                                                            
		outcome = (char*)"passed";
		TL_COLOR(TL_GREEN);
	}                                                                 
	if (_msg == NULL)
		msg = (char*)"";
	else
		msg = (char*)_msg;

	TL_PRINT(">>[test L%d] ( %s) %s. %s\n",
	         _line, _expr_str, outcome, msg);
	TL_COLOR(TL_RESET);
}

void
_TL_PRE_FUNCTION(const char* _file_name, const char* _test_name, const char* _test_message)
/**
 * _TL_PRE_FUNCTION() - Print pre-function formality of testlib.
 * @arg1: File name
 * @arg2: Test function name
 *
 * Prints:
 * - Visually eye-catching start of test section
 * - test file name
 * - function to test & its line number
 */
{
	errors_encountered = 0;                     
	errors_total = 0;                           
	TL_COLOR(TL_DEFAULT);
	TL_PRINT(TL_BOLDLINE);                                      
	TL_PRINT(TL_BOLDLINE);                                      
	TL_PRINT("[ TEST %d ]  File %s -> ", tests_encountered, _file_name);
	TL_COLOR(TL_RESET);
	TL_PRINT("%s:\n", _test_name);
	TL_COLOR(TL_DEFAULT);
	if (_test_message != NULL) {
		TL_PRINT("%s\n", _test_message);
	}
	TL_PRINT(TL_BOLDLINE);                                      
	TL_COLOR(TL_RESET);

	tl_timer_reset(&timer);
	tl_timer_start(&timer);
}

void
_TL_POST_FUNCTION(const char* _test_name)
/**
 * _TL_POST_FUNCTION() - Print post-function formality of testlib.
 *
 * Prints:
 * - Visually eye-catching passed/fail
 * - execution time
 * - outcome summation of tests for given function
 */
{
	tl_timer_stop(&timer);

	char* outcome = NULL;
	test_names[tests_encountered] = (char*)_test_name;
	if (errors_encountered == 0) {
		TL_COLOR(TL_GREEN);
		outcome = (char*)"PASSED";
		test_status[tests_encountered] = 0;
	}
	else {
		TL_COLOR(TL_RED);
		outcome = (char*)"FAILED";
		test_status[tests_encountered] = 1;
	}
	TL_PRINT(TL_LINE);                                      
	TL_PRINT(">>>[[ TEST %d %s ]]  ->", tests_encountered, outcome);
	TL_PRINT("  (errors=%d/%d, time=%fs)\n",
	          errors_encountered, errors_total,                  
	          timer.elapsed_sec);
	TL_PRINT(TL_BOLDLINE);                                      
	TL_PRINT(TL_BOLDLINE);                                      
	TL_COLOR(TL_RESET);
	tests_encountered++;
}


void
tl_summary(void)
/**
 * tl_summary() - Print summary of all tests.
 */
{
	int i;
	TL_COLOR(TL_DEFAULT);
	TL_PRINT(TL_BOLDLINE);
	TL_PRINT(TL_BOLDLINE);
	TL_PRINT("Tests Summary:\n");
	TL_PRINT(TL_LINE);
	TL_COLOR(TL_RESET);
	TL_PRINT("Total tests = (%d)\n", tests_encountered);

	for (i = 0; i < tests_encountered; i++) {
		if (test_status[i] == 0) {
			TL_COLOR(TL_GREEN);
			TL_PRINT("\t [%d]  %s\n", i, test_names[i]);
		} else {
			TL_COLOR(TL_RED);
			TL_PRINT("\t [%d]  %s\n", i, test_names[i]);
		}
	}
	TL_COLOR(TL_DEFAULT);
	TL_PRINT(TL_BOLDLINE);
	TL_PRINT(TL_BOLDLINE);
	TL_COLOR(TL_RESET);
}

void
tl_rand_seed(uint32_t _seed)
/**
 * tl_rand_seed() - Set seed for random functions.
 * @arg1: the seed used to generate random value.
 */
{
	randseed = _seed;
}

uint32_t
tl_rand_uint(void)
/**
 * tl_rand_uint() - generate seeded random uint32.
 * @arg1: the seed used to generate random value.
 *
 * Generate psuedo-random unsigned integer of 32 bit.
 * seed value is changed according to "define TL_SEED_ADVANCE".
 *
 * Return: generated number.
 */
{
	static const uint32_t mangle_1     = 0x3D73;
	static const uint32_t mangle_2     = 0xC0AE5;
	static const uint32_t mangle_3     = 0x5208DD0D;
	static const uint32_t mangle_shift = 13;
	uint32_t n;
	n = (randseed << mangle_shift) ^ randseed;
	n *= n * mangle_1;
	n += mangle_2;
	n *= n;
	n += mangle_3;
	TL_SEED_ADVANCE;
	return n;
}

uint8_t
tl_rand_bool(void)
/**
 * tl_rand_uint() - generate seeded random bool (0/1).
 * @arg1: the seed used to generate random value.
 *
 * Generate psuedo-random boolean (0/1).
 * Effectively returns even/odd of tl_rand_uint().
 *
 * Return: generated number.
 */
{
	return (tl_rand_uint() & 1);
}

uint32_t
tl_rand_ubetween(uint32_t _min, uint32_t _max)
/**
 * tl_rand_ubetween() - generate seeded random between specified min/max.
 * @arg1: the seed used to generate random value.
 * @arg2: minimum generated value.
 * @arg3: maximum generated value.
 *
 * Generate psuedo-random unsigned integer 32 bit, generated number is modified
 * to fit between specified max/min).
 *
 * Return: generated number.
 */
{
	return ((tl_rand_uint() % (_max - _min + 1)) + _min);
}

#endif /*TL_H*/

#ifdef __cplusplus
}
#endif
