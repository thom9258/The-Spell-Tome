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

void
conv_error_callback(struct error_flagger_manager* _ef)
{
	/*Just print entire log!
      Could just as well just print the newest error, or pass it to another
      manager entirely.*/
	int i = _ef->head-1;
	printf("[ERROR] (code=%d, idx=%d) : %s->%d)\tmsg = \"%s\"\n",
		   _ef->errors[i].code,
		   _ef->errors[i].error_index,
		   _ef->errors[i]._FILE_,
		   _ef->errors[i]._LINE_,
		   _ef->errors[i].msg);
	//assert(_ef->errors[i].code != FATAL_ERROR);
}


/* A Global "static" error manager is defined for error management in the
 * library.
 * */
static struct error_flagger_manager EF = ERROR_FLAGGER_NEW(conv_error_callback);

/*Convenience defines for library*/
#define CONV_ERROR_PUSH(code, msg) \
	error_flagger_push(&EF, ERROR_FLAG_NEW(code, msg));

unsigned char
conv_int2uchar(int _v)
/* some_function() -> cast a int to a char. 
 *
 * Return: If a int larget than max_char or less than 0 is entered, throw a
 *         error flag to error manager, instead of asserting.
 * */
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
