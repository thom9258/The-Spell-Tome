#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#ifndef YAL_DEFS
#define YAL_DEFS

#include "alloc.h"

#define TSTR_IMPLEMENTATION
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

    TYPE_COUNT
};


/*Forward declarations*/
typedef struct expr expr;
typedef struct Environment Environment;

typedef expr*(*buildin_fn)(Environment*, expr*);

typedef struct {
    tstr name;
    buildin_fn fn;
}Buildin;
#define BUILDIN(name, fn) (Buildin) {tstr_const(name), fn}

#define t_type Buildin
#define arr_t_name Buildins
#include "tsarray.h"

struct Environment {
    Buildins buildins;
    sbAllocator expr_allocator;
    tstrBuffer stdout_buffer;
};

struct expr {
    char type;
    union {
        int real;
        float decimal;
        tstr string;
        tstr symbol;
        /*NOTE: Maybe direct buildin functions will exist in the future?
          with fns put into cell at lexing time*/
        // Buildin buildin;
    };
    struct expr* car;
    struct expr* cdr;
};

/*Global nil object, so we dont have to allocate a bunch of them at runtime*/
expr _NIL_OBJECT = (expr){TYPE_NIL, {0}, NULL, NULL};
expr* NIL = &_NIL_OBJECT;

/* =========================================================
 * Safety and Assertion on error
 * (To be replaced with proper error management & reporting)
 * */
#define UNUSED(x) (void)(x)
#define UNIMPLEMENTED(fun) assert(1 && "UNIMPLEMENTED: " && fun)

#define ERRCHECK(ccons, msg) assert((ccons) && msg)

#define ERRCHECK_UNREACHABLE() \
    ERRCHECK(0, "REACHED UNREACHABLE CODE!")

#define ERRCHECK_INV_ENV(env) \
    ERRCHECK(env != NULL, "Env Ptr is NULL!")

#define ERRCHECK_INV_ALLOC(ptr) \
    ERRCHECK(ptr != NULL, "Invalid pointer allocation!")

#define ERRCHECK_NIL(expr) \
    ERRCHECK(!is_nil(expr), "Given expr is NIL!")

#define ERRCHECK_LEN(expr, l)                         \
    ERRCHECK(len(expr) == l, "Given expr len is not expected!")

#define ERRCHECK_TYPECHECK(expr, expected)                             \
    do {                                                            \
        ERRCHECK_NIL(expr);                                            \
        ERRCHECK(expr->type == expected, "Expr cell has wrong type!"); \
} while (0)

#define ERRCHECK_NOTNUMBER(env, atom) \
    ERRCHECK(yal_atom(env, atom)->type == REAL || yal_atom(env, atom)->type == DECIMAL, \
               "Atom is not a number!")

#define ERRCHECK_ASTLEN(env, list, check) \
        ERRCHECK(yal_AtomList_len(&yal_atom(env, list)->AST) check, "AST len does not comply based on given operator and length!");   \

#endif /*YAL_DEFS*/
