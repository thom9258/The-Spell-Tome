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
- [0.0] Initialized library.
- [0.1] Created AST structure and imported base lib functionality.


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

#define CAR(s) ((s == NULL) ? NULL : (s)->car)
#define CDR(s) ((s == NULL) ? NULL : (s)->cdr)

Environment* Environment_new(Environment* _env);
void Environment_destroy(Environment* _env);

char is_nil(expr* _e);
expr* expr_new(Environment *_env);
expr* cons(Environment *_env, expr* _car, expr* _cdr);
expr* consN(Environment *_env);
expr* nil(Environment *_env);
expr* real(Environment *_env, int _v);
expr* symbol(Environment *_env, char* _v);
expr* string(Environment *_env, tstr _v);
expr* buildin(Environment *_env, buildin_fn _buildin, char* _symbol);

/******************************************************************************/
#define CORE_IMPLEMENTATION
#ifdef CORE_IMPLEMENTATION


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
    if (_e == NULL)
        return 1;
    if (_e->type == TYPE_NIL)
        return 1;
    return 0;
}

expr*
expr_new(Environment *_env)
{
    ERROR_INV_ENV(_env);
    expr* e = (expr*)sbAllocator_get(&_env->expr_allocator);
    ERROR_INV_ALLOC(e);
    e->type = TYPE_CCONS;
    e->car = NULL;
    e->cdr = NULL;
    return e;
}

expr*
cons(Environment *_env, expr* _car, expr* _cdr)
{
    ERROR_INV_ENV(_env);
    expr* e = expr_new(_env);
    e->car = _car;
    e->cdr = _cdr;
    return e;
}


expr* nil(Environment *_env)
{
    ERROR_INV_ENV(_env);
    expr* e = expr_new(_env);
    if (e == NULL) return e;
    e->type = TYPE_NIL;
    e->car = NULL;
    e->cdr = NULL;
    return e;
}

expr*
consN(Environment *_env)
{
    ERROR_INV_ENV(_env);
    return cons(_env, NULL, NULL);
}

expr*
real(Environment *_env, int _v)
{
    ERROR_INV_ENV(_env);
    expr* e = expr_new(_env);
    if (e == NULL) return NULL;
    e->type = TYPE_REAL;
    e->real = _v;
    return e;
}

expr*
symbol(Environment *_env, char* _v)
{
    ERROR_INV_ENV(_env);
    expr* e = expr_new(_env);
    if (e == NULL) return NULL;
    e->type = TYPE_SYMBOL;
    e->symbol = tstr_(_v);
    return e;
}

expr*
string(Environment *_env, tstr _v)
{
    ERROR_INV_ENV(_env);
    expr* e = expr_new(_env);
    if (e == NULL) return NULL;
    e->type = TYPE_STRING;
    e->string = _v;
    return e;
}

#if 0
expr*
buildin(Environment *_env, buildin_fn _fn, char* _name)
{
    ERROR_INV_ENV(_env);
    expr* e = expr_new(_env);
    if (e == NULL) return NULL;
    e->type = TYPE_BUILDIN;
    e->buildin.fn = _fn;
    e->buildin.name = tstr_(_name);
    return e;
}
#endif

void
print_value(Environment* _env, expr* _e)
{
    UNUSED(_env);
    if (is_nil(_e))
        return;

    switch (_e->type) {
    case TYPE_NIL:
        printf("<NIL>");
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
        printf("%s", _e->string.c_str);
        break;
    default:
        ERROR_UNREACHABLE();
    };
}

char
_is_number(char _c)
{
    if (_c > '0' && _c < '9')
        return 1;
    return 0;
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

expr*
expr_lex(Environment* _env, char* _program, int* _cursor)
/*TODO: Make more readable!*/
{
    ERROR_INV_ENV(_env);
    ERROR(_program != NULL, "Invalid program given to lexer");
    ERROR(_cursor != NULL, "Invalid cursor given to lexer");

    expr* root = consN(_env); 
    expr* curr = root; 

    char* p;
    while (*(p = _program + *_cursor) != '\0') {
        (*_cursor)++;
        if (_is_whitespace(*p)) {
            continue;
        }
        else if (*p ==  '(') {
            curr->cdr = consN(_env);
            curr->cdr->car = expr_lex(_env, _program, _cursor);
            
        }
        else if (*p == ')') {
            return root;
        }
        else if (_is_number(*p)) {
            /*TODO: Parse numbers into actual numbers!*/
            char* nump;
            int numlen = 0;
            nump = p;
            while (!_is_whitespace(*nump) && *nump != '(' && *nump != ')') {
                numlen++;
                nump++;
            }
            /*insert found symbol and create space for new ccons*/
            tstr num = tstr_n(p, numlen+1);
            curr->car = symbol(_env, num.c_str);
            tstr_destroy(&num);
            //printf("Created number ["); print_value(_env, curr->car); printf("]\n");
            curr->cdr = consN(_env);
            curr = curr->cdr;
            (*_cursor) += numlen;
        }
        else {
            char* symp;
            int symlen = 0;
            symp = p;
            /*get length of symbol*/
            while (!_is_whitespace(*symp) && *symp != '(' && *symp != ')') {
                symlen++;
                symp++;
            }
            /*insert found symbol and create space for new ccons*/
            tstr sym = tstr_n(p, symlen+1);
            curr->car = symbol(_env, sym.c_str);
            tstr_destroy(&sym);
            //printf("Created symbol ["); print_value(_env, curr->car); printf("]\n");
            curr->cdr = consN(_env);
            curr = curr->cdr;
            (*_cursor) += symlen;
        }
    }
    ERROR_UNREACHABLE();
    return root;
}

#endif /*CORE_IMPLEMENTATION*/
#endif /*CORE_H*/
