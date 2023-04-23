#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#ifndef YAL_DEFS
#define YAL_DEFS

#include "alloc.h"
#include "error.h"

#define TSTR_IMPLEMENTATION
#include "tstr.h"

enum TYPE {
    TYPE_NIL = 0,
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
    ErrorManager errormanager;
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
#define UNIMPLEMENTED(fun) \
    assert(0 && "UNIMPLEMENTED: " && fun)

#define ASSERT_UNREACHABLE() \
    assert(0 && "YAL: Fatal Error occoured, reached unreachable code!")

#define ASSERT_INV_ENV(env) \
    assert(env != NULL && "Invalid environment is used in code! Ptr is NULL!")

#define ERRCHECK_NILALLOC(env, expr) \
    do { if (is_nil(expr)) { \
             Error_put(&env->errormanager, \
                       ERROR(ERROR_ERROR, (char*)"Was unable to allocate new expr cell!")); \
        }} while (0)


#define ERRCHECK_NIL(env, expr)     \
    do { if (is_nil(expr)) { \
             Error_put(&env->errormanager, \
                        ERROR(ERROR_ERROR, (char*)"Input expr is NIL!")); \
        }} while (0)

#define ERRCHECK_LEN(env, expr, expected)        \
    do { if (len(expr) != expected) { \
             Error_put(&env->errormanager, \
                        ERROR(ERROR_ERROR, (char*)"Length of  expr list is not expected!")); \
        }} while (0)

#define ERRCHECK_TYPE(env, expr, expected)       \
    do { if (expr->type != expected) { \
             Error_put(&env->errormanager, \
                        ERROR(ERROR_ERROR, (char*)"expr has invalid type!")); \
        }} while (0)

#define ERRCHECK_NOTNUMBER(expr) \
    do { if (expr->type != TYPE_REAL && expr->type != TYPE_DECIMAL) { \
             Error_put(&env->errormanager, \
                        ERROR(ERROR_ERROR, (char*)"Given expr is not a number!")); \
        }} while (0)

#endif /*YAL_DEFS*/
