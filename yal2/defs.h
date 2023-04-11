#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#ifndef YAL_DEFS
#define YAL_DEFS

#include "alloc.h"

#define TSTRING_IMPLEMENTATION
#include "tstr.h"

enum TYPE {
    TYPE_INVALID = 0,
    /*Atom types*/
    TYPE_NIL,
    TYPE_CCONS,
    TYPE_REAL,
    TYPE_DECIMAL,
    TYPE_SYMBOL,
    TYPE_STRING,
    TYPE_BUILDIN,

    TYPE_COUNT
};

typedef struct {
    /*TODO: Start using the environment as ccons storage, and implement buildin lut*/
    sbAllocator expr_allocator;
    tstrBuffer stdout_buffer;
}Environment;

typedef struct expr expr;
typedef expr*(*buildin_fn)(Environment*, expr*);

struct expr{
    char type;
    union {
        int real;
        float decimal;
        tstr string;
        tstr symbol;
        struct {
            tstr name;
            buildin_fn fn;
        }buildin;
    };
    //int handle;
    struct expr* car;
    struct expr* cdr;
};

/* =========================================================
 * Safety and Assertion on error
 * (To be replaced with proper error management & reporting)
 * */
#define UNUSED(x) (void)(x)
#define UNIMPLEMENTED(fun) assert(1 && "UNIMPLEMENTED: " && fun)

#define ERROR(ccons, msg) assert((ccons) && msg)

#define ERROR_UNREACHABLE() \
    assert(1 && "CRITICAL ERROR: REACHED UNREACHABLE CODE!")

#define ERROR_INV_ENV(env) \
    ERROR(env != NULL, "Env Ptr is NULL!")

#define ERROR_INV_ALLOC(ptr) \
    ERROR(ptr != NULL, "Invalid pointer allocation!")

#define ERROR_TYPECHECK(env, atom, expected) \
    ERROR(yal_atom(env, atom)->type == expected, "Atom has wrong type!")

#define ERROR_NOTNUMBER(env, atom) \
    ERROR(yal_atom(env, atom)->type == REAL || yal_atom(env, atom)->type == DECIMAL, \
               "Atom is not a number!")

#define ERROR_ASTLEN(env, list, check) \
        ERROR(yal_AtomList_len(&yal_atom(env, list)->AST) check, "AST len does not comply based on given operator and length!");   \

#define ERROR_NIL(expr) \
    ERROR(!is_nil(expr), "Given expression is NIL!")

#endif /*YAL_DEFS*/
