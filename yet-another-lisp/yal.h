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
- [0.1] Created AST structure and imported base lib functionality.
- [0.0] Initialized library.
*/

/*
https://buildyourownlisp.com/chapter9_s_expressions#lists_and_lisps
http://www.ulisp.com/show?1BLW
*/


/*
  TODO: We need to integrate a list cleaning function to make (1 2 3) into 1 2 3
  TODO: We need to propegate eval call across all arguments in eval call,
        Rules:
        - number evals to number
        - symbol evals to symbol's value
        - list evals called function
*/



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

enum TYPE {
    TYPE_CONS = 0,
    TYPE_REAL,
    TYPE_DECIMAL,
    TYPE_SYMBOL,
    TYPE_STRING,

    TYPE_COUNT
};

const char* TYPE_TO_STR[TYPE_COUNT] = {
    "TYPE_CONS",
    "TYPE_REAL",
    "TYPE_DECIMAL",
    "TYPE_SYMBOL",
    "TYPE_STRING"
};

/*Forward declarations*/
typedef struct expr expr;
typedef struct Environment Environment;

typedef expr*(*buildin_fn)(Environment*, expr*);

typedef struct {
    tstr name;
    buildin_fn fn;
}Buildin;

#define t_type Buildin
#define arr_t_name Buildins
#include "vendor/tsarray.h"

struct Environment {
    Buildins buildins;
    sbAllocator expr_allocator;
    ErrorManager errormanager;
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
        };
        /*Cons atom*/
        struct {
            struct expr* car;
            struct expr* cdr;
        };
    };
};

/* =========================================================
 * Safety and Assertion on error
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

/*Environment management*/
Environment* Env_new(Environment* _env);
void Env_destroy(Environment* _env);

/*Checks*/
char is_nil(expr* _args);
char is_val(expr* _args);
int len(expr* _args);

/*List management*/
expr* car(expr* _args);
expr* cdr(expr* _args);
expr* first(expr* _args); 
expr* second(expr* _args);
expr* third(expr* _args);
expr* fourth(expr* _args);
expr* nth(int _n, expr* _args);

/*Value creation*/
expr* cons(Environment *_env, expr* _car, expr* _cdr);
expr* real(Environment *_env, int _v);
expr* decimal(Environment *_env, float _v);
expr* symbol(Environment *_env, tstr _v);
expr* string(Environment *_env, tstr _v);

/*Printing*/
expr* print(expr* _args);

/*Core*/
expr* read(Environment* _env, char* _program_str);
expr* eval(Environment* _env, expr* _e);


/******************************************************************************/
#define YAL_IMPLEMENTATION
#ifdef YAL_IMPLEMENTATION

Environment*
Env_new(Environment* _env)
{
    *_env = (Environment){0};
    sbAllocator_init(&_env->expr_allocator, sizeof(expr));
    /*TODO: 30 is a arbitrary number, change when you know size of buildins*/
    Buildins_initn(&_env->buildins, 30);
    _env->errormanager = (ErrorManager) {0};
    return _env;
}

void
Env_destroy(Environment* _env)
{
    if (_env == NULL)
        return;
    sbAllocator_destroy(&_env->expr_allocator);
    Errors_clear(&_env->errormanager);
}

/*the global nil object, so we dont have to allocate a bunch of them at runtime*/
expr _NIL_VALUE = (expr){.type = 0, .car = NULL, .cdr = NULL};

expr*
NIL(void)
{
    return &_NIL_VALUE;
}

char
is_nil(expr* _args)
{
    if (_args == NULL || _args == NIL())
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
is_val(expr* _args)
{
    if (is_nil(_args) || _args->type == TYPE_CONS)
        return 0;
    return 1;
}

int
len(expr* _args)
{
    /*http://clhs.lisp.se/Body/f_list_l.htm*/
    if (is_nil(_args)) return 0;
    if (is_val(_args)) return 1;
    int cnt = 1;
    expr* tmp = _args;
    while (!is_nil(tmp)) {
        tmp = cdr(tmp);
        cnt++;
    }
    return cnt;
}

expr*
car(expr* _args)
{
    if (is_nil(_args) || is_val(_args) || is_nil(_args->car))
        return NIL();
    return _args->car;
}

expr*
cdr(expr* _args)
{
    if (is_nil(_args) || is_val(_args) || is_nil(_args->cdr))
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
_atom_new(Environment *_env)
{
    ASSERT_INV_ENV(_env);
    expr* e = (expr*)sbAllocator_get(&_env->expr_allocator);
    ERRCHECK_NILALLOC(_env, e);
    return e;
}

void
_atom_delete(Environment *_env, expr* _atom)
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
    sbAllocator_return(&_env->expr_allocator, _atom);
}

expr*
cons(Environment *_env, expr* _car, expr* _cdr)
{
    ASSERT_INV_ENV(_env);
    expr* e = _atom_new(_env);
    e->type = TYPE_CONS;
    e->car = _car;
    e->cdr = _cdr;
    return e;
}

expr*
real(Environment *_env, int _v)
{
    ASSERT_INV_ENV(_env);
    expr* e = _atom_new(_env);
    e->type = TYPE_REAL;
    e->real = _v;
    return e;
}

expr*
decimal(Environment *_env, float _v)
{
    ASSERT_INV_ENV(_env);
    expr* e = _atom_new(_env);
    e->type = TYPE_DECIMAL;
    e->decimal = _v;
    return e;
}

expr*
symbol(Environment *_env, tstr _v)
{
    ASSERT_INV_ENV(_env);
    expr* e = _atom_new(_env);
    e->type = TYPE_SYMBOL;
    e->symbol = _v;
    return e;
}

expr*
string(Environment *_env, tstr _v)
{
    ASSERT_INV_ENV(_env);
    expr* e = _atom_new(_env);
    e->type = TYPE_STRING;
    e->string = _v;
    return e;
}

expr*
_cons_print(expr* _args, char _open, char _close)
{
    /*Handle CAR*/
    if (is_cons(car(_args))) {
        printf("%c", _open);
        if (!is_nil(car(_args)))
            print(car(_args));
        printf("%c", _close);
    } else {
        if (!is_nil(car(_args)))
            print(car(_args));
    }
    if (!is_nil(second(_args)))
        printf(" ");

    /*Handle CDR*/
    if (!is_nil(cdr(_args)))
        print(cdr(_args));
    return _args;
}

expr*
print(expr* _arg)
{
    if (is_nil(_arg)) {
        printf("NIL");
        return _arg;
    }
    switch (_arg->type) {
    case TYPE_CONS:
        _cons_print(_arg, '(', ')');
        break;
    case TYPE_REAL:
        printf("%d", _arg->real);
        break;
    case TYPE_DECIMAL:
        printf("%f", _arg->decimal);
        break;
    case TYPE_SYMBOL:
        printf("%s", _arg->symbol.c_str);
        break;
    case TYPE_STRING:
        printf("\"%s\"", _arg->string.c_str);
        break;
    };
    return _arg;
}

char
_is_whitespace(char _c)
{
    if (_c == ' ' || _c == '\t' || _c == '\n' )
        return 1;
    return 0;
}

char
_can_lex_number(char* _start)
{
    if (*_start > '0' && *_start < '9')
        return 1; 
    return 0;
}

char
_can_lex_string(char* _start)
{
    if (*_start == '"')
        return 1;
    return 0;
}

char
_is_symbol_real_or_decimal(tstr* _num)
/*TODO: Make this more robust..*/
{
    tstr decimal_indicator = tstr_const(".");
    if (tstr_find(_num, &decimal_indicator) == TSTR_INVALID)
        return TYPE_REAL;
    return TYPE_DECIMAL;
}

expr*
_lex_number(Environment *_env, char* _start, int* _cursor)
{
    tstr str;
    int len = 0;
    char type;
    while (!_is_whitespace(_start[len]) && _start[len] != '(' && _start[len] != ')')
        len++;
    str = tstr_n(_start, len+1);
    (*_cursor) += tstr_length(&str)-1;
    type = _is_symbol_real_or_decimal(&str);
    if (type == TYPE_REAL)
        return real(_env, tstr_to_int(&str));
    return decimal(_env, tstr_to_float(&str));
}

expr*
_lex_string(Environment *_env, char* _start, int* _cursor)
{
    int len = 1;
    tstr str;
    while (_start[len] != '"')
        len++;
    str = tstr_n(_start+1, len);
    (*_cursor) += tstr_length(&str)+1;
    return string(_env, str);
}

expr*
_lex_symbol(Environment *_env, char* _start, int* _cursor)
{
    int len = 0;
    tstr str;
    while (!_is_whitespace(_start[len]) && _start[len] != '(' && _start[len] != ')')
        len++;
    str = tstr_n(_start, len+1);
    (*_cursor) += tstr_length(&str)-1;
    return symbol(_env, str);
}

expr*
_expr_lex(Environment* _env, char* _program, int* _cursor)
{
    /*TODO: Does not support special syntax for:
    quote = '
    array = []
    NIL and ()
    t
    */
    ASSERT_INV_ENV(_env);
    assert(_program != NULL && "Invalid program given to lexer");
    assert(_cursor != NULL && "Invalid cursor given to lexer");
    assert(Buildins_len(&_env->buildins) > 0 && "No buildin functions in environment!");

    expr* root = cons(_env, NULL, NULL);
    expr* curr = root;
    expr* extracted;
    char* index;
    while (1) {
        index = _program + (*_cursor)++;
        ERRCHECK_NIL(_env, curr);

        if (_is_whitespace(*index)) {
            continue;
        }
        else if (*index == '\0' || *index == ')') {
            return root;
        }
        else if (*index ==  '(') {
            extracted = _expr_lex(_env, _program, _cursor);
        }
        else if (_can_lex_string(index)) {
            /*NOTE: this is a bad way of giving it index and cursor just for incrementing*/
            extracted = _lex_string(_env, index, _cursor);
        }
        else if (_can_lex_number(index)) {
            extracted = _lex_number(_env, index, _cursor);
        }
        else {
          /*We determine that if it is not
            a string or a number, it must be a symbol*/
            extracted = _lex_symbol(_env, index, _cursor);
        }
        /*Insert extracted into expression*/
        curr->car = extracted;
        curr->cdr = cons(_env, NULL, NULL);
        curr = curr->cdr;
    }
    ASSERT_UNREACHABLE();
}

expr*
read(Environment* _env, char* _program_str)
{
    ASSERT_INV_ENV(_env);
    assert(_program_str != NULL && "Given program str is NULL");
    int cursor = 0;
    expr* lexed = _expr_lex(_env, _program_str, &cursor);
    return lexed;
}

expr*
_eval_function(Environment* _env, expr* _e)
{
    ASSERT_INV_ENV(_env);
    ERRCHECK_NIL(_env, _e);
    int i;
    Buildin* buildin = NULL;
    //_e = car(_e);
    expr* fn;
    expr* args;
    /*Extract function symbol and its arguments*/
    fn = car(car(_e));
    args = cdr(car(_e));
    //args = car(cdr(_e));
    printf("FN:   "); print(fn); printf("\n");
    printf("ARGS: "); print(args); printf("\n");

    /*evaluate arguments before function call*/
    expr* tmp = args;
    while (!is_nil(tmp)) {
        printf("calling eval on: "); print(tmp); printf("\n");
        eval(_env, tmp);
        tmp = first(tmp);
    }

    ERRCHECK_TYPE(_env, fn, TYPE_SYMBOL);
    assert(fn->type == TYPE_SYMBOL && "Recieved something that was not a symbol!");
    ERRCHECK_TYPE(_env, args, TYPE_CONS);
    if (fn != NULL) {
        //printf("looking for function %s\n", fn->symbol.c_str);
        for (i = 0; i < Buildins_len(&_env->buildins); i++) {
            buildin = Buildins_peek(&_env->buildins, i);
            if (tstr_equal(&buildin->name, &fn->symbol)) {
                return buildin->fn(_env, args);
            }
        }
        assert(0 && "Called Function did not exist in buildins!");
    }
    ASSERT_UNREACHABLE();
    return _e;
}

expr*
eval(Environment* _env, expr* _e)
{
    ASSERT_INV_ENV(_env);
    ERRCHECK_NIL(_env, _e);

    switch (_e->type) {
    case TYPE_CONS:
        return _eval_function(_env, _e);
    case TYPE_REAL:
        /*FALLTHROUGH*/
    case TYPE_DECIMAL:
        /*FALLTHROUGH*/
    case TYPE_STRING:
        return _e;
    case TYPE_SYMBOL:
        /*TODO: Check symbol table and return value of symbo*/
        return _e;
    default:
        ASSERT_UNREACHABLE();
    };
    ASSERT_UNREACHABLE();
}



expr*
buildin_quote(Environment* _env, expr* _e)
{
    ASSERT_INV_ENV(_env);
    return _e;
}

expr*
buildin_list(Environment* _env, expr* _e)
{
    ASSERT_INV_ENV(_env);
    return cons(_env, _e, NIL());
}

expr*
buildin_car(Environment* _env, expr* _e)
{
    UNUSED(_env);
    return car(_e);
}

expr*
buildin_cdr(Environment* _env, expr* _e)
{
    UNUSED(_env);
    /*TODO: cdr should return rest of arguments as a list!*/
    return cdr(_e);
    //return buildin_list(_env, cdr(_e));
}

expr*
buildin_first(Environment* _env, expr* _e)
{
    UNUSED(_env);
    return first(_e);
}

expr*
buildin_second(Environment* _env, expr* _e)
{
    UNUSED(_env);
    return second(_e);
}

expr*
buildin_third(Environment* _env, expr* _e)
{
    UNUSED(_env);
    return third(_e);
}

expr*
buildin_fourth(Environment* _env, expr* _e)
{
    UNUSED(_env);
    return fourth(_e);
}

expr*
buildin_cons(Environment* _env, expr* _e)
{
    ASSERT_INV_ENV(_env);
    ERRCHECK_LEN(_env, _e, 2);
    ERRCHECK_NIL(_env, _e);
    return cons(_env, buildin_first(_env, _e), buildin_second(_env, _e));
}

expr*
buildin_len(Environment* _env, expr* _e)
{
    ASSERT_INV_ENV(_env);
    return real(_env, len(_e));
}

expr*
buildin_plus(Environment* _env, expr* _e)
{
    ASSERT_INV_ENV(_env);
    //printf("first="); print_value(first(_e)); printf("\n");
    //printf("second="); print_value(second(_e)); printf("\n");
    //printf("third="); print_value(third(_e)); printf("\n");

    int sum = 1;
    expr* tmp = NULL;
    tmp = cdr(_e);
    while (tmp != NULL) {
        /*Eval cons cell to number*/
        if (car(tmp)->type == TYPE_CONS)
            tmp->car = eval(_env, car(tmp));
        assert(car(tmp)->type == TYPE_REAL);
        sum += car(tmp)->real;
        tmp = cdr(tmp);
    }
    return real(_env, sum);
}
void
Env_add_buildin(Environment* _env, char* _name, buildin_fn _fn)
{
    Buildins_push(&_env->buildins, (Buildin){tstr_(_name), _fn});
}

void
Env_add_core(Environment* _env)
{
    ASSERT_INV_ENV(_env);
    /*List management*/
    Env_add_buildin(_env, "car", buildin_car);
    Env_add_buildin(_env, "cdr", buildin_cdr);
    Env_add_buildin(_env, "quote", buildin_quote);
    //Env_add_buildin(_env, "cons", buildin_cons);
    //Env_add_buildin(_env, "first", buildin_first);
    //Env_add_buildin(_env, "second", buildin_second);
    //Env_add_buildin(_env, "third", buildin_third);
    //Env_add_buildin(_env, "fourth", buildin_fourth);
    /*Arimetrics*/
    //Env_add_buildin(_env, "+", buildin_plus);
    //Env_add_buildin(_env, "-", buildin_minus);
    //Env_add_buildin(_env, "*", buildin_multiply);
    //Env_add_buildin(_env, "/", buildin_divide);

    /*Utils*/
    //Env_add_buildin(_env, "eval", buildin_eval);
}

#endif /*YAL_IMPLEMENTATION*/
#endif /*YAL_H*/
