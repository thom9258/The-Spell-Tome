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


#define DBPRINT(before, expr) \
    printf(before); print(expr);  printf("\n");

#define foreach_in_list(expr) while (!is_nil(cdr(expr)))

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
char is_cons(expr* _args);
char is_symbol(expr* _args);
char is_val(expr* _args);

/*List management*/
int len(expr* _args);
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
is_symbol(expr* _args)
{
    if (!is_nil(_args) && _args->type == TYPE_SYMBOL)
        return 1;
    return 0;
}

char
car_is_list(expr* _args)
{
    if (!is_nil(_args) &&
        _args->type == TYPE_CONS &&
        is_cons(_args->car)
        )
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

int
len(expr* _args)
{
    /*http://clhs.lisp.se/Body/f_list_l.htm*/
    if (is_nil(_args)) return 0;
    if (is_val(_args)) return 1;
    int cnt = 0;
    expr* tmp = _args;
    foreach_in_list(tmp) {
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
    //if (is_dotted(_arg)) {
    //    print(car(_arg));
    //    printf(" . ");
    //    print(cdr(_arg));
    //}
    //if (car_is_list(_arg)) {
    //    _list_print(car(_arg), '(', ')');
    //}
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
_looks_like_number(char* _start)
{
    if (*_start > '0' && *_start < '9')
        return 1; 
    return 0;
}

char
_looks_like_string(char* _start)
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

#if 0
expr*
_expr_lex(Environment* _env, char* _program, int* _cursor)
{
    /*TODO: Does not support special syntax for:
    quote = '
    array = []
    NIL and t
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
        else if (_looks_like_string(index)) {
            /*NOTE: this is a bad way of giving it index and cursor just for incrementing*/
            extracted = _lex_string(_env, index, _cursor);
        }
        else if (_looks_like_number(index)) {
            extracted = _lex_number(_env, index, _cursor);
        }
        else {
            extracted = _lex_symbol(_env, index, _cursor);
        }
        /*Insert extracted into expression*/
        curr->car = extracted;
        curr->cdr = cons(_env, NULL, NULL);
        curr = curr->cdr;
    }
    ASSERT_UNREACHABLE();
}
#endif

expr*
_lex(Environment* _env, char* _program, int* _cursor)
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

    expr* first = NULL;
    expr* curr = cons(_env, NULL, NULL);
    expr* extracted;
    char* index;
    while (1) {
        index = _program + (*_cursor)++;
        ERRCHECK_NIL(_env, curr);

        if (_is_whitespace(*index)) {
            continue;
        }
        else if (*index == '\0' || *index == ')') {
            return first;
        }
        else if (*index ==  '(') {
            extracted = _lex(_env, _program, _cursor);
        }
        else if (_looks_like_string(index)) {
            /*NOTE: this is a bad way of giving it index and cursor just for incrementing*/
            extracted = _lex_string(_env, index, _cursor);
        }
        else if (_looks_like_number(index)) {
            extracted = _lex_number(_env, index, _cursor);
        }
        else {
            extracted = _lex_symbol(_env, index, _cursor);
        }
        /*Insert extracted into expression*/
        curr->car = extracted;
        if (!first)
            first = curr; 
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
    expr* lexed = _lex(_env, _program_str, &cursor);
    return lexed;
}

Buildin*
_find_buildin(Environment* _env, expr* _fnsym)
{
    Buildin* buildin = NULL;
    int i;
    if (_fnsym == NULL) ASSERT_UNREACHABLE();
    assert(is_symbol(_fnsym));
    ERRCHECK_TYPE(_env, _fnsym, TYPE_SYMBOL);

    //printf("looking for buildin '%s'\n", _fnsym->symbol.c_str);
    for (i = 0; i < Buildins_len(&_env->buildins); i++) {
        buildin = Buildins_peek(&_env->buildins, i);
        if (tstr_equal(&buildin->name, &_fnsym->symbol)) {
            //printf("found '%s'\n", buildin->name.c_str);
            return buildin;
        }
    }
    assert(0 && "Given Function did not exist in buildins!");
    ASSERT_UNREACHABLE();
}

expr*
eval_all_args(Environment* _env, expr* _e)
{
    expr* tmp;
    if (!is_cons(_e))
        return eval(_env, _e);
    tmp = _e;
    foreach_in_list(tmp) {
        //printf("calling eval_all_args on: "); print(car(tmp)); printf("\n");
        /*TODO: Error in this fn, look at quote (a b) evaling to ((a b))*/
        tmp->car = eval(_env, car(tmp));
        tmp = cdr(tmp);
    }
    return _e;
}

expr*
_eval_function(Environment* _env, expr* _e)
{
    expr* fn;
    expr* args;
    Buildin* buildin;
    ASSERT_INV_ENV(_env);
    if (is_nil(_e))
        return _e;
    /*Extract function symbol and its arguments*/
    /*NOTE: this should not be done*/
    //_e = car(_e);
    fn = car(_e);
    args = cdr(_e);
    //DBPRINT("(_eval_function) INPUT: ", _e);
    DBPRINT("(_eval_function) FN:    ", fn);
    DBPRINT("(_eval_function) ARGS:  ", args);

    assert(fn->type == TYPE_SYMBOL && "Recieved something that was not a symbol!");
    ERRCHECK_TYPE(_env, args, TYPE_CONS);
    buildin = _find_buildin(_env, fn);
    return buildin->fn(_env, args);
}

expr*
eval(Environment* _env, expr* _e)
{
    ASSERT_INV_ENV(_env);
    if (is_nil(_e))
        return _e;

    /*We are sanitizing inputs here by removing a cons cell*/
    if (is_cons(_e) && is_cons(car(_e)))
        return _eval_function(_env, car(_e));
    else if (is_cons(_e))
        return _eval_function(_env, _e);

    /*TODO: Check symbol table and return value of symbol*/
    if (is_symbol(_e))
        return _e;
    return _e;
}

expr*
buildin_quote(Environment* _env, expr* _e)
{
    ASSERT_INV_ENV(_env);
    printf("buidlin quote called\n");
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
    printf("buidlin car called\n");
    _e = eval_all_args(_env, _e);
    DBPRINT("result of eval in car: ", _e);
    return car(_e);
    /*TODO: This cant be right, lots of nesting errors!*/
    //return car(car(car(_e)));
}

expr*
buildin_cdr(Environment* _env, expr* _e)
{
    UNUSED(_env);
    _e = eval_all_args(_env, _e);
    DBPRINT("result of eval in cdr: ", _e);
    return cdr(_e);
    /*TODO: This cant be right, lots of nesting errors!*/
    //return cdr(car(car(_e)));
}

expr*
buildin_cons(Environment* _env, expr* _e)
{
    UNUSED(_env);
    _e = eval_all_args(_env, _e);
    DBPRINT("result of eval in consr: ", _e);
    return cons(_env, first(_e), second(_e));
}

expr*
buildin_plus(Environment* _env, expr* _e)
{
    ASSERT_INV_ENV(_env);
    int sum = 0;
    expr* tmp = NULL;
    tmp = eval_all_args(_env, _e);

    foreach_in_list(tmp) {
        assert(car(tmp)->type == TYPE_REAL);
        sum += car(tmp)->real;
        tmp = cdr(tmp);
    }
    return real(_env, sum);
}

expr*
buildin_minus(Environment* _env, expr* _e)
{
    ASSERT_INV_ENV(_env);
    int sum = 0;
    expr* tmp = NULL;
    tmp = eval_all_args(_env, _e);
    sum = car(tmp)->real;
    tmp = cdr(tmp);

    foreach_in_list(tmp) {
        assert(car(tmp)->type == TYPE_REAL);
        sum -= car(tmp)->real;
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
    Env_add_buildin(_env, "cons", buildin_cons);
    //Env_add_buildin(_env, "first", buildin_first);
    //Env_add_buildin(_env, "second", buildin_second);
    //Env_add_buildin(_env, "third", buildin_third);
    //Env_add_buildin(_env, "fourth", buildin_fourth);
    /*Arimetrics*/
    Env_add_buildin(_env, "+", buildin_plus);
    Env_add_buildin(_env, "-", buildin_minus);
    //Env_add_buildin(_env, "*", buildin_multiply);
    //Env_add_buildin(_env, "/", buildin_divide);

    /*Utils*/
    //Env_add_buildin(_env, "eval", buildin_eval);
}

#endif /*YAL_IMPLEMENTATION*/
#endif /*YAL_H*/
