/* 
SHORT DESCRIPTION: 
    tstring.h - textstring library written in ansi-c99.

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

MISSING:
CHANGELOG:
	[0.0] Initialized library.
*/

#ifndef C_MINUSMINUS_H
#define C_MINUSMINUS_H

#include <stdio.h>  /*NULL*/
#include <stdint.h> /*uint32_t + uint8_t*/
#include <stdlib.h>  /*malloc, realloc, free*/
#include <assert.h>  /*assert*/

#ifndef C_MINUSMINUS_API
#define C_MINUSMINUS_API static inline
#endif

#define CLASS_ASSERT(MSG) assert(MSG)
#define CLASS_ASSERT0 assert(1)

#define CLASS_SIZEOF(CLASS) ( CLASS->typesize + sizeof(class_s) )
#define CLASS_TYPE(CLASS) ( CLASS->type )
#define CLASS_MEMBER(CLASS, TYPE) ( (TYPE*)(CLASS + 1) )

#define CLASS_DESTROY(CLASS)  \
    ( (CLASS->op_destroy == NULL) ? CLASS_ASSERT0 : CLASS->op_destroy(CLASS) )

#define CLASS_PRINT(CLASS)  \
    ( (CLASS->op_print == NULL) ? CLASS_ASSERT0 : CLASS->op_print(CLASS) )

struct class_type;
typedef struct class_type class_s;

struct class_type {
    /*Operators*/
    void (*op_destroy)(class_s* _this);
    void (*op_print)(class_s* _this);
    void (*op_plus)(class_s* _this, class_s* _lhs, class_s* _rhs);
    void (*op_minus)(class_s* _this, class_s* _lhs, class_s* _rhs);

    /*Class members*/
    uint32_t    type;
    uint32_t    typesize;
};

C_MINUSMINUS_API
class_s*
class_t_new(uint32_t _type, uint32_t _typesize);

/******************************************************************************/
#ifdef C_MINUSMINUS_IMPLEMENTATION

static uint32_t id_count = 0;

uint32_t
class_id_generate(void)
{
    return id_count++;
}

C_MINUSMINUS_API
class_s*
class_t_new(uint32_t _type, uint32_t _typesize)
{
    class_s* out = (class_s*)malloc(sizeof(class_s) + _typesize);
    if (out == NULL)
        return NULL;
    out->type = _type;
    out->typesize = _typesize;
    return out;
}

#endif /*C_MINUSMINUS_IMPLEMENTATION*/
#endif /*C_MINUSMINUS_H*/
