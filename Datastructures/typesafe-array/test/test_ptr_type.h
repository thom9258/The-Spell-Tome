#include <stdlib.h> 
#include <stdio.h> 

typedef struct {
    int uid;
    double pos_x;
    double pos_y;
}entity;

static int uid_gen = 0;

entity*
entity_new(void)
{
    entity* out = (entity*)malloc(sizeof(entity));
    *out = (entity){0};
    out->uid = uid_gen++;
    return out;
}

void
entity_destroy(entity* _e)
{
    if (_e != NULL)
        free(_e);
}

void
entity_print(entity* _e)
{
    if (_e == NULL)
        return;
    printf("[UID=%d, pos=(%lf,%lf)]\n",
           _e->uid, _e->pos_x, _e->pos_y);
}

entity*
entity_copy(entity* _e)
{
    entity* copy = entity_new();
    if (copy == NULL)
        return NULL;
    copy->pos_x = _e->pos_x;
    copy->pos_y = _e->pos_y;
    return copy;
}

typedef entity* entity_ptr;
#define t_type entity_ptr
#define t_operator_init entity_new
#define t_operator_destroy entity_destroy
#define t_operator_copy entity_copy
#define t_operator_print entity_print
#include "../tsarray.h"

void
test_ptr_type(void)
{
    printf("Ptr Type test --------------\n");
    arr_entity_ptr arr = arr_entity_ptr_initn(5);

    arr_entity_ptr_push(&arr, entity_new());
    arr_entity_ptr_push(&arr, entity_new());
    arr_entity_ptr_push(&arr, entity_new());
    arr_entity_ptr_push(&arr, entity_new());

    printf("size=%d, max=%d\n", arr.count, arr.max);
    printf("original array:\n");
    arr_entity_ptr_print(&arr);

    printf("\nduplicated array:\n");
    arr_entity_ptr dup = arr_entity_ptr_duplicate(&arr);
    arr_entity_ptr_print(&dup);

    printf("\nconcatenated array:\n");
    arr_entity_ptr_concatenate(&arr, &dup);
    arr_entity_ptr_print(&arr);

    entity* tmp = NULL;
    for (int i = 0; i < arr_entity_ptr_len(&arr); i++) {
        tmp = *arr_entity_ptr_peek(&arr, i);
        if (tmp->uid == 3) {
            arr_entity_ptr_get(&arr, i);
            entity_destroy(tmp);
        }
    }
    printf("\nremoved entity with uid=3:\n");
    arr_entity_ptr_print(&arr);

    arr_entity_ptr_destroy_members(&arr);
    arr_entity_ptr_destroy(&arr);
    arr_entity_ptr_destroy(&dup);
}
