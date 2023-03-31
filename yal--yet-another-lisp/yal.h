/*
## ORIGINAL AUTHOR

Thomas Alexgaard Jensen (![github/thom9258](https://github.com/thom9258))

## LICENSE

Copyright (c) Thomas Alexgaard Jensen
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
![Zlib License](https://choosealicense.com/licenses/zlib/)

## CHANGELOG
- [0.0] Initialized library.
- [0.1] Created AST structure and imported base lib functionality.
        Added marked lists and atoms.
        Fixed Evaluation function to work properly.
        Added write, math and equal functions
        Added remaining comparison operators.


## DEVELOPMENT NOTES

  Resource for functions and their workings:
  http://www.lispworks.com/documentation/HyperSpec/Front/X_Master.htm

  Notes on playing with lisp repl:
  - it seems that a list is always expected to contain a function symbol
    at index 0, unless the list is marked -> '(1 2 3)
    I think i can create this feature, so that

  TODO: Create a self-growing atom block allocator datastructure.
  TODO: Add support for keywords nil and t
  TODO: Math library prefixed with "math." in names, needs:
        - pi, 2pi, euler vars
        - sqrt and other functions
  TODO: Defineable variables and AST functions
  TODO: env variable Constants for version, atom types, etc.
  TODO: If statements and loops
  TODO: Error and Warning management build into the languange
  TODO: Go through math operators and make sure they are up to date with syntax.
  TODO: rename buildin inputhandles so "_args" for nicer reading.
  TODO: Replace older ast access with new macros
  TODO: Make a parse function that creates a default env and calls parse_AST on
        given AST
*/

#ifndef YAL_H
#define YAL_H

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define TSTRING_IMPLEMENTATION
#include "vendor/tstring.h"

#define TH_ALLOCATOR_IMPLEMENTATION
#include "vendor/th_allocator.h"

#define UNUSED(x) (void)(x)
#define UNIMPLEMENTED(fun) assert(1 && "UNIMPLEMENTED: " && fun)

#define YAL_INVALID_ATOM_HANDLE TH_INVALID_HANDLE

#ifndef YAL_AST_INITLEN
#define YAL_AST_INITLEN 3
#endif /*YAL_AST_INITLEN*/

/* =========================================================
 * Safety and Assertion on error
 * (To be replaced with proper error management & reporting)
 * */

#define YAL_ASSERT(expr, msg) assert((expr) && msg)

#define YAL_UNREACHABLE() \
    assert(1 && "CRITICAL ERROR: REACHED UNREACHABLE CODE!")

#define YAL_ASSERT_INV_ENV(env) \
    YAL_ASSERT(env != NULL, "Env Ptr is NULL!")

#define YAL_ASSERT_INV_HANDLE(env, atom) \
    YAL_ASSERT(yal_atom(env, atom) != NULL, "Atom uses an invalid handle!")

#define YAL_ASSERT_TYPECHECK(env, atom, expected) \
    YAL_ASSERT(yal_atom(env, atom)->type == expected, "Atom has wrong type!")

#define YAL_ASSERT_NOTNUMBER(env, atom) \
    YAL_ASSERT(yal_atom(env, atom)->type == YAL_REAL || yal_atom(env, atom)->type == YAL_DECIMAL, \
               "Atom is not a number!")

#define YAL_ASSERT_ASTLEN(env, list, check) \
        YAL_ASSERT(yal_AtomArray_len(&yal_atom(env, list)->AST) check, "AST len does not comply based on given operator and length!");   \

#define YAL_ASSERT_ZEROAST(env, h) \
    YAL_ASSERT_ASTLEN(env, h, > 0)

/* ======================================
 * AST & Atom Manipulation and management  
 * */

#define YAL_ATOM_NUMBER(atom_ptr) \
    ( (atom_ptr->type == YAL_REAL) ? atom_ptr->real : atom_ptr->decimal )

#define YAL_ATOM_GETNUMBER(env, atom) \
    YAL_ATOM_NUMBER(yal_atom(env, atom))

#define YAL_AST_INSERT(env, atomast, idx, push) \
    yal_AtomArray_insert(&yal_atom(env, atomast)->AST, idx, push)
#define YAL_AST_GET(env, atomast, idx) \
    yal_AtomArray_get(&yal_atom(env, atomast)->AST, idx)
#define YAL_AST_PUT(env, atomast, push) \
    yal_AtomArray_push(&yal_atom(env, atomast)->AST, push)
#define YAL_AST_PUT_FRONT(env, atomast, push) \
    yal_AtomArray_push_front(&yal_atom(env, atomast)->AST, push)
#define YAL_AST_POP(env, atomast) \
    yal_AtomArray_pop(&yal_atom(env, atomast)->AST)
#define YAL_AST_POP_FRONT(env, atomast) \
    yal_AtomArray_pop_front(&yal_atom(env, atomast)->AST)
#define YAL_AST_LEN(env, atomast) \
    yal_AtomArray_len(&yal_atom(env, atomast)->AST)
#define YAL_AST_PEEK(env, atomast, idx) \
    yal_AtomArray_peek(&yal_atom(env, atomast)->AST, idx)
#define YAL_AST_CONCAT(env, dst, src) \
    yal_AtomArray_concatenate(&yal_atom(env, dst)->AST, &yal_atom(env, src)->AST)

enum YAL_TYPE {
    YAL_TYPE_INVALID = 0,
    /*Atom types*/
    YAL_NIL,
    YAL_REAL,
    YAL_DECIMAL,
    YAL_SYMBOL,
    YAL_STRING,
    YAL_AST,

    /*Enviroment function/variable types*/
    YAL_ENV_BUILDIN_FN,
    YAL_ENV_VARIABLE,

    YAL_TYPE_COUNT
};

typedef struct yal_Environment yal_Environment;
typedef th_handle yal_AtomHandle;
#define t_type yal_AtomHandle
#define arr_t_name yal_AtomArray
#include "vendor/tsarray.h"

typedef struct {
    char type;
    char is_marked;
    union {
        int real;
        float decimal;
        tstring_s symbol;
        tstring_s string;
        yal_AtomArray AST;
    };
}yal_Atom;

typedef void(*yal_buildin_fn)(yal_Environment*, yal_AtomHandle);

typedef struct {
    char type;
    tstring_s symbol;
    tstring_s info;
    union {
        yal_AtomHandle Variable;
        yal_buildin_fn Builin;
    };
}yal_BuildinFn;
#define t_type yal_BuildinFn
#define arr_t_name yal_BuildinFnList
#include "vendor/tsarray.h"


struct yal_Environment {
    uint8_t* memory;
    th_blockallocator atom_allocator;
    yal_BuildinFnList known;
};

/*Environment management*/
yal_Environment yal_env_new(uint32_t _max_atoms);
void yal_env_delete(yal_Environment* _env);
void yal_env_add_buildin_fn(yal_Environment* _env, char* symbol, char* info, yal_buildin_fn _fn);
void yal_env_add_buildins(yal_Environment* _env);
yal_BuildinFn* yal_env_find_buildin(yal_Environment* _env, yal_AtomHandle _h);

/*Atom Types, Creation and Management*/
yal_Atom* yal_atom(yal_Environment* _env, yal_AtomHandle _h);
yal_AtomHandle yal_mark(yal_Environment* _env, yal_AtomHandle _h);
yal_AtomHandle yal_unmark(yal_Environment* _env, yal_AtomHandle _h);
yal_AtomHandle yal_AST_new(yal_Environment* _env);
yal_AtomHandle yal_AST_new_va(yal_Environment* _env, int _n, ...);
yal_AtomHandle yal_atom_real_new(yal_Environment* _env, int _v);
yal_AtomHandle yal_atom_decimal_new(yal_Environment* _env, float _v);
yal_AtomHandle yal_atom_symbol_new(yal_Environment* _env, char* _sym);
yal_AtomHandle yal_atom_string_new(yal_Environment* _env, char* _sym);
void yal_atom_clear(yal_Environment* _env, yal_AtomHandle _h);
void yal_atom_delete(yal_Environment* _env, yal_AtomHandle _h);

/*Atom Utility*/
void yal_atom_print(yal_Atom* _a);
char yal_atom_equal(yal_Atom* _a, yal_Atom* _b);

/*Evaluation and Debugging of the languane*/
void yal_print_chain(yal_Environment* _env, yal_AtomHandle _h);
void yal_eval(yal_Environment* _env, yal_AtomHandle _AST);

/******************************************************************************/
#define YAL_IMPLEMENTATION
#ifdef YAL_IMPLEMENTATION

yal_AtomHandle
_yal_atom_memory_alloc(yal_Environment* _env)
{
    YAL_ASSERT_INV_ENV(_env);
    yal_AtomHandle a = th_blockallocator_take(&_env->atom_allocator);
    *yal_atom(_env, a) = (yal_Atom){0};
    return a;
}

void
_yal_atom_memory_free(yal_Environment* _env, yal_AtomHandle _h)
{
    YAL_ASSERT_INV_ENV(_env);
    th_blockallocator_return(&_env->atom_allocator, _h);
    
}

yal_AtomHandle
_yal_yal_buildin_add_atom_to_env(yal_Environment* _env, yal_AtomHandle _this)
{
    YAL_ASSERT_INV_ENV(_env);
    UNIMPLEMENTED("add_atom_to_env");
    return _this;
}

void
_yal_buildin_write(yal_Environment* _env, yal_AtomHandle _this)
{
    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _this);
    YAL_ASSERT_TYPECHECK(_env, _this, YAL_AST);
    YAL_ASSERT_ASTLEN(_env, _this, == 1);
    yal_atom_print(yal_atom(_env, *YAL_AST_PEEK(_env, _this, 0)));
}

#define _YAL_ARIHMETIC_2(op, a, b)        \
do { \
    if (a->type == YAL_REAL && b->type == YAL_REAL) a->real op b->real; \
    else if (a->type == YAL_DECIMAL && b->type == YAL_DECIMAL) a->decimal op b->decimal; \
    else if (a->type == YAL_DECIMAL && b->type == YAL_REAL) a->decimal op b->real; \
    else if (a->type == YAL_REAL && b->type == YAL_DECIMAL) { \
        a->type = YAL_DECIMAL; a->decimal = a->real; a->decimal op b->decimal; } \
} while (0)

void
_yal_buildin_multiply(yal_Environment* _env, yal_AtomHandle _this)
{
    yal_AtomHandle res;
    yal_AtomHandle tmp;
    yal_Atom* ths;

    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _this);
    YAL_ASSERT_TYPECHECK(_env, _this, YAL_AST);
    YAL_ASSERT_ZEROAST(_env, _this);

    ths = yal_atom(_env, _this);
    res = yal_AtomArray_pop_front(&ths->AST);
    YAL_ASSERT_INV_HANDLE(_env, res);
    YAL_ASSERT_NOTNUMBER(_env, res);

    while (yal_AtomArray_len(&ths->AST) > 0) {
        /*Pop each atom and sum its value*/
        tmp = yal_AtomArray_pop_front(&ths->AST);
        YAL_ASSERT_INV_HANDLE(_env, tmp);
        YAL_ASSERT_NOTNUMBER(_env, tmp);
        _YAL_ARIHMETIC_2(*=, yal_atom(_env, res), yal_atom(_env, tmp));
        yal_atom_delete(_env, tmp);
    }
    yal_AtomArray_push(&ths->AST, res);
}

void
_yal_buildin_divide(yal_Environment* _env, yal_AtomHandle _this)
{
    yal_AtomHandle res;
    yal_AtomHandle tmp;
    yal_Atom* ths;

    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _this);
    YAL_ASSERT_TYPECHECK(_env, _this, YAL_AST);
    YAL_ASSERT_ZEROAST(_env, _this);

    ths = yal_atom(_env, _this);
    res = yal_AtomArray_pop_front(&ths->AST);
    YAL_ASSERT_INV_HANDLE(_env, res);
    YAL_ASSERT_NOTNUMBER(_env, res);

    while (yal_AtomArray_len(&ths->AST) > 0) {
        /*Pop each atom and sum its value*/
        tmp = yal_AtomArray_pop_front(&ths->AST);
        YAL_ASSERT_INV_HANDLE(_env, tmp);
        YAL_ASSERT_NOTNUMBER(_env, tmp);
        _YAL_ARIHMETIC_2(/=, yal_atom(_env, res), yal_atom(_env, tmp));
        yal_atom_delete(_env, tmp);
    }
    yal_AtomArray_push(&ths->AST, res);
}

void
_yal_buildin_plus(yal_Environment* _env, yal_AtomHandle _this)
{
/* http://www.lispworks.com/documentation/HyperSpec/Body/f_pl.htm#PL
 * */
    yal_AtomHandle res;
    yal_AtomHandle tmp;
    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _this);
    YAL_ASSERT_TYPECHECK(_env, _this, YAL_AST);
    if (YAL_AST_LEN(_env, _this) < 1) {
        /*TODO: Throw some warning here*/
        YAL_AST_PUT_FRONT(_env, _this, yal_atom_real_new(_env, 0));
        return;
    }
    res = YAL_AST_POP_FRONT(_env, _this);
    YAL_ASSERT_INV_HANDLE(_env, res);
    YAL_ASSERT_NOTNUMBER(_env, res);

    while (YAL_AST_LEN(_env, _this) > 0) {
        /*Pop each atom and sum its value*/
        tmp = YAL_AST_POP_FRONT(_env, _this);
        YAL_ASSERT_INV_HANDLE(_env, tmp);
        YAL_ASSERT_NOTNUMBER(_env, tmp);
        _YAL_ARIHMETIC_2(+=, yal_atom(_env, res), yal_atom(_env, tmp));
        yal_atom_delete(_env, tmp);
    }
    YAL_AST_PUT(_env, _this, res);
}

void
_yal_buildin_minus(yal_Environment* _env, yal_AtomHandle _this)
{
    yal_AtomHandle res;
    yal_AtomHandle tmp;
    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _this);
    YAL_ASSERT_TYPECHECK(_env, _this, YAL_AST);

    if (YAL_AST_LEN(_env, _this) < 1) {
        /*TODO: Throw some warning here*/
        YAL_AST_PUT_FRONT(_env, _this, yal_atom_real_new(_env, 0));
        return;
    }
    if (YAL_AST_LEN(_env, _this) < 2) {
        /* Minus can act as a negation operator too, meaning that:
           (- x) is the same as (* x -1) */
        YAL_AST_PUT(_env, _this, yal_atom_real_new(_env, -1));
        _yal_buildin_multiply(_env, _this);
        return;
    }
    res = YAL_AST_POP_FRONT(_env, _this);
    YAL_ASSERT_INV_HANDLE(_env, res);
    YAL_ASSERT_NOTNUMBER(_env, res);

    while (YAL_AST_LEN(_env, _this) > 0) {
        /*Pop each atom and sum its value*/
        tmp = YAL_AST_POP_FRONT(_env, _this);
        YAL_ASSERT_INV_HANDLE(_env, tmp);
        YAL_ASSERT_NOTNUMBER(_env, tmp);
        _YAL_ARIHMETIC_2(-=, yal_atom(_env, res), yal_atom(_env, tmp));
        yal_atom_delete(_env, tmp);
    }
    YAL_AST_PUT(_env, _this, res);
}

void
_yal_buildin_equal(yal_Environment* _env, yal_AtomHandle _args)
{
    yal_AtomHandle res;
    yal_AtomHandle a;
    yal_AtomHandle b;
    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _args);
    YAL_ASSERT_TYPECHECK(_env, _args, YAL_AST);
    YAL_ASSERT_ASTLEN(_env, _args, == 2);
    b = YAL_AST_POP(_env, _args); YAL_ASSERT_NOTNUMBER(_env, b);
    a = YAL_AST_POP(_env, _args); YAL_ASSERT_NOTNUMBER(_env, a);

    if (YAL_ATOM_GETNUMBER(_env, a) == YAL_ATOM_GETNUMBER(_env, b))
        res = yal_atom_real_new(_env, 1);
    else
        res = yal_atom_real_new(_env, 0);

    yal_atom_delete(_env, a); yal_atom_delete(_env, b);
    YAL_AST_PUT(_env, _args, res);
}

void
_yal_buildin_notequal(yal_Environment* _env, yal_AtomHandle _args)
{
    yal_AtomHandle res;
    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _args);
    YAL_ASSERT_TYPECHECK(_env, _args, YAL_AST);

    _yal_buildin_equal(_env, _args);
    res = YAL_AST_POP_FRONT(_env, _args);
    yal_atom(_env, res)->real = !(yal_atom(_env, res)->real);
    YAL_AST_PUT(_env, _args, res);
}

void
_yal_buildin_lessequal(yal_Environment* _env, yal_AtomHandle _args)
{
    yal_AtomHandle res;
    yal_AtomHandle a;
    yal_AtomHandle b;
    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _args);
    YAL_ASSERT_TYPECHECK(_env, _args, YAL_AST);
    YAL_ASSERT_ASTLEN(_env, _args, == 2);
    b = YAL_AST_POP(_env, _args); YAL_ASSERT_NOTNUMBER(_env, b);
    a = YAL_AST_POP(_env, _args); YAL_ASSERT_NOTNUMBER(_env, a);

    if (YAL_ATOM_GETNUMBER(_env, a) <= YAL_ATOM_GETNUMBER(_env, b))
        res = yal_atom_real_new(_env, 1);
    else
        res = yal_atom_real_new(_env, 0);

    yal_atom_delete(_env, a); yal_atom_delete(_env, b);
    YAL_AST_PUT(_env, _args, res);
}

void
_yal_buildin_greaterequal(yal_Environment* _env, yal_AtomHandle _args)
{
    yal_AtomHandle res;
    yal_AtomHandle a;
    yal_AtomHandle b;
    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _args);
    YAL_ASSERT_TYPECHECK(_env, _args, YAL_AST);
    YAL_ASSERT_ASTLEN(_env, _args, == 2);
    b = YAL_AST_POP(_env, _args); YAL_ASSERT_NOTNUMBER(_env, b);
    a = YAL_AST_POP(_env, _args); YAL_ASSERT_NOTNUMBER(_env, a);

    if (YAL_ATOM_GETNUMBER(_env, a) >= YAL_ATOM_GETNUMBER(_env, b))
        res = yal_atom_real_new(_env, 1);
    else
        res = yal_atom_real_new(_env, 0);

    yal_atom_delete(_env, a); yal_atom_delete(_env, b);
    YAL_AST_PUT(_env, _args, res);
}

void
_yal_buildin_greaterthan(yal_Environment* _env, yal_AtomHandle _args)
{
    yal_AtomHandle res;
    yal_AtomHandle a;
    yal_AtomHandle b;
    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _args);
    YAL_ASSERT_TYPECHECK(_env, _args, YAL_AST);
    YAL_ASSERT_ASTLEN(_env, _args, == 2);
    b = YAL_AST_POP(_env, _args); YAL_ASSERT_NOTNUMBER(_env, b);
    a = YAL_AST_POP(_env, _args); YAL_ASSERT_NOTNUMBER(_env, a);

    if (YAL_ATOM_GETNUMBER(_env, a) > YAL_ATOM_GETNUMBER(_env, b))
        res = yal_atom_real_new(_env, 1);
    else
        res = yal_atom_real_new(_env, 0);

    yal_atom_delete(_env, a); yal_atom_delete(_env, b);
    YAL_AST_PUT(_env, _args, res);
}

void
_yal_buildin_lessthan(yal_Environment* _env, yal_AtomHandle _args)
{
    yal_AtomHandle res;
    yal_AtomHandle a;
    yal_AtomHandle b;
    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _args);
    YAL_ASSERT_TYPECHECK(_env, _args, YAL_AST);
    YAL_ASSERT_ASTLEN(_env, _args, == 2);
    b = YAL_AST_POP(_env, _args); YAL_ASSERT_NOTNUMBER(_env, b);
    a = YAL_AST_POP(_env, _args); YAL_ASSERT_NOTNUMBER(_env, a);

    if (YAL_ATOM_GETNUMBER(_env, a) < YAL_ATOM_GETNUMBER(_env, b))
        res = yal_atom_real_new(_env, 1);
    else
        res = yal_atom_real_new(_env, 0);

    yal_atom_delete(_env, a); yal_atom_delete(_env, b);
    YAL_AST_PUT(_env, _args, res);
}

yal_Environment
yal_env_new(uint32_t _max_atoms)
{
    th_status ok;
    yal_Environment env = {0};

    if (_max_atoms == 0) _max_atoms = 128;

    env.memory = (uint8_t*)malloc(TH_BLOCKALLOCATOR_OPTIMAL_MEMSIZE(_max_atoms, sizeof(yal_Atom)));
    assert(env.memory != NULL);

    ok = th_blockallocator_setup(&env.atom_allocator, env.memory, sizeof(yal_Atom), _max_atoms);
    assert(ok == TH_OK);

    yal_BuildinFnList_initn(&env.known, 10);
    return env;
}

void
yal_env_delete(yal_Environment* _env)
{
    YAL_ASSERT_INV_ENV(_env);
    th_blockallocator_destroy(&_env->atom_allocator);
    free(_env->memory);

    yal_BuildinFnList_destroy(&_env->known);
}

void yal_env_add_BuildinFn(yal_Environment* _env, char* symbol, char* info, yal_buildin_fn _fn)
{
    YAL_ASSERT_INV_ENV(_env);
    yal_BuildinFn v;
    v.type = YAL_ENV_BUILDIN_FN;
    v.info = tstring(info);
    v.symbol = tstring(symbol);
    v.Builin = _fn;
    yal_BuildinFnList_push(&_env->known, v);
}

void
yal_env_add_buildins(yal_Environment* _env)
{
    /*Comparison Operators*/
    yal_env_add_BuildinFn(_env, "==", "Equality operator.", _yal_buildin_equal);
    yal_env_add_BuildinFn(_env, "!=", "Non-equality operator.", _yal_buildin_notequal);
    yal_env_add_BuildinFn(_env, "<=", "Less-Equal operator.", _yal_buildin_lessequal);
    yal_env_add_BuildinFn(_env, ">=", "Greater-Equal operator.", _yal_buildin_greaterequal);
    yal_env_add_BuildinFn(_env, "<", "Less-Then operator.", _yal_buildin_lessthan);
    yal_env_add_BuildinFn(_env, ">", "Greater-Than operator.", _yal_buildin_greaterthan);

    /*Math Operators*/
    yal_env_add_BuildinFn(_env, "+", "Plus operator.", _yal_buildin_plus);
    yal_env_add_BuildinFn(_env, "-", "Minus operator.", _yal_buildin_minus);
    yal_env_add_BuildinFn(_env, "*", "Multiply operator.", _yal_buildin_multiply);
    yal_env_add_BuildinFn(_env, "/", "Divide operator.", _yal_buildin_divide);

    /*Standard Functions*/
    yal_env_add_BuildinFn(_env, "write", "Atom print", _yal_buildin_write);
}

yal_BuildinFn*
yal_env_find_buildin(yal_Environment* _env, yal_AtomHandle _h)
{
    int i;
    int imax;
    yal_BuildinFn* tmp;
    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _h);

    imax = yal_BuildinFnList_len(&_env->known);
    for (i = 0; i < imax; i++) {
         tmp = yal_BuildinFnList_peek(&_env->known, i);
         if (tstring_equal(&yal_atom(_env, _h)->symbol, &tmp->symbol))
             return tmp;
    }
    YAL_ASSERT(0, "Tried to access buildin function that did not exist!");
}

yal_Atom*
yal_atom(yal_Environment* _env, yal_AtomHandle _h)
{
    YAL_ASSERT_INV_ENV(_env);
    return TH_BLOCK_ACCESS(&_env->atom_allocator, _h, yal_Atom);
}

yal_AtomHandle
yal_mark(yal_Environment* _env, yal_AtomHandle _h)
{
    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _h);
    yal_atom(_env, _h)->is_marked = 1;
    return _h;
}


yal_AtomHandle
yal_unmark(yal_Environment* _env, yal_AtomHandle _h)
{
    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _h);
    return yal_atom(_env, _h)->is_marked = 0;
}

void
yal_atom_print(yal_Atom* _a)
{
    if (_a == NULL)
        YAL_UNREACHABLE();

    if (_a->is_marked)
        printf("#");
    switch (_a->type) {
    case YAL_REAL:
        printf("%d", _a->real);
        break;
    case YAL_NIL:
        printf("NIL");
        break;
    case YAL_DECIMAL:
        printf("%f", _a->decimal);
        break;
    case YAL_SYMBOL:
        printf("%s", _a->symbol.c_str);
        break;
    case YAL_STRING:
        printf("\"%s\"", _a->string.c_str);
        break;
    case YAL_AST:
        /*FALLTHROUGH*/
    default:
        YAL_UNREACHABLE();
    }
}

yal_AtomHandle
yal_atom_nil_new(yal_Environment* _env)
{
    YAL_ASSERT_INV_ENV(_env);
    yal_AtomHandle h = _yal_atom_memory_alloc(_env);
    YAL_ASSERT_INV_HANDLE(_env, h);
    yal_atom(_env, h)->type = YAL_NIL;
    return h;
}

yal_AtomHandle
yal_atom_real_new(yal_Environment* _env, int _v)
{
    YAL_ASSERT_INV_ENV(_env);
    yal_AtomHandle h = _yal_atom_memory_alloc(_env);
    YAL_ASSERT_INV_HANDLE(_env, h);
    yal_atom(_env, h)->type = YAL_REAL;
    yal_atom(_env, h)->real = _v;
    return h;
}

yal_AtomHandle
yal_atom_decimal_new(yal_Environment* _env, float _v)
{
    YAL_ASSERT_INV_ENV(_env);
    yal_AtomHandle h = _yal_atom_memory_alloc(_env);
    YAL_ASSERT_INV_HANDLE(_env, h);
    yal_atom(_env, h)->type = YAL_DECIMAL;
    yal_atom(_env, h)->decimal = _v;
    return h;
}
   
yal_AtomHandle
yal_atom_symbol_new(yal_Environment* _env, char* _sym)
{
    YAL_ASSERT_INV_ENV(_env);
    yal_AtomHandle h = _yal_atom_memory_alloc(_env);
    YAL_ASSERT_INV_HANDLE(_env, h);
    assert(yal_atom(_env, h) != NULL);
    yal_atom(_env, h)->type = YAL_SYMBOL;
    yal_atom(_env, h)->symbol = tstring(_sym);
    return h;
}

yal_AtomHandle
yal_atom_string_new(yal_Environment* _env, char* _sym)
{
    YAL_ASSERT_INV_ENV(_env);
    yal_AtomHandle h = _yal_atom_memory_alloc(_env);
    YAL_ASSERT_INV_HANDLE(_env, h);
    assert(yal_atom(_env, h) != NULL);
    yal_atom(_env, h)->type = YAL_STRING;
    yal_atom(_env, h)->symbol = tstring(_sym);
    return h;
}

yal_AtomHandle
yal_AST_new(yal_Environment* _env)
{
    YAL_ASSERT_INV_ENV(_env);
    yal_AtomHandle h = _yal_atom_memory_alloc(_env);
    YAL_ASSERT_INV_HANDLE(_env, h);
    yal_atom(_env, h)->type = YAL_AST;
    yal_AtomArray_initn(&yal_atom(_env, h)->AST, YAL_AST_INITLEN);
    return h;
}


yal_AtomHandle
yal_AST_new_va(yal_Environment* _env, int _n, ...)
{
    int i;
    va_list va;                     
    yal_AtomHandle ast = yal_AST_new(_env);
    va_start(va, _n);           
    for (i = 0; i < _n; i++)
        YAL_AST_PUT(_env, ast, va_arg(va, yal_AtomHandle)); 
    va_end(va);
    return ast;
}

void
yal_atom_clear(yal_Environment* _env, yal_AtomHandle _h)
{
    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _h);
    yal_Atom* p = yal_atom(_env, _h);
    if (p == NULL)
        return;

    /*Cleanup internal memory of associated type*/
    switch (p->type) {
    case YAL_SYMBOL:
        tstring_destroy(&p->symbol);
        break;
    case YAL_STRING:
        tstring_destroy(&p->string);
        break;
    case YAL_AST:
        assert(yal_AtomArray_len(&p->AST) == 0);
        yal_AtomArray_destroy(&p->AST);
        break;
    case YAL_NIL:
    case YAL_REAL:
    case YAL_DECIMAL:
        break;
    default:
        assert(1 && "Unreachable");
    }
}

char
yal_atom_equal(yal_Atom* _a, yal_Atom* _b)
{
    return (YAL_ATOM_NUMBER(_a) == YAL_ATOM_NUMBER(_b));
}

void
yal_atom_delete(yal_Environment* _env, yal_AtomHandle _h)
{
    if (_env == NULL)
        return;
    YAL_ASSERT_INV_HANDLE(_env, _h);
    yal_atom_clear(_env, _h);
    _yal_atom_memory_free(_env, _h);
}

void
yal_print_chain(yal_Environment* _env, yal_AtomHandle _h)
{
    int i;
    int imax;
    yal_Atom* p = yal_atom(_env, _h);
    yal_AtomHandle tmp;

    UNUSED(imax);
    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _h);

    if (p->type == YAL_AST) {
        printf("%s ", (p->is_marked) ? "#[":"(");
        //imax = yal_AtomArray_len(&p->AST);
        for (i = 0; i < yal_AtomArray_len(&p->AST); i++) {
            tmp = *yal_AtomArray_peek(&p->AST, i);
            YAL_ASSERT_INV_HANDLE(_env, tmp);
            yal_print_chain(_env, tmp);
        }
        printf("%s ", (p->is_marked) ? "]":")");
    } else {
        yal_atom_print(p);
        printf(" ");
    }
}

void
_yal_parse_symbol(yal_Environment* _env, yal_AtomHandle _h)
{
    yal_BuildinFn* var;
    var = yal_env_find_buildin(_env, _h);

    YAL_ASSERT(var->type != YAL_ENV_VARIABLE, "FOUND FUNCTION AS NON-FIRST ATOM IN LIST");
    
    /*Replace sym with value of enviroment symbol*/
    yal_atom_clear(_env, _h);
    *yal_atom(_env, _h) = *yal_atom(_env, var->Variable);
}

char
_yal_has_nonvalue(yal_Environment* _env, yal_AtomHandle _ast)
{
    int i;
    int imax;
    yal_AtomHandle tmp;
    YAL_ASSERT_INV_HANDLE(_env, _ast);
    YAL_ASSERT_TYPECHECK(_env, _ast, YAL_AST);
    YAL_ASSERT_ZEROAST(_env, _ast);
    imax = YAL_AST_LEN(_env, _ast);

    for (i = 0; i < imax; i++) {
        tmp = *YAL_AST_PEEK(_env, _ast, i);
        if (yal_atom(_env, tmp)->type != YAL_REAL &&
            yal_atom(_env, tmp)->type != YAL_DECIMAL)
            return 1;
    }
    return 0;
}

void
yal_eval(yal_Environment* _env, yal_AtomHandle _ast)
{
    /*TODO: Should be uptimizable using concatenate & smarter list manip.*/
    yal_AtomHandle fn = YAL_INVALID_ATOM_HANDLE;
    yal_AtomHandle tmp;
    yal_BuildinFn* buildin;
    yal_AtomHandle res;

    YAL_ASSERT_INV_HANDLE(_env, _ast);
    YAL_ASSERT_TYPECHECK(_env, _ast, YAL_AST);
    YAL_ASSERT_ZEROAST(_env, _ast);
    if (yal_atom(_env, _ast)->is_marked)
        return;

    res = yal_AST_new(_env);
    YAL_ASSERT_INV_HANDLE(_env, res);

    if (!yal_atom(_env, *YAL_AST_PEEK(_env, _ast, 0))->is_marked) {
        fn = YAL_AST_POP_FRONT(_env, _ast);
        YAL_ASSERT_INV_HANDLE(_env, fn);
    }

    /*Construct a fn input AST based on contents of given AST*/
    while (YAL_AST_LEN(_env, _ast) > 0) {
        tmp = YAL_AST_POP_FRONT(_env, _ast);
        YAL_ASSERT_INV_HANDLE(_env, tmp);

        if (yal_atom(_env, tmp)->is_marked) {
            YAL_AST_PUT(_env, res, tmp);
            continue;
        }
        switch (yal_atom(_env, tmp)->type) {
        case YAL_AST:
            /*Evaluate internal AST and unravel result if nessecary*/
            yal_eval(_env, tmp);
            if (yal_atom(_env, tmp)->is_marked)
                YAL_AST_PUT(_env, res, tmp);
            else 
                YAL_AST_CONCAT(_env, res, tmp);
            break;
        case YAL_SYMBOL: 
            /*Replace Symbols with copies of env variables*/
            _yal_parse_symbol(_env, tmp);
            /*FALLTHROUGH*/
        case YAL_DECIMAL: 
            /*FALLTHROUGH*/
        case YAL_REAL: 
            /*FALLTHROUGH*/
        case YAL_STRING: 
            YAL_AST_PUT(_env, res, tmp);
            break;
        default:
            YAL_UNREACHABLE();
            break;
        };
    }
    /*Evaluate the list function and replace input with function output*/
    if (fn != YAL_INVALID_ATOM_HANDLE) {
        buildin = yal_env_find_buildin(_env, fn);
        buildin->Builin(_env, res);
        yal_atom_delete(_env, fn);
    }

    yal_atom_clear(_env, _ast);
    *yal_atom(_env, _ast) = *yal_atom(_env, res);
}

#endif /*YAL_IMPLEMENTATION*/
#endif /*YAL_H*/
