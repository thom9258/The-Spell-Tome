/* How to define/remove custom namespaces:
 * note: "namespace" refers to a custom library tag, such as 
 *     alex_JSON_parse()
 *     |    |
 *     |    |_ function
 *     |
 *     |_namespace
 *
 * standard namespace would become:
 *
 *     arr_alex_rect
 *
 * but that is not very pleasing, we might want the namespace to be before arr_
 * or we might need to omit the namespace alltogether, so we can define:
 *
 *     #define TSARRAY_NAME_OVERRIDE
 *
 * and then define a custom namespace, examples:
 *
 *     #define arr_t_name my_rectlist
 *     #define arr_t_name alex_arr_rect
 * */

#include <stdio.h>

typedef struct {
    int x;
    int y;
    int w;
    int h;
}my_rectangle;

void
my_rectangle_print(my_rectangle _a)
{
    printf("[%d,%d,%d,%d]\n", _a.x, _a.y, _a.w, _a.h);
}

#define TSARRAY_NAME_OVERRIDE
#define t_type my_rectangle
#define arr_t_name my_rectlist
#define t_operator_print(a) my_rectangle_print(a)
#include "../tsarray.h"

void
test_namespace_override(void)
{
    printf("namespace override test --------------\n");
    printf("namespace would have been:\n"
           "\tarr_my_rectangle\n"
           "redefined namespace to\n"
           "\tmy_rectlist\n"
           );

    my_rectlist arr = {0};
    arr = my_rectlist_initn(5);

    my_rectlist_push(&arr, (my_rectangle){0});
    my_rectlist_push(&arr, (my_rectangle){0});
    my_rectlist_push(&arr, (my_rectangle){0});
    my_rectlist_push(&arr, (my_rectangle){0});

    printf("size=%d, max=%d\n", arr.count, arr.max);
    my_rectangle pop = my_rectlist_pop(&arr);
    my_rectangle_print(pop);
    printf("size=%d, max=%d\n", arr.count, arr.max);

    printf("printing array using build-in operator:\n");
    my_rectlist_print(&arr);

    my_rectlist_destroy(&arr);
    printf("after destroy: size=%d, max=%d\n", arr.count, arr.max);
}


