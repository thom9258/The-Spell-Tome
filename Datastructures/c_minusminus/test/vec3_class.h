#ifndef VEC3CLASS_H
#define VEC3CLASS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../c--.h"

uint32_t CLASS_ID_VEC3 = 1;

typedef struct {
    int val;
}C_vec3;

void
int_class_operator_print(class_s* _this)
{
    if (_this == NULL)
        return;
    printf("%d", CLASS_MEMBER(_this, int_class)->val);
}

class_s*
class_int_new(int _val)
{
    /*Allocate class container*/
    class_s* out = class_t_new(CLASS_ID_INT, sizeof(int_class));

    if (out == NULL)
        return NULL;

    out->print = int_class_operator_print;

    /*Initialize data using constructer input*/
    CLASS_MEMBER(out, int_class)->val = _val; 
    return out;
}

#endif /*VEC3CLASS_H*/
