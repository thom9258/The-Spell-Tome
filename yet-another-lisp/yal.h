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

#define _IS_WHITESPACE(C)     (((C) == ' ' || (C) == '\t' || (C) == '\n') ? 1 : 0)
#define _LOOKS_LIKE_NUMBER(C) (((C) > '0' && (C) < '9') ? 1 : 0)
#define _LOOKS_LIKE_STRING(C) (((C) == '"') ? 1 : 0)

#define DBPRINT(before, expr) \
    printf(before); printexpr(expr);  printf("\n");

#define while_not_nil(expr) while (!is_nil(cdr(expr)))

enum TYPE {
    TYPE_CONS = 0,
    TYPE_REAL,
    TYPE_DECIMAL,
    TYPE_SYMBOL,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_DICTIONARY,

    TYPE_COUNT
};

const char* TYPE_TO_STR[TYPE_COUNT] = {
    "TYPE_CONS",
    "TYPE_REAL",
    "TYPE_DECIMAL",
    "TYPE_SYMBOL",
    "TYPE_STRING", 
    "TYPE_ARRAY",
    "TYPE_DICTIONARY"
};

typedef struct {
    char type;
    char* info;
}usermsg;

/*Forward declarations*/
typedef struct expr expr;
typedef struct Environment Environment;

typedef usermsg(*buildin_fn)(Environment* _env, expr** _out, expr* _in);

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
#define UNIMPLEMENTED(fun) assert(0 && "UNIMPLEMENTED: " && fun)

#define ASSERT_UNREACHABLE() \
    assert(0 && "Fatal Error occoured, reached unreachable code!")

#define ASSERT_INV_ENV(env) \
    assert(env != NULL && "Invalid environment is used in code! Ptr is NULL!")

#define ASSERT_NOMOREMEMORY(env) \
    assert(0 && "Was unable to allocate more memory!")

#define USERERR(msg) (usermsg) {1, msg}

#define USERMSG_OK() (usermsg) {0, "OK"}

#define USERERR_LEN(fn, expected) \
    USERERR("LENGTH OF INPUT IS INVALID, EXPECTED " #expected, fn)

#define USERERR_TYPE(fn, expected) \
    USERERR("TYPE OF INPUT IS INVALID, EXPECTED " #expected, fn)

#define USERERR_NUMBER(fn) \
    USERERR("INPUT WAS NOT A NUMVER", fn)

#define USERMSG_IS_ERROR(MSGPTR) (((MSGPTR)->type == 0) ? 0 : 1)

#define return_if_error(msgdst, call)                                   \
    do {                                                                \
    (msgdst) = call;                                                    \
    if (USERMSG_IS_ERROR(&(msgdst)))                                    \
        return (msgdst);                                                \
    } while (0)

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
expr* printexpr(expr* _args);
tstr stringifyexpr(expr* _args);

/*Core*/
usermsg read(Environment* _env, expr** _out, char* _program_str);
usermsg eval(Environment* _env, expr** _out, expr* _in);
usermsg progc(Environment* _env, expr** _out, expr* _in);


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
    return _env;
}

void
Env_destroy(Environment* _env)
{
    if (_env == NULL)
        return;
    sbAllocator_destroy(&_env->expr_allocator);
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

//char
//car_is_list(expr* _args)
//{
//    if (!is_nil(_args) &&
//        _args->type == TYPE_CONS &&
//        is_cons(_args->car)
//        )
//        return 1;
//    return 0;
//}

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
    while_not_nil(tmp) {
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
    if (e == NULL)
        ASSERT_NOMOREMEMORY(env);
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

//expr*
//_cons_print(expr* _args, char* _open, char* _close)
//{
//    /*Handle CAR*/
//    if (is_cons(car(_args))) {
//        printf("%s", _open);
//        print(car(_args));
//        printf("%s", _close);
//    } else {
//        if (!is_nil(car(_args)))
//            print(car(_args));
//    }
//    if (!is_nil(second(_args)))
//        printf(" ");
//
//    /*Handle CDR*/
//    if (!is_nil(cdr(_args)))
//        print(cdr(_args));
//    return _args;
//}

expr*
_list_print(expr* _args, char* _open, char* _close)
{
    while_not_nil(_args) {
        if (is_cons(car(_args)))
            printf("%s", _open);
        printexpr(car(_args));
        if (is_cons(car(_args)) && !is_nil(car(_args)))
            printf("%s", _close);
        if (!is_nil(second(_args)))
            printf(" ");
        _args = cdr(_args);
    }
    return _args;
}

expr*
printexpr(expr* _arg)
{
    if (is_nil(_arg)) {
        printf("NIL");
        return _arg;
    }
    if (is_dotted(_arg)) {
        printf("(");
        printexpr(car(_arg));
        printf(" . ");
        printexpr(cdr(_arg));
        printf(")");
        return _arg;
    }
    switch (_arg->type) {
    case TYPE_CONS:
        _list_print(_arg, "(", ")");
        break;
    case TYPE_ARRAY:
        _list_print(_arg, "[", "]");
        break;
    case TYPE_DICTIONARY:
        _list_print(_arg, "#{", "}");
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

tstr
_stringify_list(expr* _args, char* _open, char* _close)
{
    tstr space = tstr_const(" ");
    tstr open = tstr_(_open);
    tstr close = tstr_(_close);
    tstr dst;
    tstr tmp;

    while_not_nil(_args) {
        if (is_cons(car(_args)))
            tstr_concat(&dst, &open);

        tmp = stringifyexpr(car(_args));
        tstr_concat(&dst, &tmp);
        tstr_destroy(&tmp);

        if (is_cons(car(_args)) && !is_nil(car(_args)))
            tstr_concat(&dst, &close);

        if (!is_nil(second(_args)))
            tstr_concat(&dst, &space);

        _args = cdr(_args);
    }

    tstr_destroy(&open);
    tstr_destroy(&close);
    return dst;
}

tstr
stringifyexpr(expr* _arg)
{
    tstr lparen = tstr_const("(");
    tstr rparen = tstr_const(")");
    tstr dotted = tstr_const(" . ");
    tstr quote = tstr_const("\"");
    tstr nilstr = tstr_const("NIL");
    tstr dst;

    if (is_nil(_arg)) {
        tstr_concat(&dst, &nilstr);
        return dst;
    }
    if (is_dotted(_arg)) {
        tstr carstr = stringifyexpr(car(_arg));
        tstr cdrstr = stringifyexpr(cdr(_arg));
        tstr_concatva(&dst, 5,
                      &lparen, carstr, &dotted, cdrstr, &rparen);
        tstr_destroy(&carstr);
        tstr_destroy(&cdrstr);
        return dst;
    }
    switch (_arg->type) {
    case TYPE_CONS:
        dst = _stringify_list(_arg, "(", ")");
        break;
    case TYPE_ARRAY:
        dst  =_stringify_list(_arg, "(", ")");
        break;
    case TYPE_DICTIONARY:
        dst = _stringify_list(_arg, "(", ")");
        break;
    case TYPE_REAL:
        tstr_from_int(&dst, _arg->real);
        break;
    case TYPE_DECIMAL:
        tstr_from_float(&dst, _arg->decimal);
        break;
    case TYPE_SYMBOL:
        tstr_duplicate(&_arg->symbol, &dst);
        printf("%s", _arg->symbol.c_str);
        break;
    case TYPE_STRING:
        tstr_concatva(&dst, 3,
                      &quote, &_arg->string, &quote);
        break;
    };
    return dst;
}

//char
//_is_whitespace(char _c)
//{
//    if (_c == ' ' || _c == '\t' || _c == '\n' )
//        return 1;
//    return 0;
//}
//
//char
//_looks_like_number(char* _start)
//{
//    if (*_start > '0' && *_start < '9')
//        return 1; 
//    return 0;
//}
//
//char
//_looks_like_string(char* _start)
//{
//    if (*_start == '"')
//        return 1;
//    return 0;
//}


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
    while (!_IS_WHITESPACE(_start[len]) && _start[len] != '(' && _start[len] != ')')
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
    while (!_IS_WHITESPACE(_start[len]) && _start[len] != '(' && _start[len] != ')')
        len++;
    str = tstr_n(_start, len+1);
    (*_cursor) += tstr_length(&str)-1;
    return symbol(_env, str);
}

expr*
_lex(Environment* _env, char* _program, int* _cursor)
{
    /*TODO: Does not support special syntax for:
    quote = '
    array = []
    dotted lists (a . 34)
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
        if (_IS_WHITESPACE(*index)) {
            continue;
        }
        else if (*index == '\0' || *index == ')') {
            return first;
        }
        else if (*index ==  '(') {
            extracted = _lex(_env, _program, _cursor);
        }
        else if (_LOOKS_LIKE_STRING(*index)) {
            /*NOTE: this is a bad way of giving it index and cursor just for incrementing*/
            extracted = _lex_string(_env, index, _cursor);
        }
        else if (_LOOKS_LIKE_NUMBER(*index)) {
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


usermsg
read(Environment* _env, expr** _out, char* _program_str)
{
    ASSERT_INV_ENV(_env);
    assert(_program_str != NULL && "Given program str is NULL");
    int cursor = 0;
    expr* lexed = _lex(_env, _program_str, &cursor);
    *_out = lexed;
    return USERMSG_OK();
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
    printf("looking for buildin '%s'\n", _sym->symbol.c_str);
    for (i = 0; i < Buildins_len(&_env->buildins); i++) {
        buildin = Buildins_peek(&_env->buildins, i);
        if (tstr_equal(&buildin->name, &_sym->symbol)) {
            printf("found '%s'\n", buildin->name.c_str);
            return buildin;
        }
    }
    return NULL;
}

usermsg
progc(Environment* _env, expr** _out, expr* _in)
{
    usermsg msg;
    expr* root = cons(_env, NULL, NULL);
    expr* outtmp = root;
    expr* tmp = _in;
    ASSERT_INV_ENV(_env);
    DBPRINT("input to progc: ", _in);
    if (!is_cons(_in))
        return USERERR("'progc' Expected a list of eval'able arguments!");
    while_not_nil(tmp) {
        return_if_error(msg, eval(_env, &(outtmp->car), car(tmp)));
        DBPRINT("progc evalled: ", car(tmp));
        DBPRINT("to ", car(outtmp));
        outtmp->cdr = cons(_env, NULL, NULL);
        outtmp = cdr(outtmp);
        tmp = cdr(tmp);
    }
    *_out = root;
    DBPRINT("result of progc: ", *_out);
    return USERMSG_OK();
}

usermsg
eval(Environment* _env, expr** _out, expr* _in)
{
    expr* fn;
    expr* args;
    usermsg msg;
    Buildin* buildin;

    ASSERT_INV_ENV(_env);
    if (is_nil(_in))
        return USERERR("Expected non-nil input for function 'eval'.");
    if (is_val(_in))
    /*We are sanitizing inputs here by removing a cons cell*/
    /*TODO: This is what i think goes wrong*/
    //if (is_cons(_in) && is_cons(car(_in)))
    //return _eval_function(_env, car(_e));
    _in = car(_in);

    if (is_cons(_in)) {
        //return _eval_function(_env, _out, _in);
        /*Extract function symbol and its arguments*/
        fn = car(_in);
        args = cdr(_in);
        DBPRINT("(_eval_function) FN:   ", fn);
        DBPRINT("(_eval_function) ARGS: ", args);
        if (fn->type != TYPE_SYMBOL)
            return USERERR("First value in list to eval was not a callable symbol!");
        buildin = _find_buildin(_env, fn);
        msg = buildin->fn(_env, _out, args);
        *_out =  _in;
        return msg;
    }

    /*TODO: Check symbol table and return value of symbol*/
    if (is_symbol(_in))
        *_out =  _in;

    return USERMSG_OK();
}

/*********************************************************************/
/* Buildin Functions */
/*********************************************************************/

usermsg
buildin_quote(Environment* _env, expr** _out, expr* _in)
{
    ASSERT_INV_ENV(_env);
    printf("buidlin quote called\n");
    *_out = _in;
    return USERMSG_OK();
}

usermsg
buildin_list(Environment* _env, expr** _out, expr* _in)
{
    ASSERT_INV_ENV(_env);
    *_out = cons(_env, _in, NIL());
    return USERMSG_OK();
}

usermsg
buildin_car(Environment* _env, expr** _out, expr* _in)
{
    usermsg msg;
    expr* args;
    ASSERT_INV_ENV(_env);
    return_if_error(msg, progc(_env, &args, _in));
    *_out =  car(args);
    return USERMSG_OK();
}

usermsg
buildin_cdr(Environment* _env, expr** _out, expr* _in)
{
    usermsg msg;
    expr* args;
    ASSERT_INV_ENV(_env);
    msg = progc(_env, &args, _in);
    if (USERMSG_IS_ERROR(&msg)) return msg;
    DBPRINT("result of eval in cdr: ", args);
    *_out =  cdr(args);
    return USERMSG_OK();
}

usermsg
buildin_cons(Environment* _env, expr** _out, expr* _in)
{
    usermsg msg;
    expr* args;
    ASSERT_INV_ENV(_env);
    msg = progc(_env, &args, _in);
    if (USERMSG_IS_ERROR(&msg)) return msg;
    DBPRINT("result of eval in cons: ", args);
    *_out =  cons(_env, first(args), second(args));
    return USERMSG_OK();
}

void
_arimetric_type_convert(Environment* _env, expr** _target, expr* _new)
{
    expr* tmp;
    if (!is_val(*_target) || !is_val(_new))
        return;
    else if ((*_target)->type == TYPE_DECIMAL)
        return;
    else if (_new->type == TYPE_DECIMAL && (*_target)->type == TYPE_REAL) {
        tmp = *_target;
        *_target = decimal(_env, tmp->real);
    }
}

usermsg
buildin_plus(Environment* _env, expr** _out, expr* _in)
{
    usermsg msg;
    expr* args;
    int sum = 0;
    expr* tmp;
    ASSERT_INV_ENV(_env);
    msg = progc(_env, &args, _in);
    if (USERMSG_IS_ERROR(&msg)) return msg;
    DBPRINT("result of progc in + ", args);

    tmp = args;
    while_not_nil(tmp) {
        if (!is_val(car(tmp)))
            return USERERR("Unable to call 'plus' on non-value argument");
        DBPRINT("plus added ", car(tmp))
        sum += car(tmp)->real;
        tmp = cdr(tmp);
    }
    *_out = real(_env, sum); 
    return USERMSG_OK();
}

usermsg
buildin_minus(Environment* _env, expr** _out, expr* _in)
{
    usermsg msg;
    expr* args;
    int sum = 0;
    expr* tmp;
    ASSERT_INV_ENV(_env);
    msg = progc(_env, &args, _in);
    if (USERMSG_IS_ERROR(&msg)) return msg;
    tmp = args;

    /*Check for non-numbers*/
    while_not_nil(tmp) {
        if (!is_val(car(tmp)))
            return USERERR("Unable to call 'minus' on non-value argument");
        tmp = cdr(tmp);
    }
    /*Make sure to return if there are no numbers in list*/
    tmp = _in;
    if (is_nil(car(tmp))) {
        *_out = real(_env, sum); 
        return USERMSG_OK();
    }
    sum += car(tmp)->real;
    tmp = cdr(tmp);
    while_not_nil(tmp) {
        sum -= car(tmp)->real;
        tmp = cdr(tmp);
    }
    *_out = real(_env, sum); 
    return USERMSG_OK();
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
