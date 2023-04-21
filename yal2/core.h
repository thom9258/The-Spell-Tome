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
- [0.1] Implemented a lexer for reading of s expressions.
- [0.1] Created AST structure and imported base lib functionality.
- [0.0] Initialized library.


*/

#ifndef CORE_H
#define CORE_H

#include "defs.h"
#include "tstr.h"

#ifndef CUSTOM_FMT
#define MARKED_ID   "'" 
#define LIST_OPEN   "("
#define LIST_CLOSE  ")"
#endif /*CUSTOM_FMT*/

/* ======================================
 * AST & Atom Manipulation and management  
 * */

#define CCONS_NUMBER(ccons) \
    ( ((ccons)->type == REAL) ? (ccons)->real : (ccons)->decimal )



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
#if 0
expr* buildin(Environment *_env, buildin_fn _buildin, char* _symbol);
#endif
void expr_lex(Environment* _env, expr* _root, char* _program, int* _cursor);
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
        ERRCHECK_UNREACHABLE();
    };
    ERRCHECK_UNREACHABLE();
    return NULL;
}

Environment*
Environment_new(Environment* _env)
{
    *_env = (Environment){0};
    sbAllocator_init(&_env->expr_allocator, sizeof(expr));
    tstrBuffer_init(&_env->stdout_buffer, 128);
    /*TODO: 30 is a arbitrary number, change when you know size of buildins*/
    Buildins_initn(&_env->buildins, 30);
    return _env;
}

void
Environment_destroy(Environment* _env)
{
    sbAllocator_destroy(&_env->expr_allocator);
    tstrBuffer_destroy(&_env->stdout_buffer);
}

char
is_nil(expr* _e)
{
    if (_e == NULL)           return 1;
    if (_e == NIL)            return 1;
    if (_e->type == TYPE_NIL) return 1;
    return 0;
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
    ERRCHECK_INV_ENV(_env);
    expr* e = (expr*)sbAllocator_get(&_env->expr_allocator);
    ERRCHECK_INV_ALLOC(e);
    e->type = TYPE_CCONS;
    e->car = NULL;
    e->cdr = NULL;
    return e;
}

expr*
cons(Environment *_env, expr* _car, expr* _cdr)
{
    ERRCHECK_INV_ENV(_env);
    expr* e = expr_new(_env);
    ERRCHECK_NIL(e);
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
    ERRCHECK_INV_ENV(_env);
    expr* e = expr_new(_env);
    if (e == NULL) return NULL;
    e->type = TYPE_REAL;
    e->real = _v;
    return e;
}

expr*
decimal(Environment *_env, float _v)
{
    ERRCHECK_INV_ENV(_env);
    expr* e = expr_new(_env);
    ERRCHECK_NIL(e);
    e->type = TYPE_DECIMAL;
    e->decimal = _v;
    return e;
}

expr*
symbol(Environment *_env, tstr _v)
{
    ERRCHECK_INV_ENV(_env);
    expr* e = expr_new(_env);
    ERRCHECK_NIL(e);
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
    ERRCHECK_INV_ENV(_env);
    expr* e = expr_new(_env);
    ERRCHECK_NIL(e);
    e->type = TYPE_STRING;
    e->string = _v;
    return e;
}

#if 0
/*TODO: maybe we could at lex time check for arimetrics and insert them directly
as buildin functions, so we dont have to look for them in buildins list?*/
expr*
buildin(Environment *_env, buildin_fn _fn, char* _name)
{
    ERRCHECK_INV_ENV(_env);
    expr* e = expr_new(_env);
    ERRCHECK_NIL(e);
    e->type = TYPE_BUILDIN;
    e->buildin.fn = _fn;
    e->buildin.name = tstr_(_name);
    return e;
}
#endif

void
print_value(expr* _e)
{
    if (_e == NULL)
        return;
    switch (_e->type) {
    case TYPE_NIL:
        printf("NIL");
        break;
    case TYPE_REAL:
        printf("%d", _e->real);
        break;
    case TYPE_DECIMAL:
        printf("%f", _e->decimal);
        break;
    case TYPE_SYMBOL:
        printf("%s", _e->symbol.c_str);
        break;
    case TYPE_STRING:
        printf("\"%s\"", _e->string.c_str);
        break;
    default:
        ERRCHECK_UNREACHABLE();
    };
}

void
tprint_value(expr* _e)
{
    ERRCHECK_TYPECHECK(_e, TYPE_CCONS);
    ERRCHECK_TYPECHECK(_e, TYPE_NIL);
    if (is_nil(_e))
        return;
    switch (_e->type) {
    case TYPE_REAL:
        printf("{TREAL}[%d]\n", _e->real);
        break;
    case TYPE_DECIMAL:
        printf("{TDECI}[%f]\n", _e->decimal);
        break;
    case TYPE_SYMBOL:
        printf("{TSYMB}[%s]\n", _e->symbol.c_str);
        break;
    case TYPE_STRING:
        printf("{TSTRI}[\"%s\"]\n", _e->string.c_str);
        break;
    default:
        ERRCHECK_UNREACHABLE();
    };
}

char
_is_whitespace(char _c)
{
    if (_c == ' ' ||
        _c == '\t' ||
        _c == '\n'
        )
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
    (*_cursor) += tstr_length(&str);
    //printf("number lexed = %s\n", str.c_str);
    type = _is_symbol_real_or_decimal(&str);
    if (type == TYPE_REAL)
        return real(_env, atoi(str.c_str));
    return decimal(_env, atof(str.c_str));
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
void
expr_lex(Environment* _env, expr* _root, char* _program, int* _cursor)
{
    /*TODO: Does not support special syntax for:
    quote = '
    array = []
    */
    ERRCHECK_INV_ENV(_env);
    ERRCHECK(_program != NULL, "Invalid program given to lexer");
    ERRCHECK(_cursor != NULL, "Invalid cursor given to lexer");
    ERRCHECK(Buildins_len(&_env->buildins) > 0, "No buildin functions in environment!");
    expr* curr = _root; 
    expr* extracted;
    char* p;
    while (1) {
        p = _program + (*_cursor)++;
        if (*p == '\0') {
            return;
        }
        if (_is_whitespace(*p)) {
            continue;
        }
        else if (*p == ')') {
            return;
        }
        else if (*p ==  '(') {
            extracted = cons(_env, NIL, NIL);
            expr_lex(_env, extracted, _program, _cursor);
        }
        else if (_can_lex_string(p)) {
            extracted = _lex_string(_env, p, _cursor);
        }
        else if (_can_lex_number(p)) {
            extracted = _lex_number(_env, p, _cursor);
        }
        else {
          /*We determine that if it is not
            a string or a number, it must be a symbol*/
            extracted = _lex_symbol(_env, p, _cursor);
        }
        /*Insert extracted into expression*/
        curr->car = extracted;
        curr->cdr = cons(_env, NIL, NIL);
        curr = cdr(curr);
    }
    ERRCHECK_UNREACHABLE();
}

#if 0
expr*
expr_lex(Environment* _env, char* _program, int* _cursor)
{
    /*TODO: Does not support special syntax for:
    quote = '
    array = []
    */
    ERRCHECK_INV_ENV(_env);
    ERRCHECK(_program != NULL, "Invalid program given to lexer");
    ERRCHECK(_cursor != NULL, "Invalid cursor given to lexer");
    ERRCHECK(Buildins_len(&_env->buildins) > 0, "No buildin functions in environment!");
    expr* root = cons(_env, NIL, NIL);
    expr* curr = root; 
    expr* extracted;
    char* p;

    while (1) {
        p = _program + (*_cursor)++;
        if (*p == '\0') {
            return root;
        }
        if (_is_whitespace(*p)) {
            continue;
        }
        else if (*p == ')') {
            return root;
        }
        else if (*p ==  '(') {
            extracted = expr_lex(_env, _program, _cursor);
        }
        else if (_can_lex_string(p)) {
            extracted = _lex_string(_env, p, _cursor);
        }
        else if (_can_lex_number(p)) {
            extracted = _lex_number(_env, p, _cursor);
        }
        else {
          /*We determine that if it is not
            a string or a number, it must be a symbol*/
            extracted = _lex_symbol(_env, p, _cursor);
        }
        /*Insert extracted into expression*/
        curr->car = extracted;
        curr->cdr = cons(_env, NIL, NIL);
        curr = cdr(curr);

    }
    ERRCHECK_UNREACHABLE();
    return NIL;
}
#endif

expr*
str_read(Environment* _e, char* _p)
{
    /*TODO: expression must be encased in parens (), otherwise we need to throw
      error, insead of segfaulting down the line!
    */
    ERRCHECK_INV_ENV(_e);
    ERRCHECK(_p != NULL, "Given program is NULL");
    expr* inp = cons(_e,
                     string(_e, tstr_(_p)),
                     NIL
        );
    return buildin_read(_e, inp);
}

expr*
str_readp(Environment* _e, char* _p)
/*read and parse string visually*/
{
    printf("[char* input] > %s\n", _p);
    expr* out = str_read(_e, _p);
    printf("[lex'ed expr] > "); buildin_print(_e, out); printf("\n");
    return out;
}

#endif /*CORE_IMPLEMENTATION*/
#endif /*CORE_H*/
