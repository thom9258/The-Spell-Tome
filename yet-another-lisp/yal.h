/*
## ORIGINAL AUTHOR

Thomas Alexgaard Jensen (![github/thom9258](https://github.com/thom9258))

## LICENSE

Copyright (c) Thomas Alexgaard Jensen
This software is provided 'as-is', without any cconsess or implied warranty.
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
- [0.2] Implemented a lexer for reading of s expressions.
        Cleaning up impl and purging unused stuff
        fixed lexer errors with proper tokenization.
        Fixed bug when lexing numbers starting with 9 and 0
        Added env_add_constant to setup.
        Fixed tons of errors,
        - error management
        - variable lookup
        - variable defines
        Fixed Tokenization of quotes and arrays
        Added most of the standard functions that needs to be implemented.
- [0.1] Created AST structure and imported base lib functionality.
- [0.0] Initialized library.
*/

/*
https://buildyourownlisp.com/chapter9_s_expressions#lists_and_lisps
http://www.ulisp.com/show?1BLW
*/

/*NOTE: If i dont know how to return i can always throw an exception like
  NORMAL_RETURN, and treat it as a non-exception msg that says stop to
  functions like 'do' etc.
*/

/*Major todos*/
/*TODO: Garbage collection (with info stats)*/
/*TODO: Conditionals*/

/*Minor todos*/
/*TODO: string library functions*/
/*TODO: create try, catch and throw functions*/
/*TODO: Fix binding by adding support for &rest and keyword binds aswell*/
/*TODO: Make sure all types have type? checkers*/
/*TODO: Make sure the correct errors are returned*/
/*TODO: set! function that throws an error if set is used on constant*/
/*TODO: print ' and [] instead of quote and array*/
/*TODO: terminal repl binary*/
/*TODO: put! and pop! functions*/
/*TODO: reverse and reverse! functions*/
/*TODO: Replace raw v->car = val access with setcar(v, val) access for safety*/

#ifndef YAL_H
#define YAL_H

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#include "vendor/alloc.h"
#include "vendor/error.h"

#define TSTR_IMPLEMENTATION
#include "vendor/tstr.h"

// "Any sufficiently complicated C or Fortran program contains an ad hoc, informally-specified, bug-ridden, slow implementation of half of Common Lisp."
// -  Greenspun, Philip
#define WITH_STRINGEXPR(name, expr, body)                       \
    do {                                                        \
        tstr name = stringify(expr, (char*)"(", (char*)")");    \
        body;                                                   \
        tstr_destroy(&name);                                    \
    } while (0)

#define DBPRINT(before, expr) \
    WITH_STRINGEXPR(dbexpr, (expr), printf(before); printexpr(expr);  printf("\n");)

enum TYPE {
    TYPE_CONS = 0,
    TYPE_REAL,
    TYPE_DECIMAL,
    TYPE_SYMBOL,
    TYPE_STRING,
    TYPE_FUNCTION,
    TYPE_LAMBDA,
    TYPE_MACRO,
    TYPE_ERROR,
    TYPE_VECTOR,
    TYPE_DICTIONARY,

    TYPE_COUNT
};

enum EXCEPTION {
    EXCEPTION_OK = 0,
    EXCEPTION_UNKNOWN,
    EXCEPTION_INVALIDARGEVAL,
    EXCEPTION_INVALIDINPUT,
    EXCEPTION_CANTSETCONST,
    EXCEPTION_EVAL,
    EXCEPTION_SYMNOTFOUND,
    EXCEPTION_FNNOTFOUND,
    EXCEPTION_NOTIMPLEMENTED,
    EXCEPTION_NOTAVALUE,

    EXCEPTION_COUNT
};

struct expr {
    uint8_t type;
    union {
        /*Value atom*/
        union {
            int real;
            float decimal;
            tstr string;
            tstr symbol;
            struct {
                struct expr* binds;
                struct expr* body;
            }callable;
        };
        /*Cons atom*/
        struct {
            struct expr* car;
            struct expr* cdr;
        };
    };
};
typedef struct expr expr;

typedef struct {
    char type;
    tstr msg;
}Exception;

typedef struct {
    tstr symbol;
    expr* value;
}Variable;

#define t_type Variable
#define arr_t_name Variables
#include "vendor/tsarray.h"

/*Forward declarations*/
typedef struct Environment Environment;
typedef struct VariableScope VariableScope;
typedef expr*(*buildin_fn)(Environment*, VariableScope*, Exception*, expr*);

struct VariableScope {
    VariableScope* outer;
    Variables variables;
};

#define t_type VariableScope
#define arr_t_name VariableScopes
#include "vendor/tsarray.h"

typedef struct {
    tstr name;
    buildin_fn fn;
}Buildin;

#define t_type Buildin
#define arr_t_name Buildins
#include "vendor/tsarray.h"


struct Environment {
    Buildins buildins;
    sbAllocator variable_allocator;
    VariableScope constants;
    VariableScope globals;
    VariableScopes active_scopes;
};

/* =========================================================
 * Safety and Assertion on error
 * */
#define UNUSED(x) (void)(x)
#define UNIMPLEMENTED(fun) assert(0 && "UNIMPLEMENTED: " && fun)

#define THROW_ARGEVALFAIL(msgdst, fnsym, args)             \
    _throw_error(msgdst, fnsym, EXCEPTION_INVALIDARGEVAL, "Could not evaluate arguments.", args)

#define THROW_INVALIDINPUT(msgdst, fnsym, expected, args)                     \
    _throw_error(msgdst, fnsym, EXCEPTION_INVALIDINPUT, expected, args)

#define THROW_INVALIDSYMBOL(msgdst, fnsym, args)               \
    _throw_error(msgdst, fnsym, EXCEPTION_SYMNOTFOUND, "Symbol does not exist", args)

#define THROW_INVALIDFUNCTION(msgdst, fnsym, args)               \
    _throw_error(msgdst, fnsym, EXCEPTION_FNNOTFOUND, "Function does not exist", args)

#define THROW_INVALIDTYPE(msgdst, fnsym, type, args)                    \
    _throw_error(msgdst, fnsym, EXCEPTION_NOTAVALUE, "Invalid type, expected " #type, args)

#define THROW_NONVALUEARG(msgdst, fnsym, args)               \
    _throw_error(msgdst, fnsym, EXCEPTION_NOTAVALUE, "Not a value", args)

#define THROW_NOTIMPLEMENTED(msgdst, fnsym)               \
    _throw_error(msgdst, fnsym, EXCEPTION_NOTIMPLEMENTED, "called function is not implemented", NIL())

#define RETURN_ON_EXCEPTION(msgdst, ret) \
    do { if (Exception_is_error(msgdst)) return ret; } while (0)


#define ASSERT_UNREACHABLE() \
    assert(0 && "Fatal Error occoured, reached unreachable code!")

#define ASSERT_NULL(ptr, infostr)                                               \
    assert(ptr != NULL && "Invalid " infostr " is used in code! Ptr is NULL!")

#define ASSERT_INV_SCOPE(scope) \
    ASSERT_NULL(scope, "VariableScope*")

#define ASSERT_INV_ENV(env) \
    ASSERT_NULL(env, "Environment*")

#define ASSERT_NOMOREMEMORY() \
    assert(0 && "Was unable to allocate more memory!")

#define RESULT_NOT_OK(RESULT) (((RESULT).type != RESTYPE_OK) ? 1 : 0)

/*Error Management*/
const char* Exception_to_str(int _e);
char Exception_is_error(Exception* _dst);
void Exception_destroy(Exception* _msg);

/*Environment Management*/
Environment* Env_new(Environment* _env);
void Env_destroy(Environment* _env);
void Env_add_buildin(Environment* _env, const char* _name, buildin_fn _fn);
void Env_add_constant(Environment* _env, const char* _name, expr* _value);
void Env_add_global(Environment* _env, const char* _name, expr* _value);
void Env_add_variable(Environment* _env, VariableScope* _scope, const char* _name, expr* value);
void Env_add_core(Environment* _env);

/*Scope Management*/
VariableScope* VariableScope_new(Environment* _env, VariableScope* _this, VariableScope* _outer);
void VariableScope_destroy(Environment* _env, VariableScope* _scope);

/*Global Vars*/
expr* NIL(void);

/*Checks*/
char is_nil(expr* _args);
char is_cons(expr* _args);
char is_symbol(expr* _args);
char is_val(expr* _args);
char is_string(expr* _args);

/*List management*/
int len(expr* _args);
expr* car(expr* _args);
expr* cdr(expr* _args);
expr* first(expr* _args); 
expr* second(expr* _args);
expr* third(expr* _args);
expr* fourth(expr* _args);
expr* nth(int _n, expr* _args);

/*Printing*/
void printexpr(expr* _args);
tstr stringify(expr* _args, const char* _open, const char* _close);

/*Value creation*/
void variable_delete(Environment* _env, expr* _atom);
const char* type_to_str(int _e);
expr* cons(Environment* _env, expr* _car, expr* _cdr);
expr* real(Environment* _env, int _v);
expr* decimal(Environment* _env, float _v);
expr* symbol(Environment* _env, char* _v);
expr* string(Environment* _env, char* _v);

/*Core*/
expr* read(Environment* _env, VariableScope* _scope, Exception* _throwdst, char* _program_str);
expr* eval_expr(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* list(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);

/*Buildins*/
expr* buildin_read(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_quote(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_cdr(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_car(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_cons(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_list(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_len(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_first(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_second(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_third(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_fourth(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_nth(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_range(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_plus(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_minus(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_multiply(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_divide(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_mathequal(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_simequal(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_defconst(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_defglobal(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_defvar(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_set(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_pop(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_put(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_reverse(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_do(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_apply(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);


/******************************************************************************/
#define YAL_IMPLEMENTATION
#ifdef YAL_IMPLEMENTATION

void
_throw_error(Exception* _dst,
             const char* _fnsym,
             char _type,
             const char* _msg,
             expr* _expr)
{
    UNUSED(_fnsym);
    tstr msg;
    if (_dst == NULL)
        return;

    WITH_STRINGEXPR(str, _expr,
                    msg = tstr_fmt("%s, %s\n", _msg, str.c_str);
                    tstr_destroy(&_dst->msg);
                    _dst->type = _type;
                    _dst->msg = msg;
        );
}

const char*
Exception_to_str(int _e)    
{
    switch (_e) {
    case EXCEPTION_OK:
        return "EXCEPTION_OK";
    case EXCEPTION_UNKNOWN:
        return "EXCEPTION_UNKNOWN";
    case EXCEPTION_INVALIDARGEVAL:
        return "EXCEPTION_INVALIDARGEVAL";
    case EXCEPTION_INVALIDINPUT:
        return "EXCEPTION_INVALIDINPUT";
    case EXCEPTION_CANTSETCONST:
        return "EXCEPTION_CANTSETCONST";
    case EXCEPTION_EVAL:
        return "EXCEPTION_EVAL";
    case EXCEPTION_SYMNOTFOUND:
        return "EXCEPTION_SYMNOTFOUND";
    case EXCEPTION_FNNOTFOUND:
        return "EXCEPTION_FNNOTFOUND";
    case EXCEPTION_NOTIMPLEMENTED:
        return "EXCEPTION_NOTIMPLEMENTED";
    case EXCEPTION_NOTAVALUE:
        return "EXCEPTION_NOTAVALUE";
    default:
        ASSERT_UNREACHABLE();
    };
    return NULL;
}

char
Exception_is_error(Exception* _dst)
{
     if (_dst == NULL)
        return 0;
     return (_dst->type != EXCEPTION_OK) ? 1:0;
}

void
Exception_destroy(Exception* _dst)
{
     if (_dst == NULL)
        return;
     tstr_destroy(&_dst->msg);
     _dst->type = EXCEPTION_OK;
}

Environment*
Env_new(Environment* _env)
{
    *_env = (Environment){0};
    /*TODO: 30 is a arbitrary number, change when you know size of buildins*/
    sbAllocator_init(&_env->variable_allocator, sizeof(expr));
    Buildins_initn(&_env->buildins, 30);
    VariableScope_destroy(_env, &_env->globals);
    VariableScope_new(_env, &_env->globals, NULL);
    return _env;
}

void
Env_destroy(Environment* _env)
{
    int i;
    int n;
    if (_env == NULL)
        return;
    Buildins_destroy(&_env->buildins);
    VariableScope_destroy(_env, &_env->globals);
    n = VariableScopes_len(&_env->active_scopes);
    for (i = 0; i < n; i++)
        VariableScope_destroy(_env, VariableScopes_peek(&_env->active_scopes, i));

    sbAllocator_destroy(&_env->variable_allocator);
}

VariableScope*
VariableScope_new(Environment* _env, VariableScope* _this, VariableScope* _outer)
{
    VariableScope_destroy(_env, _this);
    Variables_initn(&_this->variables, 1);
    _this->outer = _outer;
    return _this;
}

void
VariableScope_destroy(Environment* _env, VariableScope* _scope)
{
    int i;
    Variable tmp;
    for (i = 0; i < Variables_len(&_scope->variables); i++) {
        tmp = Variables_get(&_scope->variables, i);
        variable_delete(_env, tmp.value);
    }
    Variables_destroy(&_scope->variables);
    _scope->outer = NULL;
}

/*the global nil object, so we dont have to allocate a bunch of them at runtime*/
expr _NIL_VALUE = (expr){.type = 0, .car = NULL, .cdr = NULL};

expr*
NIL(void)
{
    _NIL_VALUE.car = NULL;
    _NIL_VALUE.cdr = NULL;
    return &_NIL_VALUE;
}

char
is_nil(expr* _args)
{
    if (_args == NULL || _args == NIL())
        return 1;
    /*TODO: Error on empty cons cell being NIL*/
    //if (_args->type == TYPE_CONS && _args->car == NULL && _args->cdr == NULL)
    //return 1;
    //if (_args->type == TYPE_CONS && is_nil(car(_args)) && is_nil(cdr(_args)))
    //return 1;
    if (_args->type == TYPE_SYMBOL && tstr_equalc(&_args->symbol, "nil"))
        return 1;
    if (_args->type == TYPE_SYMBOL && tstr_equalc(&_args->symbol, "NIL"))
        return 1;

    return 0;
}

char
is_cons(expr* _args)
{
    if (!is_nil(_args) && _args->type == TYPE_CONS)
        return 1;
    return 0;
}

char
is_symbol(expr* _args)
{
    if (!is_nil(_args) && _args->type == TYPE_SYMBOL)
        return 1;
    return 0;
}

char
is_dotted(expr* _args)
{
    if (!is_nil(_args) && _args->type == TYPE_CONS &&
        !is_nil(_args->car) && !is_cons(_args->car) &&
        !is_nil(_args->cdr) && !is_cons(_args->cdr)
        )
        return 1;
    return 0;
}

char
is_val(expr* _args)
{
    if (is_nil(_args) || _args->type == TYPE_CONS)
        return 0;
    return 1;
}

char
is_string(expr* _args)
{
    if (!is_nil(_args) && _args->type == TYPE_STRING)
        return 1;
    return 0;
}

int
len(expr* _args)
{
    if (is_nil(_args)) return 0;
    if (is_val(_args)) return 1;
    int cnt = 0;
    expr* tmp = _args;
    while (!is_nil(cdr(tmp))) {
        tmp = cdr(tmp);
        cnt++;
    }
    return cnt;
}

expr*
car(expr* _args)
{
    if (is_nil(_args) || !is_cons(_args) || is_nil(_args->car))
        return NIL();
    return _args->car;
}

expr*
cdr(expr* _args)
{

    if (is_nil(_args) || !is_cons(_args) || is_nil(_args->cdr))
        return NIL();
    return _args->cdr;
}

expr* first(expr* _args)
{ return car(_args); }
expr* second(expr* _args)
{ return car(cdr(_args)); }
expr* third(expr* _args)
{ return car(cdr(cdr(_args))); }
expr* fourth(expr* _args)
{ return car(cdr(cdr(cdr(_args)))); }

expr*
nth(int _n, expr* _args)
{
    int i;
    expr* tmp = _args;
    if (_n < 0)
        return NIL();
    for (i = 0; i < _n; i++)
        tmp = cdr(tmp);
    return car(tmp);
}

expr*
_variable_new(Environment* _env)
{
    ASSERT_INV_ENV(_env);
    expr* e = (expr*)sbAllocator_get(&_env->variable_allocator);
    if (e == NULL)
        ASSERT_NOMOREMEMORY();
    return e;
}

expr*
variable_duplicate(Environment* _env, expr* _atom)
{
    ASSERT_INV_ENV(_env);
    if (is_nil(_atom))
        return NIL();
    switch (_atom->type) {
    case TYPE_SYMBOL:
        return symbol(_env, _atom->symbol.c_str);
    case TYPE_STRING:
        return string(_env, _atom->string.c_str);
    case TYPE_REAL:
        return real(_env, _atom->real);
    case TYPE_DECIMAL:
        return decimal(_env, _atom->decimal);
    case TYPE_CONS:
        return cons(_env,
                    variable_duplicate(_env, car(_atom)),
                    variable_duplicate(_env, cdr(_atom)));
    default:
        ASSERT_UNREACHABLE();
    };
}

void
variable_delete(Environment* _env, expr* _atom)
{
    ASSERT_INV_ENV(_env);
    if (is_nil(_atom))
        return;

    switch (_atom->type) {
    case TYPE_SYMBOL:
        tstr_destroy(&_atom->symbol);
        break;
    case TYPE_STRING:
        tstr_destroy(&_atom->string);
        break;
    default:
        break;
    };
    sbAllocator_return(&_env->variable_allocator, _atom);
}

const char*
type_to_str(int _e)
{
    switch (_e) {
    case TYPE_CONS:
        return "TYPE_CONS";
    case TYPE_REAL:
        return "TYPE_REAL";
    case TYPE_DECIMAL:
        return "TYPE_DECIMAL";
    case TYPE_SYMBOL:
        return "TYPE_SYMBOL";
    case TYPE_STRING:
        return "TYPE_STRING"; 
    case TYPE_LAMBDA:
        return "TYPE_LAMBDA";
    case TYPE_FUNCTION:
        return "TYPE_FUNCTION";
    case TYPE_MACRO:
        return "TYPE_MACRO";
    case TYPE_ERROR:
        return "TYPE_ERROR";
    case TYPE_VECTOR:
        return "TYPE_VECTOR";
    case TYPE_DICTIONARY:
        return "TYPE_DICTIONARY";
    default:
        ASSERT_UNREACHABLE();
    };
    return NULL;
}

expr*
cons(Environment* _env, expr* _car, expr* _cdr)
{
    ASSERT_INV_ENV(_env);
    expr* e = _variable_new(_env);
    e->type = TYPE_CONS;
    e->car = _car;
    e->cdr = _cdr;
    return e;
}

expr*
real(Environment* _env, int _v)
{
    ASSERT_INV_ENV(_env);
    expr* e = _variable_new(_env);
    e->type = TYPE_REAL;
    e->real = _v;
    return e;
}

expr*
decimal(Environment* _env, float _v)
{
    ASSERT_INV_SCOPE(_env);
    expr* e = _variable_new(_env);
    e->type = TYPE_DECIMAL;
    e->decimal = _v;
    return e;
}

expr*
symbol(Environment* _env, char* _v)
{
    ASSERT_INV_ENV(_env);
    expr* e = _variable_new(_env);
    e->type = TYPE_SYMBOL;
    e->symbol = tstr_(_v);
    return e;
}

expr*
string(Environment* _env, char* _v)
{
    ASSERT_INV_ENV(_env);
    expr* e = _variable_new(_env);
    e->type = TYPE_STRING;
    e->string = tstr_(_v);
    return e;
}

void
printexpr(expr* _arg)
{
    tstr str = stringify(_arg, "(", ")");
    printf("%s", str.c_str);
    tstr_destroy(&str);
}


tstr
_stringify_value(expr* _arg)
{
    tstr dst = {0};
    if (is_nil(_arg))
        return tstr_("NIL");

    switch (_arg->type) {
    case TYPE_CONS:
        dst = stringify(_arg, "(", ")");
        break;
    case TYPE_VECTOR:
        dst = stringify(_arg, "{", "}");
        break;
    case TYPE_DICTIONARY:
        dst = stringify(_arg, "#[", "]");
        break;
    case TYPE_LAMBDA:
        dst = tstr_("#<lambda>");
        break;
    case TYPE_MACRO:
        dst = tstr_("#<macro>");
        break;
    case TYPE_FUNCTION:
        dst = tstr_("#<function>");
        break;
    case TYPE_REAL:
        dst = tstr_from_int(_arg->real);
        break;
    case TYPE_DECIMAL:
        dst = tstr_from_float(_arg->decimal);
        break;
    case TYPE_SYMBOL:
        dst = tstr_(_arg->symbol.c_str);
        break;
    case TYPE_STRING:
        /*NOTE: The quotes are part of the string datatype now*/
        dst = tstr_(_arg->string.c_str);
        //dst = tstr_fmt("\"%s\"", _arg->string.c_str);
        break;
    default:
        ASSERT_UNREACHABLE();
    };
    return dst;
}

tstr
stringify(expr* _args, const char* _open, const char* _close)
{
    tstr dst = {0};
    expr* curr = _args;
    tstr currstr = {0};
    tstr open = tstr_view(_open);
    tstr close = tstr_view(_close);
    tstr space = tstr_view(" ");

    if (is_nil(_args))
        return tstr_("NIL");

    if (_args->type != TYPE_CONS)
        return _stringify_value(curr);

    if (is_dotted(_args)) {
        tstr carstr = _stringify_value(car(_args));
        tstr cdrstr = _stringify_value(cdr(_args));
        dst = tstr_fmt("(%s . %s)", carstr.c_str, cdrstr.c_str);
        tstr_destroy(&carstr);
        tstr_destroy(&cdrstr);
        return dst;
    }
    /*Manage head value*/
    tstr_add2end(&dst, &open);
    currstr = _stringify_value(car(curr));
    tstr_add2end(&dst, &currstr);
    curr = cdr(curr);

    /*Manage tail values*/
    while (!is_nil(curr)) {
        if (is_nil(car(curr)) && is_nil(cdr(curr)))
            break;
        tstr_add2end(&dst, &space);
        currstr = _stringify_value(car(curr));
        tstr_add2end(&dst, &currstr);
        curr = cdr(curr);
    }
    tstr_add2end(&dst, &close);
    return dst;
}

char
_token_value_type(tstr* _num)
{
    tstr decimal_indicator = tstr_view(".");
    if (tstr_find(_num, &decimal_indicator) == TSTR_INVALID)
        return TYPE_REAL;
    return TYPE_DECIMAL;
}

tstr
_try_get_special_token(char* _t)    
{
    const char* stokens[] = {
        "(", ")","'(",
        "{", "}","'{",
        "[", "]","'[",
        };
    const int stoken_len = sizeof(stokens) / sizeof(stokens[0]);
    tstr stok;
    int i;
    for (i = 0; i < stoken_len; i++) {
        stok = tstr_view(stokens[i]);
        if (_rawstr_is_equal(stok.c_str, _t, tstr_length(&stok)))
            return stok;
    }
    return (tstr) {0};
}

#define _IS_WHITESPACE(C) \
    (((C) == ' ' || (C) == '\t' || (C) == '\n') ? 1 : 0)


tstr
_get_next_token(char* _source, int* _cursor)
{
    tstr token = {0};
    tstr maybestoken = {0};
    int len = 0;

    maybestoken = _try_get_special_token(&_source[*_cursor]);

    if (tstr_ok(&maybestoken)) {
        len = tstr_length(&maybestoken);
    }
    else if (_source[*_cursor + len] == '\"') {
        len++;
        while (_source[*_cursor + len] != '\"' && _source[*_cursor + len] != '\0') {
            len++;
        }
        len++;
        token = tstr_n(&_source[*_cursor], len);
        *_cursor += len;
        return token;
    }
    else {
        while (!_IS_WHITESPACE(_source[*_cursor + len]) &&
               _source[*_cursor + len] != '\0') {
            len++;
            maybestoken = _try_get_special_token(&_source[*_cursor + len]);
            if (tstr_ok(&maybestoken))
                break;
        }
    }
    /*Create Token*/
    token = tstr_n(&_source[*_cursor], len);
    *_cursor += len;
    return token;
}

void
_trim_whitespace(char* _source, int* _cursor)
{
    while (_IS_WHITESPACE(_source[*_cursor]))
        (*_cursor)++;
}

#define t_type tstr
#define arr_t_name Tokens
#include "vendor/tsarray.h"

Tokens
_tokenize(char* _source, int* _cursor)
{
    assert(_source != NULL && "Invalid program given to tokenizer");
    assert(_cursor != NULL && "Invalid cursor given to tokenizer");
    Tokens tokens;
    Tokens_initn(&tokens, 1);
    tstr curr;

    *_cursor = 0;
    while (_source[*_cursor] != '\0') {
        _trim_whitespace(_source, _cursor);
        //printf("cursor(%d) -> ", *_cursor);
        curr = _get_next_token(_source, _cursor);
        //printf("token: %s\n", curr.c_str);
        Tokens_push(&tokens, curr);
    }
    /*TODO:  Do error checking like matching+even parens*/
    /*TODO:  Remove garbage tokens*/
    /*TODO:  if you want a tokenizer impl that is isolated, take this
             and add a tokenization recipe struct with:
             - garbate token list
             - literal token list
             - extra stuff like use newline tokens
             - etc..
    */
    return tokens;
}

void
tokens_print(Tokens* _tokens, const char* _start, const char* _end, const char* _sep)
{
    int i;
    int n = Tokens_len(_tokens);
    printf("TOKENS(%d): ", n);
    for (i = 0; i < n; i++) {
        printf("%s%s%s%s", _start, Tokens_peek(_tokens, i)->c_str, _end, _sep);
    }
    printf("\n");
}

void
_tokens_post_process(Tokens* _tokens)
{
  /*TODO: Here we remove the outer scope of the lexed function.
    if we lex more than one scope we cant do this..
    On top of this, we cant lex non-list inputs..
   */
    UNUSED(_tokens);
    //tokens_print(_tokens, "[", "]", " ");
    //Tokens_pop(_tokens);
    //Tokens_pop_front(_tokens);
}

char
_looks_like_number(tstr _token)
{
  /*TODO: We can do cool stuff like allowing _ in numbers and remove them before
          lexing so that 100_000_000 is a valid number.
  */
    if (tstr_length(&_token) > 1 && (_token.c_str[0] == '-' || _token.c_str[0] == '+'))
        return (_token.c_str[1] >= '0' && (_token.c_str[1]) <= '9') ? 1 : 0;
    return (_token.c_str[0] >= '0' && (_token.c_str[0]) <= '9') ? 1 : 0;
}

char
_looks_like_string(tstr _token)
{
    if (tstr_length(&_token) < 2)
        return 0;
    return (_token.c_str[0] == '\"');
}

char
tstr_cstartswith(tstr* _src, const char* _st)    
{
    tstr st = tstr_view(_st);
    if (tstr_length(_src) < tstr_length(&st))
        return 0;
    return _rawstr_is_equal(_src->c_str, st.c_str, tstr_length(&st));
}

expr*
_lex_value(Environment* _env, tstr* _token)
{
    if (_looks_like_string(*_token)) {
        return string(_env, _token->c_str);
    }
    else if (_looks_like_number(*_token)) {
        if (_token_value_type(_token) == TYPE_REAL)
            return real(_env, tstr_to_int(_token));
        else
            return decimal(_env, tstr_to_float(_token));
    }
    return symbol(_env, _token->c_str);
}

expr*
_lex(Environment* _env, Tokens* _tokens)
{
    /*TODO: Does not support special syntax for:
    dotted lists (a . 34)
    */
    ASSERT_INV_ENV(_env);
    assert(_tokens != NULL);
    tstr token;
    expr* program = cons(_env, NULL, NULL);
    expr* curr = program;

    while (Tokens_len(_tokens) > 0) {
        token = Tokens_pop_front(_tokens);
        if (tstr_equalc(&token, ")") || tstr_equalc(&token, "]")) {
            break;
        }
        else if (tstr_equalc(&token, "(")) {
            curr->car = _lex(_env, _tokens);
        }
        else if (tstr_equalc(&token, "[")) {
            curr->car = cons(_env, symbol(_env, "list"), _lex(_env, _tokens));
        }
        else if (tstr_equalc(&token, "{")) {
            ASSERT_UNREACHABLE();
            /*TODO: we dont support maps or vectors*/
            curr->car = cons(_env, symbol(_env, "map"), _lex(_env, _tokens));
        }
        else if (tstr_cstartswith(&token, "'")) {
            tstr resttok = tstr_(token.c_str+1);
            if (tstr_equalc(&resttok, "(") ||
                tstr_equalc(&resttok, "[") ||
                tstr_equalc(&resttok, "{")) {
                curr->car = cons(_env, symbol(_env, "quote"), cons(_env, _lex(_env, _tokens), NULL));
            } else {
                 curr->car = cons(_env, symbol(_env, "quote"), cons(_env, _lex_value(_env, &resttok), NULL));
            }
        }
        else {
            curr->car = _lex_value(_env, &token);
        }

        /*Insert extracted into lexed program*/
        tstr_destroy(&token);
        curr->cdr = cons(_env, NULL, NULL);
        curr = curr->cdr;
    }
    //DBPRINT("lexed program: ", program);
    return program;
}

expr*
read(Environment* _env, VariableScope* _scope, Exception* _throwdst, char* _program_str)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    assert(_program_str != NULL && "Given program str is NULL");
    /*TODO: Need error checking from _lex()*/
    UNUSED(_throwdst);
    int cursor = 0;
    Tokens tokens = _tokenize(_program_str, &cursor);
    _tokens_post_process(&tokens);
    expr* program = _lex(_env, &tokens);

    /*TODO: This mighr be wrong to do!*/
    //DBPRINT("program in 'read' ", program);
    //printf("Amount of programs in 'read' = %d\n", len(program));
    if (len(program) == 1)
        return car(program);
    return program;
}

Buildin*
_find_buildin(Environment* _env, expr* _sym)
// This function cannot assert or break, on error we return a NULL ptr.
// It is up to the user of this function to show an error if NULL is returned.
{
    Buildin* buildin = NULL;
    int i;
    if (is_nil(_sym) || !is_symbol(_sym))
        return NULL;
    //printf("looking for buildin '%s'\n", _sym->symbol.c_str);
    for (i = 0; i < Buildins_len(&_env->buildins); i++) {
        buildin = Buildins_peek(&_env->buildins, i);
        if (tstr_equal(&buildin->name, &_sym->symbol)) {
            //printf("found '%s'\n", buildin->name.c_str);
            return buildin;
        }
    }
    return NULL;
}

Variable*
_find_variable(VariableScope* _scope, expr* _sym)
{
    Variable* variable = NULL;
    int i;
    for (i = 0; i < Variables_len(&_scope->variables); i++) {
        variable = Variables_peek(&_scope->variables, i);
        if (tstr_equal(&variable->symbol, &_sym->symbol)) {
            return variable;
        }
    }
    if (_scope->outer != NULL)
        return _find_variable(_scope->outer, _sym);
    return NULL;
}

void
_bind(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _binds, expr* _values)
{
  /*TODO: We just blindly pair binds and values,
          In the future, add support for &rest and keyword binds aswell*/
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);

    if (len(_binds) != len(_values))
        THROW_INVALIDINPUT(_throwdst,
                           "_bind",
                           "Expected equal amount of binds and values",
                           _binds);

    expr* bind = _binds; 
    expr* val = _values; 
    //DBPRINT("binding ", car(bind));
    //DBPRINT("to ", car(val));
    while (!is_nil(cdr(bind)) && !is_nil(cdr(val))) {
        Env_add_variable(_env, _scope, car(bind)->symbol.c_str, car(val));
        bind = cdr(bind);
        val = cdr(val);
    }
}

expr*
eval_expr(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    /*TODO: Needs a cleaning*/
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    expr* fn = NULL;
    expr* args = NULL;
    Buildin* buildin = NULL;
    Variable* variable = NULL;
    VariableScope local = {0};
    expr* result = NULL;
    if (is_nil(_in))
        return _in;

    switch (_in->type) {
    case TYPE_CONS:
        fn = car(_in);
        if (fn->type == TYPE_CONS)
            fn = eval_expr(_env, _scope, _throwdst, fn);
        args = cdr(_in);

        if (fn->type == TYPE_LAMBDA) {
            //DBPRINT("lambda binds: ", fn->callable.binds);
            //DBPRINT("lambda body: ", fn->callable.body);
            args = list(_env, _scope, _throwdst, args);
            RETURN_ON_EXCEPTION(_throwdst, NIL());
            VariableScope_new(_env, &local, _scope);
            _bind(_env, &local, _throwdst, fn->callable.binds, args);
            result = buildin_do(_env, &local, _throwdst, fn->callable.body);
            RETURN_ON_EXCEPTION(_throwdst, NIL());
            VariableScope_destroy(_env, &local);
            return result;
        }
        else if (fn->type == TYPE_SYMBOL) {
            buildin = _find_buildin(_env, fn);
            if (buildin != NULL) {
                result = buildin->fn(_env, _scope, _throwdst, args);
                //printf("buildin [%s]\n", buildin->name.c_str);
                //DBPRINT("(eval) RESULT: ", result);
                return result;
            }
            //DBPRINT("trying to locate function variable: ", fn);
            variable = _find_variable(&_env->globals, fn);
            if (variable != NULL) {
                if (variable->value->type == TYPE_FUNCTION) {
                    //printf("found fn: %s\n", variable->symbol.c_str);
                    //DBPRINT("fn binds: ", variable->value->callable.binds);
                    //DBPRINT("fn body: ", variable->value->callable.body);
                    args = list(_env, _scope, _throwdst, args);
                    RETURN_ON_EXCEPTION(_throwdst, NIL());
                    VariableScope_new(_env, &local, _scope);
                    _bind(_env, &local, _throwdst, variable->value->callable.binds, args);
                    result = buildin_do(_env, &local, _throwdst, variable->value->callable.body);
                    RETURN_ON_EXCEPTION(_throwdst, NIL());
                    VariableScope_destroy(_env, &local);
                    return result;
                }
                else if (variable->value->type == TYPE_MACRO) {
                    /*TODO: verify this is correct, only diff is i dont eval args!*/
                    VariableScope_new(_env, &local, _scope);
                    _bind(_env, &local, _throwdst, variable->value->callable.binds, args);
                    result = buildin_do(_env, &local, _throwdst, variable->value->callable.body);
                    RETURN_ON_EXCEPTION(_throwdst, NIL());
                    VariableScope_destroy(_env, &local);
                    return result;
                }
            }
            THROW_INVALIDINPUT(_throwdst, "eval", "expected callable buildin function", _in);
            return NIL();
        }
        THROW_INVALIDFUNCTION(_throwdst, "eval", _in);
        return NIL();
    case TYPE_SYMBOL:
        variable = _find_variable(&_env->constants, _in);
        if (variable != NULL) {
            return variable->value;
        }
        variable = _find_variable(_scope, _in);
        if (variable != NULL) {
            return variable->value;
        }
        variable = _find_variable(&_env->globals, _in);
        if (variable != NULL) {
            return variable->value;
        }
        THROW_INVALIDSYMBOL(_throwdst, "eval", _in);
        return NIL();

    case TYPE_REAL:
    case TYPE_DECIMAL:
    case TYPE_STRING:
        return _in;
    default:
        ASSERT_UNREACHABLE();
    };
    ASSERT_UNREACHABLE();
    return NIL();
}

expr* buildin_list(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr*
list(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    return buildin_list(_env, _scope, _throwdst, _in);
}

/*********************************************************************/
/* Buildin Functions */
/*********************************************************************/



expr*
buildin_read(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    if (!is_string(_in)) {
        THROW_INVALIDINPUT(_throwdst, "read", "expected string input", _in);
        return NIL();
    }
    return read(_env, _scope, _throwdst, _in->string.c_str);
}

expr*
buildin_list(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    /*list() is a list evaller that expects a list of eval-able lists */
    expr* root = cons(_env, NULL, NULL);
    expr* curr = root;
    ASSERT_INV_SCOPE(_scope);
    //DBPRINT("'list' input: ", _in);
    if (!is_cons(_in)) {
        THROW_INVALIDINPUT(_throwdst, "list", "expected a list", _in);
        return NIL();
    }
    while (!is_nil(cdr(_in))) {
        curr->car = eval_expr(_env, _scope, _throwdst, car(_in));
        RETURN_ON_EXCEPTION(_throwdst, NIL());
        //DBPRINT("list evalled: ", car(_in));
        //DBPRINT("to ", car(curr));
        curr->cdr = cons(_env, NULL, NULL);
        curr = cdr(curr);
        _in = cdr(_in);
    }
    //DBPRINT("'list' args: ", root);
    return root;
}

expr*
buildin_len(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* args;
    ASSERT_INV_SCOPE(_scope);
    args = list(_env, _scope, _throwdst, _in);
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    if (len(args) != 1) {
        THROW_INVALIDINPUT(_throwdst, "len", "Expected only 1 input", _in);
        return NIL();
    }
    args = first(args);
    return real(_env, len(args));
}

expr*
buildin_quote(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_SCOPE(_scope);
    ASSERT_INV_SCOPE(_env);
    UNUSED(_throwdst);
    return car(_in);
}

expr*
buildin_car(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* args;
    ASSERT_INV_SCOPE(_scope);
    args = list(_env, _scope, _throwdst, _in);
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    if (len(args) != 1) {
        THROW_INVALIDINPUT(_throwdst, "car", "Expected only 1 input", _in);
        return NIL();
    }
    args = first(args);
    return car(args);
}

expr*
buildin_cdr(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* args;
    ASSERT_INV_SCOPE(_scope);
    args = list(_env, _scope, _throwdst, _in);
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    if (len(args) != 1) {
        THROW_INVALIDINPUT(_throwdst, "cdr", "Expected only 1 input", _in);
        return NIL();
    }
    args = first(args);
    return cdr(args);
}

expr*
buildin_cons(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* args;
    ASSERT_INV_SCOPE(_scope);
    //DBPRINT("cons input: ", _in);
    args = list(_env, _scope, _throwdst, _in);
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    if (len(args) != 2) {
        THROW_INVALIDINPUT(_throwdst, "cdr", "Expected only 2 inputs", _in);
        return NIL();
    }
    //DBPRINT("cons a: ", first(args));
    //DBPRINT("cons b: ", second(args));
    return cons(_env, first(args), second(args));
}

expr*
buildin_first(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* args;
    ASSERT_INV_SCOPE(_scope);
    args = list(_env, _scope, _throwdst, _in);
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    if (len(args) != 2) {
        THROW_INVALIDINPUT(_throwdst, "first", "Expected only 1 input", _in);
        return NIL();
    }
    args = first(args);
    return first(args);
}

expr*
buildin_second(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* args;
    ASSERT_INV_SCOPE(_scope);
    args = list(_env, _scope, _throwdst, _in);
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    if (len(args) != 1) {
        THROW_INVALIDINPUT(_throwdst, "second", "Expected only 1 input", _in);
        return NIL();
    }
    args = first(args);
    return second(args);
}

expr*
buildin_third(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* args;
    ASSERT_INV_SCOPE(_scope);
    args = list(_env, _scope, _throwdst, _in);
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    if (len(args) != 1) {
        THROW_INVALIDINPUT(_throwdst, "third", "Expected only 1 input", _in);
        return NIL();
    }
    args = first(args);
    return third(args);
}

expr*
buildin_fourth(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* args;
    ASSERT_INV_SCOPE(_scope);
    args = list(_env, _scope, _throwdst, _in);
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    if (len(args) != 1) {
        THROW_INVALIDINPUT(_throwdst, "fourth", "Expected only 1 input", _in);
        return NIL();
    }
    args = first(args);
    return fourth(args);
}

expr*
buildin_nth(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* args;
    expr* n;
    ASSERT_INV_SCOPE(_scope);
    args = list(_env, _scope, _throwdst, _in);
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    if (len(args) != 2) {
        THROW_INVALIDINPUT(_throwdst, "nth", "Expected only 2 inputs", _in);
        return NIL();
    }
    n = first(args);
    args = second(args);
    if (n->type != TYPE_REAL) {
        THROW_INVALIDTYPE(_throwdst, "nth", TYPE_REAL, n);
        return NIL();
    }
    return nth(n->real, args);
}

expr*
buildin_range(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* args;
    expr* start;
    expr* end;
    expr* range;
    expr* curr;
    int i;

    ASSERT_INV_SCOPE(_scope);

    args = list(_env, _scope, _throwdst, _in);
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    if (len(args) != 2) {
        THROW_INVALIDINPUT(_throwdst, "range", "Expected only 2 inputs", _in);
        return NIL();
    }
    start = first(args);
    end = second(args);
    if (start->type != TYPE_REAL || end->type != TYPE_REAL) {
        THROW_INVALIDTYPE(_throwdst, "range", TYPE_REAL, args);
        return NIL();
    }
    if (start->real > end->real) {
        THROW_INVALIDINPUT(_throwdst, "range", "expected first input to be smaller than second input", _in);
        return NIL();
    }
    range = cons(_env, NULL, NULL);
    curr = range;
    for (i = start->real; i < end->real; i++) {
        curr->car = real(_env, i);
        curr->cdr = cons(_env, NULL, NULL);
        curr = cdr(curr);
    }
    return range;
}

expr*
_PLUS2(Environment* _env, Exception* _throwdst, expr* _a, expr* _b)
{
    if (!is_val(_a) || !is_val(_b)) {
        THROW_INVALIDINPUT(_throwdst, "_PLUS2", "expected inputs to be values", cons(_env, _a, _b));
        return real(_env, 0);
    }
    if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
        return decimal(_env, _a->decimal + _b->decimal);
    else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
        return decimal(_env, _a->real + _b->decimal);
    else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
        return  decimal(_env, _a->decimal + _b->real);
    return real(_env, _a->real + _b->real);
}

expr*
buildin_plus(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* args;
    expr* tmp;
    expr* result;
    ASSERT_INV_SCOPE(_scope);
    args = list(_env, _scope, _throwdst, _in);
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    if (len(args) == 0) {
        return real(_env, 0);
    }
    /*Check for non value inputs*/
    tmp = args;
    while (!is_nil(cdr(tmp))) {
        if (!is_val(car(tmp))) {
            THROW_NONVALUEARG(_throwdst, "+", args);
            return NIL();
        }
        tmp = cdr(tmp);
    }
    /*Do the math*/
    tmp = args;
    result = real(_env, 0);
    while (!is_nil(cdr(tmp))) {
        result = _PLUS2(_env, _throwdst, result, car(tmp));
        tmp = cdr(tmp);
        if (Exception_is_error(_throwdst))
            return NIL();
    }
    return result;
}

expr*
_MINUS2(Environment* _env, Exception* _throwdst, expr* _a, expr* _b)
{
    if (!is_val(_a) || !is_val(_b)) {
        THROW_INVALIDINPUT(_throwdst, "_MINUS2", "expected inputs to be values", cons(_env, _a, _b));
        return real(_env, 0);
    }
    if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
        return decimal(_env, _a->decimal - _b->decimal);
    else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
        return decimal(_env, _a->real - _b->decimal);
    else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
        return  decimal(_env, _a->decimal - _b->real);
    return real(_env, _a->real - _b->real);
}

expr*
buildin_minus(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* args;
    expr* tmp;
    expr* result;
    ASSERT_INV_SCOPE(_scope);
    args = list(_env, _scope, _throwdst, _in);
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    /*Check for non value inputs*/
    if (len(args) == 0) {
        return real(_env, 0);
    }
    tmp = args;
    while (!is_nil(cdr(tmp))) {
        if (!is_val(car(tmp))) {
            THROW_NONVALUEARG(_throwdst, "-", args);
            return NIL();
        }
        tmp = cdr(tmp);
    }
    /*Do the math*/
    tmp = args;
    result = variable_duplicate(_env, first(args));
    tmp = cdr(tmp);
    while (!is_nil(cdr(tmp))) {
        result = _MINUS2(_env, _throwdst, result, car(tmp));
        tmp = cdr(tmp);
        if (Exception_is_error(_throwdst))
            return NIL();
    }
    return result;
}

expr*
_MULTIPLY2(Environment* _env, Exception* _throwdst, expr* _a, expr* _b)
{
    ASSERT_INV_ENV(_env);
    if (!is_val(_a) || !is_val(_b)) {
        THROW_INVALIDINPUT(_throwdst, "_MULTIPLY2", "expected inputs to be values", cons(_env, _a, _b));
        return real(_env, 0);
    }
    if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
        return decimal(_env, _a->decimal * _b->decimal);
    else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
        return decimal(_env, _a->real * _b->decimal);
    else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
        return  decimal(_env, _a->decimal * _b->real);
    return real(_env, _a->real * _b->real);
}

expr*
buildin_multiply(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* args;
    expr* tmp;
    expr* result;
    ASSERT_INV_SCOPE(_scope);
    args = list(_env, _scope, _throwdst, _in);
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    /*Check for non value inputs*/
    if (len(args) == 0) {
        return real(_env, 1);
    }
    tmp = args;
    while (!is_nil(cdr(tmp))) {
        if (!is_val(car(tmp))) {
            THROW_NONVALUEARG(_throwdst, "*", args);
            return NIL();
        }
        tmp = cdr(tmp);
    }
    /*Do the math*/
    tmp = args;
    result = real(_env, 1);
    while (!is_nil(cdr(tmp))) {
        result = _MULTIPLY2(_env, _throwdst, result, car(tmp));
        tmp = cdr(tmp);
        if (Exception_is_error(_throwdst))
            return NIL();
    }
    return result;
}

expr*
_DIVIDE2(Environment* _env, Exception* _throwdst, expr* _a, expr* _b)
{
    ASSERT_INV_ENV(_env);
    if (!is_val(_a) || !is_val(_b)) {
        THROW_INVALIDINPUT(_throwdst, "_DIVIDE2", "expected inputs to be values", cons(_env, _a, _b));
        return real(_env, 0);
    }
    if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
        return decimal(_env, _a->decimal / _b->decimal);
    else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
        return decimal(_env, _a->real / _b->decimal);
    else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
        return  decimal(_env, _a->decimal / _b->real);
    return real(_env, _a->real / _b->real);
}

expr*
buildin_divide(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* args;
    expr* tmp;
    expr* result;
    ASSERT_INV_SCOPE(_scope);
    args = list(_env, _scope, _throwdst, _in);
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    if (len(args) == 0) {
        return real(_env, 1);
    }
    /*Check for non value inputs*/
    tmp = args;
    while (!is_nil(cdr(tmp))) {
        if (!is_val(car(tmp))) {
            THROW_NONVALUEARG(_throwdst, "/", args);
            return NIL();
        }
        if ((tmp->type == TYPE_REAL && tmp->real == 0) ||
            (tmp->type == TYPE_DECIMAL && tmp->decimal == 0.f)) {
            THROW_INVALIDINPUT(_throwdst, "divide", "division by zero!", args);
            return real(_env, 0);
        }
        tmp = cdr(tmp);
    }
    /*Do the math*/
    tmp = args;
    result = variable_duplicate(_env, first(args));
    tmp = cdr(tmp);
    while (!is_nil(cdr(tmp))) {
        result = _DIVIDE2(_env, _throwdst, result, car(tmp));
        tmp = cdr(tmp);
        if (Exception_is_error(_throwdst))
            return NIL();
    }
    return result;
}

char
_MATHEQUAL2(Environment* _env, Exception* _throwdst, expr* _a, expr* _b)
{
    ASSERT_INV_ENV(_env);
    if (is_cons(_a) || is_cons(_b)) {
        THROW_INVALIDTYPE(_throwdst, "_MATHEQUAL2", "expected inputs to be values, symbols or strings", cons(_env, _a, _b));
        return 0;
    }
    if (_a == _b)
        return 1;

    if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
        return _a->decimal == _b->decimal;
    else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
        return _a->real == _b->decimal;
    else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
        return  _a->decimal == _b->real;
    else if (_a->type == TYPE_REAL && _b->type == TYPE_REAL)
        return _a->real == _b->real;

    else if (_a->type == TYPE_SYMBOL && _b->type == TYPE_SYMBOL)
        return tstr_equal(&_a->symbol, &_b->symbol);
    else if (_a->type == TYPE_STRING && _b->type == TYPE_STRING)
        return tstr_equal(&_a->string, &_b->string);

    return 0;
}

expr*
buildin_mathequal(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* args;
    expr* oldtmp;
    expr* tmp;
    ASSERT_INV_SCOPE(_scope);
    args = list(_env, _scope, _throwdst, _in);
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    if (len(args) < 2) {
        return symbol(_env, "t");
    }
    /*Check for non value inputs*/
    tmp = args;
    while (!is_nil(cdr(tmp))) {
        if (!is_val(car(tmp))) {
            THROW_NONVALUEARG(_throwdst, "=", args);
            return symbol(_env, "NIL");
        }
        tmp = cdr(tmp);
    }
    /*check args for equality*/
    oldtmp = first(args);
    tmp = second(args);
    while (!is_nil(tmp)) {
        if(!_MATHEQUAL2(_env, _throwdst, oldtmp, tmp))
            return symbol(_env, "NIL");
        if (Exception_is_error(_throwdst))
            return symbol(_env, "NIL");
        tmp = cdr(tmp);
        oldtmp = cdr(oldtmp);
    }
    return symbol(_env, "t");
}

expr*
buildin_simequal(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    tstr gt;
    tstr curr;
    expr* tmp;
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    UNUSED(_throwdst);

    if (len(_in) < 2) {
        return symbol(_env, "t");
    }
    tmp = _in;
    gt = _stringify_value(first(tmp));
    tmp = cdr(tmp);

    /*check for equality*/
    while (!is_nil(cdr(tmp))) {
        curr = _stringify_value(car(tmp));
        if (!tstr_equal(&gt, &curr)) {
            tstr_destroy(&gt);
            tstr_destroy(&curr);
            return NIL();
        }
        tstr_destroy(&curr);
        tmp = cdr(tmp);
    }
    tstr_destroy(&gt);
    return symbol(_env, "t");
}

expr*
buildin_eq(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    tstr gt;
    tstr curr;
    expr* tmp;
    expr* args;
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    args = list(_env, _scope, _throwdst, _in);
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    if (len(args) < 2) {
        return symbol(_env, "t");
    }
    tmp = args;
    gt = _stringify_value(first(tmp));
    tmp = cdr(tmp);

    /*check for equality*/
    while (!is_nil(cdr(tmp))) {
        curr = _stringify_value(car(tmp));
        if (!tstr_equal(&gt, &curr)) {
            tstr_destroy(&gt);
            tstr_destroy(&curr);
            return NIL();
        }
        tstr_destroy(&curr);
        tmp = cdr(tmp);
    }
    tstr_destroy(&gt);
    return symbol(_env, "t");
}

expr*
buildin_do(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    expr* tmp = _in;
    expr* last = NIL();

    if (!is_cons(tmp)) {
        last = eval_expr(_env, _scope, _throwdst, tmp);
        RETURN_ON_EXCEPTION(_throwdst, NIL());
        return last;
    }
    while (!is_nil(cdr(tmp))) {
        if (!is_cons(tmp)) {
            THROW_INVALIDINPUT(_throwdst, "do", "expected callable functions", _in);
            return NIL();
        }
        last = eval_expr(_env, _scope, _throwdst, car(tmp));
        RETURN_ON_EXCEPTION(_throwdst, NIL());
        tmp = cdr(tmp);
    }
    return last;
}

void
_add_to_variablescope(Variables* _dst, expr* _n, expr* _v)
{
    Variable v = {0};
    tstr_copy(&_n->symbol, &v.symbol);
    v.value = _v;
    Variables_push(_dst, v);
}

expr*
buildin_defconst(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    expr* val;
    val = eval_expr(_env, _scope, _throwdst, second(_in));
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    if (len(_in) < 1 || len(_in) > 2 || !is_symbol(first(_in))) {
        THROW_INVALIDINPUT(_throwdst, "const", "expected 1-2 args, and first arg to be symbol", _in);
        return NIL();
    }
    _add_to_variablescope(&_env->constants.variables, first(_in), val);
    DBPRINT("created constant: ", first(_in));
    DBPRINT("with value: ", val);
    return first(_in);
}

expr*
buildin_defglobal(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    expr* val;

    DBPRINT("global input ", _in);
    DBPRINT("global symbol name ", first(_in));
    DBPRINT("global symbol value ", second(_in));
    val = eval_expr(_env, _scope, _throwdst, second(_in));
    DBPRINT("global evaled symbol value to ", val);
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    if (len(_in) < 1 || len(_in) > 2 || !is_symbol(first(_in))) {
        THROW_INVALIDINPUT(_throwdst, "global", "expected 1-2 args, and first arg to be symbol", _in);
        return NIL();
    }
    _add_to_variablescope(&_env->globals.variables, first(_in), val);
    DBPRINT("created global: ", first(_in));
    DBPRINT("with value: ", val);
    return first(_in);
}

expr*
buildin_defvar(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    expr* val;
    val = eval_expr(_env, _scope, _throwdst, second(_in));
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    if (len(_in) < 1 || len(_in) > 2 || !is_symbol(first(_in))) {
        THROW_INVALIDINPUT(_throwdst, "var", "expected 1-2 args, and first arg to be symbol", _in);
        return NIL();
    }
    _add_to_variablescope(&_scope->variables, first(_in), val);
    DBPRINT("created variable: ", first(_in));
    DBPRINT("with value: ", val);
    return first(_in);
}

expr*
buildin_set(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    THROW_NOTIMPLEMENTED(_throwdst, "set!");
    return _in;
}

expr*
buildin_put(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    THROW_NOTIMPLEMENTED(_throwdst, "put!");
    return _in;
}

expr*
buildin_pop(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    THROW_NOTIMPLEMENTED(_throwdst, "pop!");
    return _in;
}

expr*
buildin_reverse(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    THROW_NOTIMPLEMENTED(_throwdst, "reverse");
    return _in;
}

expr*
buildin_apply(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    THROW_NOTIMPLEMENTED(_throwdst, "apply");
    return _in;
}

expr*
buildin_map(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    THROW_NOTIMPLEMENTED(_throwdst, "map");
    return _in;
}

expr*
buildin_defmacro(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    /*TODO: add safety*/
    UNUSED(_throwdst);

    Variable var = {0};
    tstr_copy(&first(_in)->symbol, &var.symbol);
    var.value = _variable_new(_env);
    var.value->type = TYPE_MACRO;
    var.value->callable.binds = second(_in);
    var.value->callable.body = cdr(cdr(_in));
    Variables_push(&_env->globals.variables, var);

    return _in;
}

expr*
buildin_deffn(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    /*TODO: add safety*/
    UNUSED(_throwdst);

    Variable var = {0};
    tstr_copy(&first(_in)->symbol, &var.symbol);
    var.value = _variable_new(_env);
    var.value->type = TYPE_FUNCTION;
    var.value->callable.binds = second(_in);
    var.value->callable.body = cdr(cdr(_in));
    Variables_push(&_env->globals.variables, var);
    return first(_in);
}

expr*
buildin_deflambda(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    /*TODO: add safety*/
    UNUSED(_throwdst);

    expr* lambda = _variable_new(_env);
    lambda->type = TYPE_LAMBDA;
    lambda->callable.binds = car(_in);
    lambda->callable.body = cdr(_in);
    return lambda;
}

expr*
buildin_if(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    expr* res;
    expr* cond;
    if (len(_in) != 2 && len(_in) != 3) {
        THROW_INVALIDINPUT(_throwdst, "if", "expected 2-3 inputs", _in);
        return NIL();
    }
    if (is_nil(_in))
        return NIL();
    cond = eval_expr(_env, _scope, _throwdst, first(_in));
    RETURN_ON_EXCEPTION(_throwdst, NIL());
    if (!is_nil(cond)) {
        res = eval_expr(_env, _scope, _throwdst, second(_in));
        RETURN_ON_EXCEPTION(_throwdst, NIL());
        return res;
    }
    return eval_expr(_env, _scope, _throwdst, third(_in));
}

expr*
buildin_cond(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    THROW_NOTIMPLEMENTED(_throwdst, "cond");
    return _in;
}

expr*
buildin_not(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    THROW_NOTIMPLEMENTED(_throwdst, "not");
    return _in;
}

expr*
buildin_gc_run(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    THROW_NOTIMPLEMENTED(_throwdst, "gc-run");
    return _in;
}

expr*
buildin_gc_info(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    THROW_NOTIMPLEMENTED(_throwdst, "gc-info");
    return _in;
}

expr*
buildin_gc_memused(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    THROW_NOTIMPLEMENTED(_throwdst, "gc-mem-used");
    return _in;
}

expr*
buildin_try(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    THROW_NOTIMPLEMENTED(_throwdst, "try");
    return _in;
}

expr*
buildin_catch(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    THROW_NOTIMPLEMENTED(_throwdst, "catch");
    return _in;
}

expr*
buildin_throw(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    THROW_NOTIMPLEMENTED(_throwdst, "throw");
    return _in;
}

expr*
buildin_print_env(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    THROW_NOTIMPLEMENTED(_throwdst, "print-env");
    return _in;
}

void
Env_add_buildin(Environment* _env, const char* _name, buildin_fn _fn)
{
    Buildins_push(&_env->buildins, (Buildin){tstr_((char*)_name), _fn});
}

void
Env_add_constant(Environment* _env, const char* _name, expr* value)
{
    Variable v = {0};
    v.symbol = tstr_((char*)_name);
    v.value = variable_duplicate(_env, value);
    Variables_push(&_env->constants.variables, v);
}

void
Env_add_variable(Environment* _env, VariableScope* _scope, const char* _name, expr* value)
{
    Variable v = {0};
    v.symbol = tstr_((char*)_name);
    v.value = variable_duplicate(_env, value);
    Variables_push(&_scope->variables, v);
}

void
Env_add_global(Environment* _env, const char* _name, expr* value)
{
    Env_add_variable(_env, &_env->globals, _name, value);
    //Variable v = {0};
    //v.symbol = tstr_((char*)_name);
    //v.value = variable_duplicate(_env, value);
    //Variables_push(&_env->globals.variables, v);
}

void
Env_add_core(Environment* _env)
{
    ASSERT_INV_ENV(_env);

    /*List management*/
    Env_add_buildin(_env, "range", buildin_range);
    Env_add_buildin(_env, "quote", buildin_quote);
    Env_add_buildin(_env, "list", buildin_list);
    Env_add_buildin(_env, "cons", buildin_cons);
    Env_add_buildin(_env, "len", buildin_len);
    Env_add_buildin(_env, "put!", buildin_put);
    Env_add_buildin(_env, "pop!", buildin_pop);
    Env_add_buildin(_env, "reverse", buildin_reverse);

    /*Evaluation*/
    Env_add_buildin(_env, "apply", buildin_apply);
    Env_add_buildin(_env, "do", buildin_do);
    Env_add_buildin(_env, "map", buildin_map);

    /*Garbage Collection*/
    Env_add_buildin(_env, "gc-run", buildin_gc_run);
    Env_add_buildin(_env, "gc-info", buildin_gc_info);
    Env_add_buildin(_env, "gc-mem-used", buildin_gc_memused);

    /*Error Management*/
    Env_add_buildin(_env, "try", buildin_try);
    Env_add_buildin(_env, "catch", buildin_catch);
    Env_add_buildin(_env, "throw", buildin_throw);

    /*Accessors*/
    Env_add_buildin(_env, "car", buildin_car);
    Env_add_buildin(_env, "cdr", buildin_cdr);
    Env_add_buildin(_env, "head", buildin_car);
    Env_add_buildin(_env, "tail", buildin_cdr);
    Env_add_buildin(_env, "first", buildin_car);
    Env_add_buildin(_env, "rest", buildin_cdr);
    Env_add_buildin(_env, "second", buildin_second);
    Env_add_buildin(_env, "third", buildin_third);
    Env_add_buildin(_env, "fourth", buildin_fourth);
    Env_add_buildin(_env, "nth", buildin_nth);

    /*String management*/

    /*Math operators*/
    Env_add_buildin(_env, "+", buildin_plus);
    Env_add_buildin(_env, "-", buildin_minus);
    Env_add_buildin(_env, "*", buildin_multiply);
    Env_add_buildin(_env, "/", buildin_divide);
    Env_add_buildin(_env, "=", buildin_mathequal);
    Env_add_buildin(_env, "eq", buildin_eq);
    Env_add_buildin(_env, "equal", buildin_simequal);

    /*Variable management*/
    Env_add_buildin(_env, "const", buildin_defconst);
    Env_add_buildin(_env, "global", buildin_defglobal);
    Env_add_buildin(_env, "var", buildin_defvar);
    //Env_add_buildin(_env, "let*", buildin_letstar);
    Env_add_buildin(_env, "set", buildin_set);
    Env_add_buildin(_env, "print-env", buildin_print_env);

    /*Condition management*/
    Env_add_buildin(_env, "cond", buildin_cond);
    Env_add_buildin(_env, "if", buildin_if);
    Env_add_buildin(_env, "?", buildin_if);
    Env_add_buildin(_env, "not", buildin_not);
    Env_add_buildin(_env, "!", buildin_not);

    /*Function management*/
    Env_add_buildin(_env, "fn", buildin_deffn);
    Env_add_buildin(_env, "macro", buildin_defmacro);
    Env_add_buildin(_env, "lambda", buildin_deflambda);
    Env_add_buildin(_env, "/.", buildin_deflambda);

    /*Useful Constants*/
    Env_add_constant(_env, "NIL", symbol(_env, "NIL"));
    Env_add_constant(_env, "nil", symbol(_env, "nil"));
    Env_add_constant(_env, "t", symbol(_env, "t"));
    Env_add_constant(_env, "T", symbol(_env, "T"));
    Env_add_constant(_env, "PI", decimal(_env, 3.14));
    Env_add_constant(_env, "PI/2", decimal(_env, 0.5*3.14));
    Env_add_constant(_env, "2PI", decimal(_env, 2*3.14));
    Env_add_constant(_env, "E", decimal(_env, 2.71828));
    Env_add_constant(_env, "E-constant", decimal(_env, 0.57721));
}

#endif /*YAL_IMPLEMENTATION*/
#endif /*YAL_H*/
