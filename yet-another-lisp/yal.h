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
- [0.1] Created AST structure and imported base lib functionality.
- [0.0] Initialized library.
*/

/*
https://buildyourownlisp.com/chapter9_s_expressions#lists_and_lisps
http://www.ulisp.com/show?1BLW
*/

/*
  TODO: The stringifyexpr in error returns are the ones doing memory leaks, fix
  them by not directly returning string with result stringifyexpr.
  It would be possible if stringifyexpr is not used but by creating a variable
  type called error, then it would remove itself on env_destroy()
*/

/*TODO: Seems to segfault if using a buildinsymbol we cant find*/

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

#define WHILE_NOT_NIL(expr) while (!is_nil(cdr(expr)))

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

enum EXCEPTYPE {
    EXCEPTYPE_OK = 0,
    EXCEPTYPE_ERROR,

    EXCEPTYPE_COUNT
};

const char* EXCEPTYPE_TO_STR[EXCEPTYPE_COUNT] = {
    "EXCEPTYPE_OK",
    "EXCEPTYPE_ERROR"
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
    VariableScope global;
    VariableScopes active_scopes;
};

/* =========================================================
 * Safety and Assertion on error
 * */
#define UNUSED(x) (void)(x)
#define UNIMPLEMENTED(fun) assert(0 && "UNIMPLEMENTED: " && fun)

#define THROW_ARGEVALFAIL(msgdst, args)             \
    _throw_argevalfail((char*)__FUNCTION__, msgdst, args)
#define THROW_INVALIDARGS(expected, msgdst, args)               \
    _throw_invalidargs((char*)__FUNCTION__, expected, msgdst, args)
#define THROW_NONVALUEARG(msgdst, args)               \
    _throw_nonvaluearg((char*)__FUNCTION__, msgdst, args)

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
char Exception_is_error(Exception* _dst);
void Exception_destroy(Exception* _msg);

/*Environment Management*/
Environment* Env_new(Environment* _env);
void Env_destroy(Environment* _env);
void Env_add_buildin(Environment* _env, char* _name, buildin_fn _fn);
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
tstr stringify(expr* _args, char* _open, char* _close);

/*Value creation*/
expr* cons(Environment* _env, expr* _car, expr* _cdr);
expr* real(Environment* _env, int _v);
expr* decimal(Environment* _env, float _v);
expr* symbol(Environment* _env, char* _v);
expr* string(Environment* _env, char* _v);
void variable_delete(Environment* _env, expr* _atom);

/*Core*/
expr* read(Environment* _env, VariableScope* _scope, Exception* _throwdst, char* _program_str);
expr* eval(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
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
expr* buildin_range(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_plus(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_minus(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_multiply(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_divide(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_mathequal(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_defglobal(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);
expr* buildin_mathequal(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in);


/******************************************************************************/
#define YAL_IMPLEMENTATION
#ifdef YAL_IMPLEMENTATION

void
_throw_error(Exception* _dst, tstr _msg)
{
     if (_dst == NULL)
        return;
    tstr_destroy(&_dst->msg);
    _dst->type = EXCEPTYPE_ERROR;
    _dst->msg = _msg;
}

char
Exception_is_error(Exception* _dst)
{
     if (_dst == NULL)
        return 0;
     return (_dst->type == EXCEPTYPE_ERROR) ? 1:0;
}

void
_throw_argevalfail(char* _fnstr, Exception* _throwdst, expr* args)
{
    WITH_STRINGEXPR(str, args,
                    _throw_error(_throwdst,
                               tstr_fmt("'%s' could not evaluate arguments %s\n",
                                        _fnstr, str.c_str)););
}

void
_throw_invalidargs(char* _fnstr, char* _expected, Exception* _throwdst, expr* args)
{
    WITH_STRINGEXPR(str, args,
                    _throw_error(_throwdst,
                               tstr_fmt("'%s' %s %s\n",
                                        _fnstr, _expected, str.c_str)););
}

void
_throw_nonvaluearg(char* _fnstr, Exception* _throwdst, expr* args)
{
    WITH_STRINGEXPR(str, args,
                    _throw_error(_throwdst,
                               tstr_fmt("'%s' got non-value arguments in %s\n",
                                        _fnstr, str.c_str)););
}

void
Exception_destroy(Exception* _dst)
{
     if (_dst == NULL)
        return;
     tstr_destroy(&_dst->msg);
     _dst->type = EXCEPTYPE_OK;
}

Environment*
Env_new(Environment* _env)
{
    *_env = (Environment){0};
    /*TODO: 30 is a arbitrary number, change when you know size of buildins*/
    sbAllocator_init(&_env->variable_allocator, sizeof(expr));
    Buildins_initn(&_env->buildins, 30);
    VariableScope_new(_env, &_env->global, NULL);
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
    VariableScope_destroy(_env, &_env->global);
    n = VariableScopes_len(&_env->active_scopes);
    for (i = 0; i < n; i++)
        VariableScope_destroy(_env, VariableScopes_peek(&_env->active_scopes, i));

    sbAllocator_destroy(&_env->variable_allocator);
}

VariableScope*
VariableScope_new(Environment* _env, VariableScope* _this, VariableScope* _outer)
{
    VariableScope_destroy(_env, _this);
    Variables_initn(&_this->variables, 3);
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
    /*http://clhs.lisp.se/Body/f_list_l.htm*/
    if (is_nil(_args)) return 0;
    if (is_val(_args)) return 1;
    int cnt = 0;
    expr* tmp = _args;
    WHILE_NOT_NIL(tmp) {
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
    case TYPE_ARRAY:
        dst  = stringify(_arg, "[", "]");
        break;
    case TYPE_DICTIONARY:
        dst = stringify(_arg, "#{", "}");
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
        dst = tstr_fmt("\"%s\"", _arg->string.c_str);
        break;
    default:
        ASSERT_UNREACHABLE();
    };
    return dst;
}

tstr
stringify(expr* _args, char* _open, char* _close)
{
    tstr dst = {0};
    expr* curr = _args;
    tstr currstr = {0};
    tstr open = tstr_view(_open);
    tstr close = tstr_view(_close);
    tstr space = tstr_view(" ");

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

char
_is_literal_token(char _t)    
{
    char ltokens[] = {'(', ')', '\'','\'', '\"', '[', ']'};
    int ltoken_len = sizeof(ltokens) / sizeof(ltokens[0]);
    int i;
    for (i = 0; i < ltoken_len; i++)
        if (ltokens[i] == _t)
            return _t;
    return 0;
}

#define _IS_WHITESPACE(C) \
    (((C) == ' ' || (C) == '\t' || (C) == '\n') ? 1 : 0)

tstr
_get_next_token(char* _source, int* _cursor)
{
    tstr token = {0};
    int len = 0;
    if (_is_literal_token(_source[*_cursor])) {
        len = 1;
    } else {
        while (!_IS_WHITESPACE(_source[*_cursor + len]) &&
               !_is_literal_token(_source[*_cursor + len]))
            len++;
    }
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
_tokens_post_process(Tokens* _tokens)
{
  /*TODO: Here we remove the outer scope of the lexed function.
    if we lex more than one scope we cant do this..
   */
    Tokens_pop(_tokens);
    Tokens_pop_front(_tokens);
}

char
_looks_like_number(tstr _token)
{
    if (tstr_length(&_token) > 1 && (_token.c_str[0] == '-' || _token.c_str[0] == '+'))
        return (_token.c_str[1] > '0' && (_token.c_str[1]) < '9') ? 1 : 0;
    return (_token.c_str[0] > '0' && (_token.c_str[0]) < '9') ? 1 : 0;
}

expr*
_lex(Environment* _env, Tokens* _tokens)
{
    /*TODO: Does not support special syntax for:
    quote = '
    array = []
    dotted lists (a . 34)
    */
    ASSERT_INV_ENV(_env);
    assert(_tokens != NULL);
    tstr token;
    int i = 0;
    expr* program = cons(_env, NULL, NULL);
    expr* curr = program;

    while (Tokens_len(_tokens) > 0) {
        token = Tokens_pop_front(_tokens);
        i++;
        if (tstr_equalc(&token, ")")) {
            break;
        }
        else if (tstr_equalc(&token, "(")) {
            curr->car = _lex(_env, _tokens);
        }
        else if (tstr_equalc(&token, "\"")) {
            curr->car = symbol(_env, token.c_str);
        }
        else if (_looks_like_number(token)) {
            if (_token_value_type(&token) == TYPE_REAL)
                curr->car = real(_env, tstr_to_int(&token));
            else
                curr->car = decimal(_env, tstr_to_float(&token));
        }
        else {
            curr->car = symbol(_env, token.c_str);
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
    //printf("Amount of programs in 'read' = %d\n", len(program));
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
    if (is_nil(_sym) || !is_symbol(_sym))
        return NULL;
    printf("looking for variable '%s'\n", _sym->symbol.c_str);

    for (i = 0; i < Variables_len(&_scope->variables); i++) {
        variable = Variables_peek(&_scope->variables, i);
        if (tstr_equal(&variable->symbol, &_sym->symbol)) {
            printf("found '%s'\n", variable->symbol.c_str);
            return variable;
        }
    }
    if (_scope->outer != NULL)
        return _find_variable(_scope->outer, _sym);
    return NULL;
}

expr*
eval(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* fn = NULL;
    expr* args = NULL;
    Buildin* buildin = NULL;
    Variable* variable = NULL;
    expr* result = NULL;
    ASSERT_INV_ENV(_env);

    if (is_nil(_in))
        return _in;
    switch (_in->type) {
    case TYPE_CONS:
        fn = car(_in);
        args = cdr(_in);
        DBPRINT("(eval) INPUT: ", _in);
        DBPRINT("(eval) NAME: ", fn);
        DBPRINT("(eval) ARGS: ", args);
        if (is_cons(fn) || fn->type != TYPE_SYMBOL) {
            THROW_INVALIDARGS("expected callable function", _throwdst, _in);
            return NIL();
        }
        buildin = _find_buildin(_env, fn);
        if (buildin != NULL) {
            result = buildin->fn(_env, _scope, _throwdst, args);
            printf("buildin [%s]\n", buildin->name.c_str);
            DBPRINT("(eval) RESULT: ", result);
            return result;
        } 
        return NIL();

    case TYPE_SYMBOL:
        DBPRINT("TRYING TO ACCESS VAR: ", _in);
        //variable = _find_variable(_scope, fn);
        if (variable != NULL) {
            return variable->value;
        }
        return NIL();

    case TYPE_REAL:
    case TYPE_DECIMAL:
    case TYPE_STRING:
    default:
        return _in;
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
        THROW_INVALIDARGS("expected string input", _throwdst, _in);
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
    DBPRINT("input to buildin list: ", _in);
    if (!is_cons(_in)) {
        THROW_INVALIDARGS("expected list input", _throwdst, _in);
        return NIL();
    }
    while (!is_nil(cdr(_in))) {
        curr->car = eval(_env, _scope, _throwdst, car(_in));
        if (Exception_is_error(_throwdst))
            return NIL();
        DBPRINT("list evalled: ", car(_in));
        DBPRINT("to ", car(curr));
        curr->cdr = cons(_env, NULL, NULL);
        curr = cdr(curr);
        _in = cdr(_in);
    }
    return root;
}

expr*
buildin_len(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* args;
    ASSERT_INV_SCOPE(_scope);
    args = list(_env, _scope, _throwdst, _in);
    if (Exception_is_error(_throwdst)) {
        THROW_ARGEVALFAIL(_throwdst, _in);
        return NIL();
    }
    if (len(args) != 1) {
        THROW_INVALIDARGS("Expected only 1 input", _throwdst, _in);
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
    if (Exception_is_error(_throwdst)) {
        THROW_ARGEVALFAIL(_throwdst, _in);
        return NIL();
    }
    if (len(args) != 1) {
        THROW_INVALIDARGS("Expected only 1 input", _throwdst, _in);
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
    if (Exception_is_error(_throwdst)) {
        THROW_ARGEVALFAIL(_throwdst, _in);
        return NIL();
    }
    if (len(args) != 1) {
        THROW_INVALIDARGS("Expected only 1 input", _throwdst, _in);
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
    args = list(_env, _scope, _throwdst, _in);
    if (Exception_is_error(_throwdst)) {
        THROW_ARGEVALFAIL(_throwdst, _in);
        return NIL();
    }
    if (len(args) != 2) {
        THROW_INVALIDARGS("Expected 2 inputs", _throwdst, _in);
        return NIL();
    }
    DBPRINT("cons args: ", args);
    return cons(_env, first(args), second(args));
}

expr*
buildin_first(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* args;
    ASSERT_INV_SCOPE(_scope);
    args = list(_env, _scope, _throwdst, _in);
    if (Exception_is_error(_throwdst)) {
        THROW_ARGEVALFAIL(_throwdst, _in);
        return NIL();
    }
    if (len(args) != 1) {
        THROW_INVALIDARGS("Expected only 1 input", _throwdst, _in);
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
    if (Exception_is_error(_throwdst)) {
        THROW_ARGEVALFAIL(_throwdst, _in);
        return NIL();
    }
    if (len(args) != 1) {
        THROW_INVALIDARGS("Expected only 1 input", _throwdst, _in);
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
    if (Exception_is_error(_throwdst)) {
        THROW_ARGEVALFAIL(_throwdst, _in);
        return NIL();
    }
    if (len(args) != 1) {
        THROW_INVALIDARGS("Expected only 1 input", _throwdst, _in);
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
    if (Exception_is_error(_throwdst)) {
        THROW_ARGEVALFAIL(_throwdst, _in);
        return NIL();
    }
    if (len(args) != 1) {
        THROW_INVALIDARGS("Expected only 1 input", _throwdst, _in);
        return NIL();
    }
    args = first(args);
    return fourth(args);
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

    DBPRINT("'range' input: ", _in);
    args = list(_env, _scope, _throwdst, _in);
    DBPRINT("'range' args: ", args);

    if (Exception_is_error(_throwdst)) {
        THROW_ARGEVALFAIL(_throwdst, _in);
        return NIL();
    }
    if (len(args) != 2) {
        THROW_INVALIDARGS("expected 2 inputs", _throwdst, args);
        return NIL();
    }
    start = first(args);
    end = second(args);
    if (start->type != TYPE_REAL || end->type != TYPE_REAL) {
        THROW_INVALIDARGS("expected inputs of type REAL", _throwdst, args);
        return NIL();
    }
    if (start->real > end->real) {
        THROW_INVALIDARGS("expected input 1 to be smaller than input 2", _throwdst, args);
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
        THROW_INVALIDARGS("expected inputs to be values", _throwdst, cons(_env, _a, _b));
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
    if (Exception_is_error(_throwdst)) {
        THROW_ARGEVALFAIL(_throwdst, args);
        return NIL();
    }
    if (len(args) == 0) {
        return real(_env, 0);
    }
    /*Check for non value inputs*/
    tmp = args;
    while (!is_nil(cdr(tmp))) {
        if (!is_val(car(tmp))) {
            THROW_NONVALUEARG(_throwdst, args);
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
        THROW_INVALIDARGS("expected inputs to be values", _throwdst, cons(_env, _a, _b));
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
    if (Exception_is_error(_throwdst)) {
        THROW_ARGEVALFAIL(_throwdst, args);
        return NIL();
    }
    /*Check for non value inputs*/
    if (len(args) == 0) {
        return real(_env, 0);
    }
    tmp = args;
    while (!is_nil(cdr(tmp))) {
        if (!is_val(car(tmp))) {
            THROW_NONVALUEARG(_throwdst, args);
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
        THROW_INVALIDARGS("expected inputs to be values", _throwdst, cons(_env, _a, _b));
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
    if (Exception_is_error(_throwdst)) {
        THROW_ARGEVALFAIL(_throwdst, args);
        return NIL();
    }
    /*Check for non value inputs*/
    if (len(args) == 0) {
        return real(_env, 1);
    }
    tmp = args;
    while (!is_nil(cdr(tmp))) {
        if (!is_val(car(tmp))) {
            THROW_NONVALUEARG(_throwdst, args);
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
        THROW_INVALIDARGS("expected inputs to be values", _throwdst, cons(_env, _a, _b));
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
    if (Exception_is_error(_throwdst)) {
        THROW_ARGEVALFAIL(_throwdst, args);
        return NIL();
    }
    if (len(args) == 0) {
        return real(_env, 1);
    }
    /*Check for non value inputs*/
    tmp = args;
    while (!is_nil(cdr(tmp))) {
        if (!is_val(car(tmp))) {
            THROW_NONVALUEARG(_throwdst, args);
            return NIL();
        }
        if ((tmp->type == TYPE_REAL && tmp->real == 0) ||
            (tmp->type == TYPE_DECIMAL && tmp->decimal == 0.f)) {
            THROW_INVALIDARGS("division by zero!", _throwdst, args);
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
    if (!is_val(_a) || !is_val(_b)) {
        THROW_INVALIDARGS("expected inputs to be values", _throwdst, cons(_env, _a, _b));
        return 0;
    }
    if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
        return _a->decimal == _b->decimal;
    else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
        return _a->real == _b->decimal;
    else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
        return  _a->decimal == _b->real;
    return _a->real == _b->real;
}

expr*
buildin_mathequal(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    expr* args;
    expr* oldtmp;
    expr* tmp;
    ASSERT_INV_SCOPE(_scope);
    args = list(_env, _scope, _throwdst, _in);
    if (Exception_is_error(_throwdst)) {
        THROW_ARGEVALFAIL(_throwdst, args);
        return symbol(_env, "NIL");
    }
    if (len(args) < 2) {
        return symbol(_env, "t");
    }
    /*Check for non value inputs*/
    tmp = args;
    while (!is_nil(cdr(tmp))) {
        if (!is_val(car(tmp))) {
            THROW_NONVALUEARG(_throwdst, args);
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
buildin_defglobal(Environment* _env, VariableScope* _scope, Exception* _throwdst, expr* _in)
{
    ASSERT_INV_SCOPE(_scope);
    Variable v = {0};
    expr* args;
    expr* sym;
    expr* val;

    args = list(_env, _scope, _throwdst, _in);
    if (Exception_is_error(_throwdst)) {
        THROW_ARGEVALFAIL(_throwdst, args);
        return NIL();
    }
    if (len(args) > 2) {
        THROW_INVALIDARGS("expected 1-2 arguments", _throwdst, args);
        return NIL();
    }
    sym = first(args);
    val = second(args);
    if (!is_symbol(sym)) {
        THROW_INVALIDARGS("expected first argument to be symbol", _throwdst, sym);
        return NIL();
    }
    v.symbol = tstr_(first(args)->symbol.c_str);
    v.value = variable_duplicate(_env, val);
    Variables_push(&_env->global.variables, v);
    DBPRINT("created global: ", sym);
    DBPRINT("with value: ", val);
    return sym;
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
    Env_add_buildin(_env, "eval", eval);
    Env_add_buildin(_env, "range", buildin_range);
    Env_add_buildin(_env, "quote", buildin_quote);
    Env_add_buildin(_env, "list", buildin_list);
    Env_add_buildin(_env, "cons", buildin_cons);
    Env_add_buildin(_env, "len", buildin_len);
    //Env_add_buildin(_env, "put", buildin_put);
    //Env_add_buildin(_env, "pop", buildin_pop);

    /*Accessors*/
    Env_add_buildin(_env, "car", buildin_car);
    Env_add_buildin(_env, "head", buildin_car);
    Env_add_buildin(_env, "cdr", buildin_cdr);
    Env_add_buildin(_env, "tail", buildin_cdr);
    Env_add_buildin(_env, "rest", buildin_cdr);
    Env_add_buildin(_env, "first", buildin_first);
    Env_add_buildin(_env, "second", buildin_second);
    Env_add_buildin(_env, "third", buildin_third);
    Env_add_buildin(_env, "fourth", buildin_fourth);

    /*Arimetrics*/
    Env_add_buildin(_env, "+", buildin_plus);
    Env_add_buildin(_env, "-", buildin_minus);
    Env_add_buildin(_env, "*", buildin_multiply);
    Env_add_buildin(_env, "/", buildin_divide);
    Env_add_buildin(_env, "=", buildin_mathequal);

    /*Variable management*/
    Env_add_buildin(_env, "global", buildin_defglobal);
    //Env_add_buildin(_env, "set", buildin_set);
    //Env_add_buildin(_env, "const", buildin_defconst);

    /*Condition management*/
    //Env_add_buildin(_env, "cond", buildin_cond);
    //Env_add_buildin(_env, "if", buildin_if);

    /*Function management*/
    //Env_add_buildin(_env, "fn", buildin_defn);
    //Env_add_buildin(_env, "macro", buildin_macro);
    //Env_add_buildin(_env, "lambda", buildin_lambda);
}

#endif /*YAL_IMPLEMENTATION*/
#endif /*YAL_H*/
