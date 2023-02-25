/* 
SHORT DESCRIPTION:
	Simple multi-file logger in ansi-c99.

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

DESCRIPTION:
	a stb-style header only library implemented in ansi c.
	<https://github.com/nothings/stb/blob/master/docs/stb_howto.txt>

CHANGELOG:
	[1.1] Simplified syntax.
	      Cleaned implementation.
	      Implemented filename input for multi-file logging.
	      Implemented macro for showing datetime information.
	[1.0] Ported from another logger library.
	[0.0] Initialized library.

DOCUMENTATION:

EXAMPLE HOW-TO USE:

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;
	char* log_a = "Logfile_A.txt";
	char* log_b = "Logfile_B.txt";
	LOG_CLEAN(log_a);
	LOG_CLEAN(log_b);
	LOG_INFO(log_a, "Remember to drink lots of water! (%d liters)", 2);

	LOG_WARNING(log_a, "Detected something bad.");
	LOG_WARNING(log_a, "Kernel error count: (%d), ram usage: (%f%%).", 15, 68.4);
	LOG_ERROR(log_a, "Stopping execution.");
	LOG_LOG(log_a, "CUSTOM TYPE", "info about this type.");

	char* complicated_msg = "hello this is a multiline msg \n\n"
	                        "This message has no applied formatting other than "
	                        "standard arguments like a printf()\n"
	                        "this is the end of it \n";
	LOG_PURE(log_b, complicated_msg);
	return 0;
}

*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MLOGGER_H
#define MLOGGER_H

#include <stdio.h>
#include <stdarg.h>

#ifdef LOG_SHOW_DATETIME

#include <time.h>
#ifndef LOG_DATETIME_FSTRING
#define LOG_DATETIME_FSTRING "%d-%d-%d %d:%02d:%02d - "
#endif /*LOG_DATETIME_FORMAT*/

#ifndef LOG_DATETIME_FVALUE
#define LOG_DATETIME_FVALUE(T)                                                 \
    T.tm_year+1900, T.tm_mon+1, T.tm_mday,                                     \
    T.tm_hour, T.tm_min, T.tm_sec
#endif /*LOG_DATETIME_FORMAT*/

#endif /*LOG_SHOW_DATETIME*/

#ifndef LOG_FILELINE_FORMAT
#define LOG_FILELINE_FORMAT "{%s: L %d}"
#endif

#define LOG_CLEAN(_FILENAME) fclose(fopen(_FILENAME, "w")); 

#ifndef MLOG_CUSTOM_LOGS
#define LOG_LOG(_FILENAME, _TYPE, _MSG, ...) \
    _internal_mlog(_FILENAME, _TYPE, __FILE__, __LINE__, _MSG, ##__VA_ARGS__);

#define LOG_INFO(_FILENAME, _MSG, ...) \
    LOG_LOG(_FILENAME, "INFO", _MSG, ##__VA_ARGS__)

#define LOG_WARNING(_FILENAME, _MSG, ...) \
    LOG_LOG(_FILENAME, "WARNING", _MSG, ##__VA_ARGS__)

#define LOG_ERROR(_FILENAME, _MSG, ...) \
    LOG_LOG(_FILENAME, "ERROR", _MSG, ##__VA_ARGS__)

#define LOG_PURE(_FILENAME, _MSG, ...) \
    _internal_mlog_pure(_FILENAME, _MSG, ##__VA_ARGS__)
#endif /*MLOG_CUSTOM_LOGS*/

/******************************************************************************/
#ifdef LOG_IMPLEMENTATION

void
_internal_mlog(char* _filename, 
               char* _type,
               char* _file_,
               int _line_,
               char* _msg,
               ...)
/**
 * _internal_mlog() - internal multi-file log function for LOG macros.
 * @arg1: Name of logfile
 * @arg2: Logtype, is automatically encased by square brackets -> [TYPE]
 * @arg3: Expects preprocessor macro __FILE__
 * @arg4: Expects preprocessor macro __LINE__
 * @arg5: Message that log needs to express
 * @arg6: (va_args) that needs to be placed inside message (@arg5)
 *
 * A internal log print function, that enables logwriting to files.
 * (needs to explain in more detail later.)
 */
{
	FILE* f;
	va_list args;
#ifdef LOG_SHOW_DATETIME
	time_t t;
	struct tm lt;
#endif /*LOG_SHOW_DATETIME*/
	f = fopen(_filename, "a");
	if (f == NULL)
		return;
#ifdef LOG_SHOW_DATETIME
	t = time(NULL);
	lt = *localtime(&t);
	/*Datetime*/
	fprintf(f, LOG_DATETIME_FSTRING, LOG_DATETIME_FVALUE(lt));
#endif /*LOG_SHOW_DATETIME*/
	/*Message type*/
	fprintf(f, "[%s] ", _type);
	/*Message with va_args*/
	va_start(args, _msg);
	vfprintf(f, _msg, args);
	va_end(args);
	/*File and line of function call*/
	fprintf(f, " " LOG_FILELINE_FORMAT "\n", _file_, _line_);
	fclose(f);
}

void
_internal_mlog_pure(char* _filename, char* _msg, ...)
/**
 * _internal_mlog_pure() - internal multi-file log function without formal info.
 * @arg1: Name of logfile
 * @arg2: Message that log needs to express
 * @arg3: (va_args) that needs to be placed inside message (@arg5)
 *
 * A internal log print function, that enables logwriting to files.
 * Does not place any form of special information such as datetime, priority or
 * line/file information.
 */
{
	FILE* f;
	va_list args;
	f = fopen(_filename, "a");
	if (f == NULL)
		return;
	va_start(args, _msg);
	vfprintf(f, _msg, args);
	va_end(args);
	fclose(f);
}

#endif /*LOG_IMPLEMENTATION*/
#endif /*MLOGGER_H*/

#ifdef __cplusplus
}
#endif
