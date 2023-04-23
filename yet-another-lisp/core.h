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

#ifndef CORE_H
#define CORE_H

#include "defs.h"
#include "error.h"
#include "tstr.h"

Environment* Environment_new(Environment* _env);
void Environment_destroy(Environment* _env);
char is_nil(expr* _expr);
int len(expr* _e);
expr* car(expr* _expr);
expr* cdr(expr* _expr);
expr* first(expr* _expr);
expr* second(expr* _expr);
expr* third(expr* _expr);
expr* expr_new(Environment *_env);
expr* cons(Environment *_env, expr* _car, expr* _cdr);
expr* real(Environment *_env, int _v);
expr* decimal(Environment *_env, float _v);
expr* csymbol(Environment *_env, char* _v);
expr* symbol(Environment *_env, tstr _v);
expr* string(Environment *_env, tstr _v);
void value2str(tstr* _dst, expr* _expr);
void expr2str(tstr* _dst, expr* _e);
expr* str_read(Environment* _e, char* _p);


/******************************************************************************/
#define CORE_IMPLEMENTATION
#ifdef CORE_IMPLEMENTATION

/*Forward declaraitons of buildins used in core*/
expr* buildin_read(Environment* _e, expr* _inp);
expr* buildin_print(Environment* _e, expr* _inp);

expr*
_try_nest_sanitize(expr* _e)
/*Nest sanitization is needed where the list is offset to car of the
root cell. this means that we need to sanitize ((write 4)) to (write 4)
*/
{
    // printf("this type = %s\n", type2str(car(_e)->type));
    // printf("first type = %s\n", type2str(first(_e)->type));
    // printf("second type = %s\n", type2str(second(_e)->type));
    if (is_nil(_e)) return NIL;
    if (first(_e)->type == TYPE_CCONS && second(_e)->type == TYPE_NIL)
        return first(_e);
    return _e;
}

char*
type2str(char t)
{
    switch (t) {
    case TYPE_NIL:     return "TYPE_NIL";
    case TYPE_CCONS:   return "TYPE_CCONS";
    case TYPE_REAL:    return "TYPE_REAL";
    case TYPE_DECIMAL: return "TYPE_DECIMAL";
    case TYPE_SYMBOL:  return "TYPE_SYMBOL";
    case TYPE_STRING:  return "TYPE_STRING";
    default:
        ASSERT_UNREACHABLE();
    };
    ASSERT_UNREACHABLE();
    return NULL;
}

Environment*
Environment_new(Environment* _env)
{
    *_env = (Environment){0};
    sbAllocator_init(&_env->expr_allocator, sizeof(expr));
    /*TODO: 30 is a arbitrary number, change when you know size of buildins*/
    Buildins_initn(&_env->buildins, 30);
    _env->errormanager = (ErrorManager) {0};
    return _env;
}

void
Environment_destroy(Environment* _env)
{
    if (_env == NULL)
        return;
    sbAllocator_destroy(&_env->expr_allocator);
    Errors_clear(&_env->errormanager);
}

char
is_nil(expr* _e)
{
    if (_e == NULL)           return 1;
    if (_e == NIL)            return 1;
    if (_e->type == TYPE_NIL) return 1;
    return 0;
}

char
is_value(expr* _e)
{
    if (is_nil(_e))
        return 0;
    if (_e->type == TYPE_CCONS) return 0;
    return 1;
}

int
len(expr* _e)
{
    /*http://clhs.lisp.se/Body/f_list_l.htm*/
    if (is_nil(_e)) return 0;
    int cnt = 1;
    expr* tmp = _e;
    while (tmp != NULL) {
        tmp = cdr(tmp);
        if (is_nil(tmp))
            break;
        cnt++;
    }
    return cnt;
}

expr*
expr_new(Environment *_env)
{
    ASSERT_INV_ENV(_env);
    expr* e = (expr*)sbAllocator_get(&_env->expr_allocator);
    ERRCHECK_NILALLOC(_env, e);
    e->type = TYPE_CCONS;
    e->car = NULL;
    e->cdr = NULL;
    return e;
}

expr*
cons(Environment *_env, expr* _car, expr* _cdr)
{
    ASSERT_INV_ENV(_env);
    expr* e = expr_new(_env);
    ERRCHECK_NIL(_env, e);
    e->car = _car;
    e->cdr = _cdr;
    return e;
}

expr*
car(expr* _expr)
{
    if (is_nil(_expr)) return NIL;
    if (is_nil(_expr->car)) return NIL;
    return _expr->car;
}

expr*
cdr(expr* _expr)
{
    if (is_nil(_expr)) return NIL;
    if (is_nil(_expr->cdr)) return NIL;
    return _expr->cdr;
}

expr* first(expr* _expr)
{ return car(_expr); }
expr* second(expr* _expr)
{ return car(cdr(_expr)); }
expr* third(expr* _expr)
{ return car(cdr(cdr(_expr))); }
expr* fourth(expr* _expr)
{ return car(cdr(cdr(cdr(_expr)))); }
expr* fifth(expr* _expr) 
{ return car(cdr(cdr(cdr(cdr(_expr))))); }
expr* sixth(expr* _expr)
{ return car(cdr(cdr(cdr(cdr(cdr(_expr)))))); }
expr* seventh(expr* _expr)
{ return car(cdr(cdr(cdr(cdr(cdr(cdr(_expr))))))); }
expr* eighth(expr* _expr)
{ return car(cdr(cdr(cdr(cdr(cdr(cdr(cdr(_expr)))))))); }
expr* ninth(expr* _expr)
{ return car(cdr(cdr(cdr(cdr(cdr(cdr(cdr(cdr(_expr))))))))); }
expr* tenth(expr* _expr)
{ return car(cdr(cdr(cdr(cdr(cdr(cdr(cdr(cdr(cdr(_expr)))))))))); }

expr*
nth(int _n, expr* _expr)
{
    int i;
    expr* tmp = _expr;
    if (_n < 0)
        return NIL;
    for (i = 0; i < _n; i++)
        tmp = cdr(tmp);
    return car(tmp);
}

expr*
real(Environment *_env, int _v)
{
    ASSERT_INV_ENV(_env);
    expr* e = expr_new(_env);
    ERRCHECK_NIL(_env, e);
    e->type = TYPE_REAL;
    e->real = _v;
    return e;
}

expr*
decimal(Environment *_env, float _v)
{
    ASSERT_INV_ENV(_env);
    expr* e = expr_new(_env);
    ERRCHECK_NIL(_env, e);
    e->type = TYPE_DECIMAL;
    e->decimal = _v;
    return e;
}

expr*
symbol(Environment *_env, tstr _v)
{
    ASSERT_INV_ENV(_env);
    expr* e = expr_new(_env);
    ERRCHECK_NIL(_env, e);
    e->type = TYPE_SYMBOL;
    e->symbol = _v;
    return e;
}

expr*
csymbol(Environment *_env, char* _v)
{
    return symbol(_env, tstr_(_v));
}

expr*
string(Environment *_env, tstr _v)
{
    ASSERT_INV_ENV(_env);
    expr* e = expr_new(_env);
    ERRCHECK_NIL(_env, e);
    e->type = TYPE_STRING;
    e->string = _v;
    return e;
}

void
value2str(tstr* _dst, expr* _expr)
{
    tstr dquote = tstr_const("\"");
    tstr_destroy(_dst);
    if (is_nil(_expr)) {
        *_dst =  tstr_("NIL");
        return;
    }
    switch (_expr->type) {
    case TYPE_REAL:
        tstr_from_int(_dst, _expr->real);
        break;
    case TYPE_DECIMAL:
        tstr_from_float(_dst, _expr->decimal);
        break;
    case TYPE_SYMBOL:
        tstr_duplicate(_dst, &_expr->symbol);
        break;
    case TYPE_STRING:
        tstr_concat(_dst, &dquote);
        tstr_concat(_dst, &_expr->string);
        tstr_concat(_dst, &dquote);
        break;
    };
}

void
expr2str(tstr* _dst, expr* _e)
{
    tstr openparen = tstr_const("(");
    tstr closeparen = tstr_const("(");
    tstr space = tstr_const(" ");
    tstr tmp = {0};

    /*Manage CAR*/
    if (!is_nil(car(_e))) {
        /*Manage nested expr*/
        if (car(_e)->type == TYPE_CCONS) {
            tstr_concat(_dst, &openparen);
            expr2str(_dst, car(_e));
            tstr_concat(_dst, &closeparen);
        }
        /*Manage value*/
        else {
            value2str(&tmp, _e);
            tstr_concat(_dst, &tmp);
            tstr_destroy(&tmp);
        }
        /*add a seperator unless this is the last value*/
        if (!is_nil(car(cdr(_e))))
            tstr_concat(_dst, &space);
       
    }
    /*Manage CDR - Keep going until we reach the end of expr list*/
    if (!is_nil(cdr(_e))) {
        expr2str(_dst, cdr(_e));
    }
}
expr*
str_read(Environment* _e, char* _p)
{
    /*TODO: expression must be encased in parens (), otherwise we need to throw
      error, insead of segfaulting down the line!
    */
    ASSERT_INV_ENV(_e);
    assert(_p != NULL && "Given program is NULL");
    expr* inp = cons(_e,
                     string(_e, tstr_(_p)),
                     NIL
        );
    return buildin_read(_e, inp);
}

#endif /*CORE_IMPLEMENTATION*/
#endif /*CORE_H*/
