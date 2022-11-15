#include <stdio.h>

void
double_print(double _a)
{
    printf("%lf ", _a);
}
#define t_operator_print(d) double_print(d)

#define t_type double
#include "../tsarray.h"

void
test_double(void)
{
    arr_double arr = {0};
    arr = arr_double_initn(2);

    printf("size=%d, max=%d\n", arr.count, arr.max);
    arr_double_push(&arr, 23.4);
    printf("size=%d, max=%d\n", arr.count, arr.max);
    printf("size=%d, max=%d\n", arr.count, arr.max);
    arr_double_push(&arr, 15.8);
    printf("size=%d, max=%d\n", arr.count, arr.max);
    arr_double_push(&arr, 7.1);
    printf("size=%d, max=%d\n", arr.count, arr.max);
    double pop = arr_double_pop(&arr);
    printf("popped a value (val=%lf)\n", pop);
    printf("size=%d, max=%d\n", arr.count, arr.max);

    arr_double_print(&arr);
    printf("\n");

    arr_double_destroy(&arr);
    printf("after destroy: size=%d, max=%d\n", arr.count, arr.max);

    printf("--------------\n");
    arr = arr_double_initn(12);
    for (int i = 0; i < 10; i++)
        arr_double_push(&arr, (double)(i + 0.5));
    arr_double_print(&arr);
    printf("\n");

    double get = arr_double_get(&arr, 2);
    printf("get(%d) = %lf\n", 2, get);
    arr_double_print(&arr);
    printf("\n");

    get = arr_double_get(&arr, -1);
    printf("get(%d) = %lf\n", -1, get);
    arr_double_print(&arr);
    printf("\n");

    get = arr_double_pop(&arr);
    printf("pop() = %lf\n", get);
    arr_double_print(&arr);
    printf("\n");

    get = arr_double_pop_front(&arr);
    printf("pop_front() = %lf\n", get);
    arr_double_print(&arr);
    printf("\n");

    arr_double_push_front(&arr, 15.2);
    printf("push_front(%lf)\n", 15.2);
    arr_double_print(&arr);
    printf("\n");

    arr_double_insert(&arr, 2, -2.2);
    printf("insert(%d, %lf)\n", 2, -2.2);
    arr_double_print(&arr);
    printf("\n");

    arr_double_destroy(&arr);

    arr_double lhs = arr_double_initn(3);
    for (int i = 0; i < 3; i++)
        arr_double_push(&lhs, (double)(i + 0.5));

    arr_double rhs = arr_double_initn(3);
    for (int i = 0; i < 3; i++)
        arr_double_push(&rhs, (double)(i*2 + 0.7));


    printf("CONCATENATION\n");
    printf("lhs = ");
    arr_double_print(&lhs);
    printf("\n");
    printf("rhs = ");
    arr_double_print(&rhs);
    printf("\n");
    printf("cct = ");
    arr_double_concatenate(&lhs, &rhs);
    arr_double_print(&lhs);
    printf("\n");

    arr_double_destroy(&lhs);
    arr_double_destroy(&rhs);
}
