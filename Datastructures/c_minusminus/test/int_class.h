#ifndef INT_CLASS_H
#define INT_CLASS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../c--.h"

uint32_t C_int_id = 0;

typedef struct {
    int val;
}C_int;

void
C_int_class_initialize(void)
{
    C_int_id = class_id_generate();
}

void
C_int_operator_print(class_s* _this)
{
    if (_this == NULL)
        return;
    if (CLASS_TYPE(_this) != C_int_id)
        return;

    printf("%d", CLASS_MEMBER(_this, C_int)->val);
}

class_s*
C_int_new(int _val)
{
    /*Allocate class container*/
    class_s* out = class_t_new(C_int_id, sizeof(C_int));

    if (out == NULL)
        return NULL;

    out->op_print = C_int_operator_print;

    /*Initialize data using constructer input*/
    CLASS_MEMBER(out, C_int)->val = _val; 
    return out;
}

#endif /*INT_CLASS_H*/
