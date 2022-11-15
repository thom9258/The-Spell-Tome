#include <stdio.h>

typedef struct {
    char type;
    union {
        int   val_int;
        float val_float;
        char* val_cstr;
    };
}atom;

void
atom_print(atom _a)
{
    switch(_a.type) {
    case 1:
        printf("%d\n", _a.val_int);
        break;
    case 2:
        printf("%f\n", _a.val_float);
        break;
    case 3:
        if (_a.val_cstr == NULL) printf("(nullptr)\n");
        else printf("%s\n", _a.val_cstr);
        break;
    default:
        printf("{atom was unprintable}\n");
        break;
    }
}

#define t_type atom
#define t_operator_print(a) atom_print(a)
#include "../tsarray.h"

atom
atom_init(char _type)
{
    atom out = {0};
    out.type = _type;
    switch(out.type) {
    case 1:
        out.val_int = 0;
        break;
    case 2:
        out.val_float = 0.0f;
        break;
    case 3:
        out.val_cstr = NULL;
        break;
    default:
        out.type = 0;
        break;
    }
    return out;
}

void
test_atom(void)
{
    printf("Atom test --------------\n");
    arr_atom arr = {0};
    arr = arr_atom_initn(5);

    arr_atom_push(&arr, atom_init(1));
    arr_atom_push(&arr, atom_init(3));
    arr_atom_push(&arr, atom_init(25));
    arr_atom_push(&arr, atom_init(2));

    printf("size=%d, max=%d\n", arr.count, arr.max);
    atom pop = arr_atom_pop(&arr);
    printf("popped a value (type=%d)\n", pop.type);
    printf("size=%d, max=%d\n", arr.count, arr.max);

    printf("printing array using build-in operator:\n");
    arr_atom_print(&arr);

    arr_atom_destroy(&arr);
    printf("after destroy: size=%d, max=%d\n", arr.count, arr.max);
}


