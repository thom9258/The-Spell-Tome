/*
SHORT DESCRIPTION:


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

CHANGELOG:
[1.0] Added core functionality.
[0.0] Initialized library.
*/

#ifndef CTUPLE_H
#define CTUPLE_H

#include <stdint.h> /*uint32_t + uint8_t*/
#include <stdio.h>  /*NULL*/

#ifndef CTUPLE_NO_STDLIB
    #include <stdlib.h> /*malloc + realloc + free*/
    #define CTUPLE_MALLOC  malloc
    #define CTUPLE_FREE    free
    #define CTUPLE_SIZE_T  uint32_t

#endif

#ifndef CTUPLE_API
#define CTUPLE_API static inline
#endif

/*
Tuple of size 3 (int char int) init function call:
ctuple_new(3, sizeof(int), sizeof(char), sizeof(int))

	uint8_t        [ N_ members ]
	CTUPLE_SIZE_T  [  offset 1  ]
	CTUPLE_SIZE_T  [  offset 2  ]
	CTUPLE_SIZE_T  [  offset 3  ]
	uint8_t[]      [   data 1   ]
	uint8_t[]      [   data 2   ]
	uint8_t[]      [   data 3   ]

resulting tuple:

	[ N_ members ] = 3
	[  offset 1  ] = 0
	[  offset 2  ] = 2
	[  offset 3  ] = 3
	[   data 1   ] = 2 bytes
	[   data 2   ] = 1 byte
	[   data 3   ] = 4 bytes

*/

#define ctuple_def(...) ctuple

#define _ctuple_offset(TUPLE, INDEX)                                           \
	( (INDEX > 0 && INDEX < TUPLE.count) ? TUPLE.data[INDEX] : 0 )

#define ctuple(TUPLE, INDEX, TYPE)                                             \
	TUPLE.data[_ctuple_offset(TUPLE, INDEX)]

typedef uint8_t* ctuple;

CTUPLE_API
ctuple
ctuple_new(int _n, ...);

CTUPLE_API
void
ctuple_destroy(ctuple _tuple);

/******************************************************************************/
#ifdef CTUPLE_IMPLEMENTATION

CTUPLE_API
ctuple
ctuple_new(int _n_members, ...)
{
	int i;
	CTUPLE_SIZE_T total_memsize = sizeof(uint8_t);
	ctuple tuple = NULL;
	uint8_t* tmp = NULL;
	va_list va_sizes;
	va_list va_offset;

	/*Construct 3 va lists to iterate though*/
	va_copy(va_offset, va_sizes);

	/*Accumulate total size needed in tuple*/
	va_start(va_sizes, _n_members);
	for (i = 0; i < _n_members; i++)
		total_memsize += sizeof(CTUPLE_SIZE_T) + va_arg(ap, CTUPLE_SIZE_T);
	va_end(va_sizes);

	printf("Needed size = %ld\n", total_memsize);
	tuple = (ctuple)CTUPLE_MALLOC(memsize);
	if (tuple == NULL)
		return NULL;

	/*Insert offsets*/
	*tuple = _n_members;
	tmp = tuple + sizeof(uint8_t);
	va_start(va_offset, _n_members);
	for (i = 0; i < _n_members; i++) {
		*tmp = va_arg(va_offset, CTUPLE_SIZE_T);
		tmp += sizeof(CTUPLE_SIZE_T);
	}
	va_end(va_offset);
	return tuple;
}

CTUPLE_API
void
ctuple_destroy(ctuple* _tuple)
{
	if (*_tuple == NULL)
		return;
	CTUPLE_FREE(*_tuple);
	*_tuple = NULL;
}

#endif /*CTUPLE_IMPLEMENTATION*/
#endif /*CTUPLE_H*/
