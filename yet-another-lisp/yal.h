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

enum MSGTYPE {
    MSGTYPE_OK = 0,
    MSGTYPE_ERROR,

    MSGTYPE_COUNT
};

const char* MSGTYPE_TO_STR[MSGTYPE_COUNT] = {
    "MSGTYPE_OK",
    "MSGTYPE_ERROR"
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
}yal_Msg;

typedef struct {
    char type;
    tstr symbol;
    expr* value;
}Variable;

#define t_type Variable
#define arr_t_name Variables
#include "vendor/tsarray.h"

/*Forward declarations*/
typedef struct Environment Environment;
typedef struct VariableScope VariableScope;
typedef expr*(*buildin_fn)(VariableScope*, yal_Msg*, expr*);

struct VariableScope {
    Environment* env;
    VariableScope* outer;
    Variables variables;
};

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
};

/* =========================================================
 * Safety and Assertion on error
 * */
#define UNUSED(x) (void)(x)
#define UNIMPLEMENTED(fun) assert(0 && "UNIMPLEMENTED: " && fun)

#define ERROR_ARGEVALFAIL(msgdst, args)             \
    _error_argevalfail((char*)__FUNCTION__, msgdst, args)
#define ERROR_INVALIDARGS(expected, msgdst, args)               \
    _error_invalidargs((char*)__FUNCTION__, expected, msgdst, args)
#define ERROR_NONVALUEARG(msgdst, args)               \
    _error_nonvaluearg((char*)__FUNCTION__, msgdst, args)

#define ASSERT_UNREACHABLE() \
    assert(0 && "Fatal Error occoured, reached unreachable code!")

#define ASSERT_NULL(ptr, infostr)                                               \
    assert(ptr != NULL && "Invalid " infostr " is used in code! Ptr is NULL!")

#define ASSERT_INV_SCOPE(scope) \
    ASSERT_NULL(scope, "VariableScope*")

#define ASSERT_INV_ENV(scope) \
    ASSERT_NULL(scope, "Environment*")

#define ASSERT_NOMOREMEMORY() \
    assert(0 && "Was unable to allocate more memory!")

#define RESULT_NOT_OK(RESULT) (((RESULT).type != RESTYPE_OK) ? 1 : 0)

/*Error Management*/
char yal_is_error(yal_Msg* _dst);
void yal_msg_destroy(yal_Msg* _msg);

Environment* Env_new(Environment* _env);
void Env_destroy(Environment* _env);

VariableScope* VariableScope_new(VariableScope* _this, VariableScope* _outer, Environment* _env);
void VariableScope_destroy(VariableScope* _scope);

/*Checks*/
expr* NIL(void);
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

/*Value creation*/
expr* cons(VariableScope *_env, expr* _car, expr* _cdr);
expr* real(VariableScope *_env, int _v);
expr* decimal(VariableScope *_env, float _v);
expr* symbol(VariableScope *_env, char* _v);
expr* string(VariableScope *_env, char* _v);
void variable_delete(VariableScope *_scope, expr* _atom);

/*Printing*/
void printexpr(expr* _args);
tstr stringify(expr* _args, char* _open, char* _close);

/*Core*/
expr* read(VariableScope* _scope, yal_Msg* _msgdst, char* _program_str);
expr* eval(VariableScope* _scope, yal_Msg* _msgdst, expr* _in);
expr* list(VariableScope* _scope, yal_Msg* _msgdst, expr* _in);

/******************************************************************************/
#define YAL_IMPLEMENTATION
#ifdef YAL_IMPLEMENTATION

void
_yal_error(yal_Msg* _dst, tstr _msg)
{
     if (_dst == NULL)
        return;
    tstr_destroy(&_dst->msg);
    _dst->type = MSGTYPE_ERROR;
    _dst->msg = _msg;
}

char
yal_is_error(yal_Msg* _dst)
{
     if (_dst == NULL)
        return 0;
     return (_dst->type == MSGTYPE_ERROR) ? 1:0;
}

void
_error_argevalfail(char* _fnstr, yal_Msg* _msgdst, expr* args)
{
    WITH_STRINGEXPR(str, args,
                    _yal_error(_msgdst,
                               tstr_fmt("'%s' could not evaluate arguments %s\n",
                                        _fnstr, str.c_str)););
}

void
_error_invalidargs(char* _fnstr, char* _expected, yal_Msg* _msgdst, expr* args)
{
    WITH_STRINGEXPR(str, args,
                    _yal_error(_msgdst,
                               tstr_fmt("'%s' %s %s\n",
                                        _fnstr, _expected, str.c_str)););
}

void
_error_nonvaluearg(char* _fnstr, yal_Msg* _msgdst, expr* args)
{
    WITH_STRINGEXPR(str, args,
                    _yal_error(_msgdst,
                               tstr_fmt("'%s' got non-value arguments in %s\n",
                                        _fnstr, str.c_str)););
}

void
yal_msg_destroy(yal_Msg* _dst)
{
     if (_dst == NULL)
        return;
     tstr_destroy(&_dst->msg);
     _dst->type = MSGTYPE_OK;
}

Environment*
Env_new(Environment* _env)
{
    *_env = (Environment){0};
    /*TODO: 30 is a arbitrary number, change when you know size of buildins*/
    sbAllocator_init(&_env->variable_allocator, sizeof(expr));
    Buildins_initn(&_env->buildins, 30);
    VariableScope_new(&_env->global, NULL, _env);
    return _env;
}

void
Env_destroy(Environment* _env)
{
    if (_env == NULL)
        return;
    Buildins_destroy(&_env->buildins);
    sbAllocator_destroy(&_env->variable_allocator);
    VariableScope_destroy(&_env->global);
}

VariableScope*
VariableScope_new(VariableScope* _this, VariableScope* _outer, Environment* _env)
{
    VariableScope_destroy(_this);
    Variables_initn(&_this->variables, 3);
    _this->env = _env;
    _this->outer = _outer;
    return _this;
}

void
VariableScope_destroy(VariableScope* _scope)
{
    int i;
    Variable tmp;
    for (i = 0; i < Variables_len(&_scope->variables); i++) {
        tmp = Variables_get(&_scope->variables, i);
        variable_delete(_scope, tmp.value);
    }
    Variables_destroy(&_scope->variables);
    _scope->env = NULL;
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
_variable_new(VariableScope *_scope)
{
    ASSERT_INV_SCOPE(_scope);
    expr* e = (expr*)sbAllocator_get(&_scope->env->variable_allocator);
    if (e == NULL)
        ASSERT_NOMOREMEMORY();
    return e;
}

expr*
variable_duplicate(VariableScope *_scope, expr* _atom)
{
    ASSERT_INV_SCOPE(_scope);
    if (is_nil(_atom))
        return NIL();
    switch (_atom->type) {
    case TYPE_SYMBOL:
        return symbol(_scope, _atom->symbol.c_str);
    case TYPE_STRING:
        return string(_scope, _atom->string.c_str);
    case TYPE_REAL:
        return real(_scope, _atom->real);
    case TYPE_DECIMAL:
        return decimal(_scope, _atom->decimal);
    case TYPE_CONS:
    default:
        ASSERT_UNREACHABLE();
    };
}

void
variable_delete(VariableScope *_scope, expr* _atom)
{
    ASSERT_INV_SCOPE(_scope);
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
    sbAllocator_return(&_scope->env->variable_allocator, _atom);
}

expr*
cons(VariableScope *_scope, expr* _car, expr* _cdr)
{
    ASSERT_INV_SCOPE(_scope);
    expr* e = _variable_new(_scope);
    e->type = TYPE_CONS;
    e->car = _car;
    e->cdr = _cdr;
    return e;
}

expr*
real(VariableScope *_scope, int _v)
{
    ASSERT_INV_SCOPE(_scope);
    expr* e = _variable_new(_scope);
    e->type = TYPE_REAL;
    e->real = _v;
    return e;
}

expr*
decimal(VariableScope *_scope, float _v)
{
    ASSERT_INV_SCOPE(_scope);
    expr* e = _variable_new(_scope);
    e->type = TYPE_DECIMAL;
    e->decimal = _v;
    return e;
}

expr*
symbol(VariableScope *_scope, char* _v)
{
    ASSERT_INV_SCOPE(_scope);
    expr* e = _variable_new(_scope);
    e->type = TYPE_SYMBOL;
    e->symbol = tstr_(_v);
    return e;
}

expr*
string(VariableScope *_scope, char* _v)
{
    ASSERT_INV_SCOPE(_scope);
    expr* e = _variable_new(_scope);
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
_lex(VariableScope* _scope, Tokens* _tokens)
{
    /*TODO: Does not support special syntax for:
    quote = '
    array = []
    dotted lists (a . 34)
    */
    ASSERT_INV_SCOPE(_scope);
    assert(_tokens != NULL);
    tstr token;
    int i = 0;
    expr* program = cons(_scope, NULL, NULL);
    expr* curr = program;

    while (Tokens_len(_tokens) > 0) {
        token = Tokens_pop_front(_tokens);
        i++;
        if (tstr_equalc(&token, ")")) {
            break;
        }
        else if (tstr_equalc(&token, "(")) {
            curr->car = _lex(_scope, _tokens);
        }
        else if (tstr_equalc(&token, "\"")) {
            curr->car = symbol(_scope, token.c_str);
        }
        else if (_looks_like_number(token)) {
            if (_token_value_type(&token) == TYPE_REAL)
                curr->car = real(_scope, tstr_to_int(&token));
            else
                curr->car = decimal(_scope, tstr_to_float(&token));
        }
        else {
            curr->car = symbol(_scope, token.c_str);
        }
        /*Insert extracted into lexed program*/
        tstr_destroy(&token);
        curr->cdr = cons(_scope, NULL, NULL);
        curr = curr->cdr;
    }
    //DBPRINT("lexed program: ", program);
    return program;
}

expr*
read(VariableScope* _scope, yal_Msg* _msgdst, char* _program_str)
{
    ASSERT_INV_SCOPE(_scope);
    assert(_program_str != NULL && "Given program str is NULL");
    /*TODO: Need error checking from _lex()*/
    UNUSED(_msgdst);
    int cursor = 0;
    Tokens tokens = _tokenize(_program_str, &cursor);
    _tokens_post_process(&tokens);
    expr* program = _lex(_scope, &tokens);
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

expr*
eval(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    expr* fn;
    expr* args;
    Buildin* buildin;
    expr* result;
    ASSERT_INV_SCOPE(_scope);

    if (is_nil(_in))
        return _in;

    //DBPRINT("(eval) INP:  ", _in);
    switch (_in->type) {
    case TYPE_CONS:
        fn = car(_in);
        args = cdr(_in);
        //DBPRINT("(eval) NAME: ", fn);
        //DBPRINT("(eval) ARGS: ", args);
        if (is_cons(fn) || fn->type != TYPE_SYMBOL) {
            ERROR_INVALIDARGS("expected callable function", _msgdst, _in);
            return NIL();
        }
        buildin = _find_buildin(_scope->env, fn);
        if (buildin == NULL) {
            ERROR_INVALIDARGS("Could not find function in environment", _msgdst, fn);
            return NIL();
        }
        result = buildin->fn(_scope, _msgdst, args);
        break;
    case TYPE_SYMBOL:
        /*TODO: Check symbol table and return value of symbol*/
        return _in;
        break;
    default:
        return _in;
        break;
    };
    return result;
}

expr* buildin_list(VariableScope* _scope, yal_Msg* _msgdst, expr* _in);
expr*
list(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    return buildin_list(_scope, _msgdst, _in);
}

/*********************************************************************/
/* Buildin Functions */
/*********************************************************************/

expr*
buildin_read(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    if (!is_string(_in)) {
        ERROR_INVALIDARGS("expected string input", _msgdst, _in);
        return NIL();
    }
    return read(_scope, _msgdst, _in->string.c_str);
}

expr*
buildin_list(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    /*list() is a list evaller that expects a list of eval-able lists */
    expr* root = cons(_scope, NULL, NULL);
    expr* curr = root;
    ASSERT_INV_SCOPE(_scope);
    //DBPRINT("input to buildin list: ", _in);
    if (!is_cons(_in)) {
        ERROR_INVALIDARGS("expected list input", _msgdst, _in);
        return NIL();
    }

    //while (!is_nil(_in)) {
    WHILE_NOT_NIL(_in) {
        curr->car = eval(_scope, _msgdst, car(_in));
        if (yal_is_error(_msgdst))
            return NIL();
        //DBPRINT("list evalled: ", car(_in));
        //DBPRINT("to ", car(curr));
        curr->cdr = cons(_scope, NULL, NULL);
        curr = cdr(curr);
        _in = cdr(_in);
    }
    return root;
}

expr*
buildin_quote(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    ASSERT_INV_SCOPE(_scope);
    UNUSED(_msgdst);
    return car(_in);
}

expr*
buildin_car(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    expr* args;
    ASSERT_INV_SCOPE(_scope);
    args = list(_scope, _msgdst, _in);
    if (yal_is_error(_msgdst)) {
        ERROR_ARGEVALFAIL(_msgdst, _in);
        return NIL();
    }
    if (len(args) != 1) {
        ERROR_INVALIDARGS("Expected only 1 input", _msgdst, _in);
        return NIL();
    }
    args = first(args);
    return car(args);
}

expr*
buildin_cdr(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    expr* args;
    ASSERT_INV_SCOPE(_scope);
    args = list(_scope, _msgdst, _in);
    if (yal_is_error(_msgdst)) {
        ERROR_ARGEVALFAIL(_msgdst, _in);
        return NIL();
    }
    if (len(args) != 1) {
        ERROR_INVALIDARGS("Expected only 1 input", _msgdst, _in);
        return NIL();
    }
    args = first(args);
    return cdr(args);
}

expr*
buildin_cons(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    expr* args;
    ASSERT_INV_SCOPE(_scope);
    args = list(_scope, _msgdst, _in);
    if (yal_is_error(_msgdst)) {
        ERROR_ARGEVALFAIL(_msgdst, _in);
        return NIL();
    }
    if (len(args) != 2) {
        ERROR_INVALIDARGS("Expected 2 inputs", _msgdst, _in);
        return NIL();
    }

    return cons(_scope, first(args), second(args));
}

expr*
buildin_first(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    expr* args;
    ASSERT_INV_SCOPE(_scope);
    args = list(_scope, _msgdst, _in);
    if (yal_is_error(_msgdst)) {
        ERROR_ARGEVALFAIL(_msgdst, _in);
        return NIL();
    }
    if (len(args) != 1) {
        ERROR_INVALIDARGS("Expected only 1 input", _msgdst, _in);
        return NIL();
    }
    args = first(args);
    return first(args);
}

expr*
buildin_second(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    expr* args;
    ASSERT_INV_SCOPE(_scope);
    args = list(_scope, _msgdst, _in);
    if (yal_is_error(_msgdst)) {
        ERROR_ARGEVALFAIL(_msgdst, _in);
        return NIL();
    }
    if (len(args) != 1) {
        ERROR_INVALIDARGS("Expected only 1 input", _msgdst, _in);
        return NIL();
    }
    args = first(args);
    return second(args);
}

expr*
buildin_third(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    expr* args;
    ASSERT_INV_SCOPE(_scope);
    args = list(_scope, _msgdst, _in);
    if (yal_is_error(_msgdst)) {
        ERROR_ARGEVALFAIL(_msgdst, _in);
        return NIL();
    }
    if (len(args) != 1) {
        ERROR_INVALIDARGS("Expected only 1 input", _msgdst, _in);
        return NIL();
    }
    args = first(args);
    return third(args);
}

expr*
buildin_fourth(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    expr* args;
    ASSERT_INV_SCOPE(_scope);
    args = list(_scope, _msgdst, _in);
    if (yal_is_error(_msgdst)) {
        ERROR_ARGEVALFAIL(_msgdst, _in);
        return NIL();
    }
    if (len(args) != 1) {
        ERROR_INVALIDARGS("Expected only 1 input", _msgdst, _in);
        return NIL();
    }
    args = first(args);
    return fourth(args);
}

expr*
buildin_range(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    expr* args;
    expr* start;
    expr* end;
    expr* range;
    expr* curr;
    int i;

    ASSERT_INV_SCOPE(_scope);

    DBPRINT("'range' input: ", _in);
    args = list(_scope, _msgdst, _in);
    DBPRINT("'range' args: ", args);

    if (yal_is_error(_msgdst)) {
        ERROR_ARGEVALFAIL(_msgdst, _in);
        return NIL();
    }
    if (len(args) != 2) {
        ERROR_INVALIDARGS("expected 2 inputs", _msgdst, args);
        return NIL();
    }
    start = first(args);
    end = second(args);
    if (start->type != TYPE_REAL || end->type != TYPE_REAL) {
        ERROR_INVALIDARGS("expected inputs of type REAL", _msgdst, args);
        return NIL();
    }
    if (start->real > end->real) {
        ERROR_INVALIDARGS("expected input 1 to be smaller than input 2", _msgdst, args);
        return NIL();
    }
    range = cons(_scope, NULL, NULL);
    curr = range;
    for (i = start->real; i < end->real; i++) {
        curr->car = real(_scope, i);
        curr->cdr = cons(_scope, NULL, NULL);
        curr = cdr(curr);
    }
    return range;
}

expr*
_PLUS2(VariableScope* _scope, yal_Msg* _msgdst, expr* _a, expr* _b)
{
    ASSERT_INV_SCOPE(_scope);
    if (!is_val(_a) || !is_val(_b)) {
        ERROR_INVALIDARGS("expected inputs to be values", _msgdst, cons(_scope, _a, _b));
        return real(_scope, 0);
    }
    if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
        return decimal(_scope, _a->decimal + _b->decimal);
    else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
        return decimal(_scope, _a->real + _b->decimal);
    else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
        return  decimal(_scope, _a->decimal + _b->real);
    return real(_scope, _a->real + _b->real);
}

expr*
buildin_plus(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    expr* args;
    expr* tmp;
    expr* result;
    ASSERT_INV_SCOPE(_scope);
    args = list(_scope, _msgdst, _in);
    if (yal_is_error(_msgdst)) {
        ERROR_ARGEVALFAIL(_msgdst, args);
        return NIL();
    }
    if (len(args) == 0) {
        return real(_scope, 0);
    }
    /*Check for non value inputs*/
    tmp = args;
    while (!is_nil(cdr(tmp))) {
        if (!is_val(car(tmp))) {
            ERROR_NONVALUEARG(_msgdst, args);
            return NIL();
        }
        tmp = cdr(tmp);
    }
    /*Do the math*/
    tmp = args;
    result = real(_scope, 0);
    while (!is_nil(cdr(tmp))) {
        result = _PLUS2(_scope, _msgdst, result, car(tmp));
        tmp = cdr(tmp);
        if (yal_is_error(_msgdst))
            return NIL();
    }
    return result;
}

expr*
_MINUS2(VariableScope* _scope, yal_Msg* _msgdst, expr* _a, expr* _b)
{
    ASSERT_INV_SCOPE(_scope);
    if (!is_val(_a) || !is_val(_b)) {
        ERROR_INVALIDARGS("expected inputs to be values", _msgdst, cons(_scope, _a, _b));
        return real(_scope, 0);
    }
    if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
        return decimal(_scope, _a->decimal - _b->decimal);
    else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
        return decimal(_scope, _a->real - _b->decimal);
    else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
        return  decimal(_scope, _a->decimal - _b->real);
    return real(_scope, _a->real - _b->real);
}

expr*
buildin_minus(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    expr* args;
    expr* tmp;
    expr* result;
    ASSERT_INV_SCOPE(_scope);
    args = list(_scope, _msgdst, _in);
    if (yal_is_error(_msgdst)) {
        ERROR_ARGEVALFAIL(_msgdst, args);
        return NIL();
    }
    /*Check for non value inputs*/
    if (len(args) == 0) {
        return real(_scope, 0);
    }
    tmp = args;
    while (!is_nil(cdr(tmp))) {
        if (!is_val(car(tmp))) {
            ERROR_NONVALUEARG(_msgdst, args);
            return NIL();
        }
        tmp = cdr(tmp);
    }
    /*Do the math*/
    tmp = args;
    result = variable_duplicate(_scope, first(args));
    tmp = cdr(tmp);
    while (!is_nil(cdr(tmp))) {
        result = _MINUS2(_scope, _msgdst, result, car(tmp));
        tmp = cdr(tmp);
        if (yal_is_error(_msgdst))
            return NIL();
    }
    return result;
}

expr*
_MULTIPLY2(VariableScope* _scope, yal_Msg* _msgdst, expr* _a, expr* _b)
{
    ASSERT_INV_SCOPE(_scope);
    if (!is_val(_a) || !is_val(_b)) {
        ERROR_INVALIDARGS("expected inputs to be values", _msgdst, cons(_scope, _a, _b));
        return real(_scope, 0);
    }
    if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
        return decimal(_scope, _a->decimal * _b->decimal);
    else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
        return decimal(_scope, _a->real * _b->decimal);
    else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
        return  decimal(_scope, _a->decimal * _b->real);
    return real(_scope, _a->real * _b->real);
}

expr*
buildin_multiply(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    expr* args;
    expr* tmp;
    expr* result;
    ASSERT_INV_SCOPE(_scope);
    args = list(_scope, _msgdst, _in);
    if (yal_is_error(_msgdst)) {
        ERROR_ARGEVALFAIL(_msgdst, args);
        return NIL();
    }
    /*Check for non value inputs*/
    if (len(args) == 0) {
        return real(_scope, 1);
    }
    tmp = args;
    while (!is_nil(cdr(tmp))) {
        if (!is_val(car(tmp))) {
            ERROR_NONVALUEARG(_msgdst, args);
            return NIL();
        }
        tmp = cdr(tmp);
    }
    /*Do the math*/
    tmp = args;
    result = real(_scope, 1);
    while (!is_nil(cdr(tmp))) {
        result = _MULTIPLY2(_scope, _msgdst, result, car(tmp));
        tmp = cdr(tmp);
        if (yal_is_error(_msgdst))
            return NIL();
    }
    return result;
}

expr*
_DIVIDE2(VariableScope* _scope, yal_Msg* _msgdst, expr* _a, expr* _b)
{
    ASSERT_INV_SCOPE(_scope);
    if (!is_val(_a) || !is_val(_b)) {
        ERROR_INVALIDARGS("expected inputs to be values", _msgdst, cons(_scope, _a, _b));
        return real(_scope, 0);
    }
    if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
        return decimal(_scope, _a->decimal / _b->decimal);
    else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
        return decimal(_scope, _a->real / _b->decimal);
    else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
        return  decimal(_scope, _a->decimal / _b->real);
    return real(_scope, _a->real / _b->real);
}

expr*
buildin_divide(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    expr* args;
    expr* tmp;
    expr* result;
    ASSERT_INV_SCOPE(_scope);
    args = list(_scope, _msgdst, _in);
    if (yal_is_error(_msgdst)) {
        ERROR_ARGEVALFAIL(_msgdst, args);
        return NIL();
    }
    if (len(args) == 0) {
        return real(_scope, 1);
    }
    /*Check for non value inputs*/
    tmp = args;
    while (!is_nil(cdr(tmp))) {
        if (!is_val(car(tmp))) {
            ERROR_NONVALUEARG(_msgdst, args);
            return NIL();
        }
        if ((tmp->type == TYPE_REAL && tmp->real == 0) ||
            (tmp->type == TYPE_DECIMAL && tmp->decimal == 0.f)) {
            ERROR_INVALIDARGS("division by zero!", _msgdst, args);
            return real(_scope, 0);
        }
        tmp = cdr(tmp);
    }
    /*Do the math*/
    tmp = args;
    result = variable_duplicate(_scope, first(args));
    tmp = cdr(tmp);
    while (!is_nil(cdr(tmp))) {
        result = _DIVIDE2(_scope, _msgdst, result, car(tmp));
        tmp = cdr(tmp);
        if (yal_is_error(_msgdst))
            return NIL();
    }
    return result;
}

char
_MATHEQUAL2(VariableScope* _scope, yal_Msg* _msgdst, expr* _a, expr* _b)
{
    ASSERT_INV_SCOPE(_scope);
    if (!is_val(_a) || !is_val(_b)) {
        ERROR_INVALIDARGS("expected inputs to be values", _msgdst, cons(_scope, _a, _b));
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
buildin_mathequal(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    expr* args;
    expr* oldtmp;
    expr* tmp;
    ASSERT_INV_SCOPE(_scope);
    args = list(_scope, _msgdst, _in);
    if (yal_is_error(_msgdst)) {
        ERROR_ARGEVALFAIL(_msgdst, args);
        return symbol(_scope, "NIL");
    }
    if (len(args) < 2) {
        return symbol(_scope, "t");
    }
    /*Check for non value inputs*/
    tmp = args;
    while (!is_nil(cdr(tmp))) {
        if (!is_val(car(tmp))) {
            ERROR_NONVALUEARG(_msgdst, args);
            return symbol(_scope, "NIL");
        }
        tmp = cdr(tmp);
    }
    /*check args for equality*/
    oldtmp = first(args);
    tmp = second(args);
    while (!is_nil(tmp)) {
        if(!_MATHEQUAL2(_scope, _msgdst, oldtmp, tmp))
            return symbol(_scope, "NIL");
        if (yal_is_error(_msgdst))
            return symbol(_scope, "NIL");
        tmp = cdr(tmp);
        oldtmp = cdr(oldtmp);
    }
    return symbol(_scope, "t");
}

expr*
buildin_defvar(VariableScope* _scope, yal_Msg* _msgdst, expr* _in)
{
    UNUSED(_scope);
    UNUSED(_msgdst);
    UNUSED(_in);
    return NIL();
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
    Env_add_buildin(_env, "list", list);
    Env_add_buildin(_env, "cons", buildin_cons);

    /*Accessors*/
    Env_add_buildin(_env, "car", buildin_car);
    Env_add_buildin(_env, "cdr", buildin_cdr);
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

    /*Program management*/
    //Env_add_buildin(_env, "var", buildin_defvar);
}

#endif /*YAL_IMPLEMENTATION*/
#endif /*YAL_H*/
