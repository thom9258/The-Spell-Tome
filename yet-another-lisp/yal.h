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

#define _LOOKS_LIKE_NUMBER(C) (((C) > '0' && (C) < '9') ? 1 : 0)

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

    RESTYPE_OK,
    RESTYPE_WARNING,
    RESTYPE_ERROR,

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

    "RESTYPE_OK"
    "RESTYPE_WARNING"
    "RESTYPE_ERROR"
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
    expr* result;
}Result;

#define RESULT_OK(result) \
    (Result) {RESTYPE_OK, tstr_view((char*)"OK"), result}
#define RESULT_WARNING(result, msg, ...) \
    (Result) {RESTYPE_WARNING, tstr_fmt((char*)msg, ##__VA_ARGS__), result}
#define RESULT_ERROR(msg, ...) \
    (Result) {RESTYPE_ERROR, tstr_fmt((char*)msg, ##__VA_ARGS__), NIL()}

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
typedef Result(*buildin_fn)(VariableScope* _scope, expr* _in);

typedef struct {
    tstr name;
    buildin_fn fn;
}Buildin;

#define t_type Buildin
#define arr_t_name Buildins
#include "vendor/tsarray.h"

struct VariableScope {
    Environment* env;
    struct VariableScope* outer;
    Variables variables;
};

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

/*Environment management*/

Environment* Env_new(Environment* _env);
void Env_destroy(Environment* _env);

VariableScope* VariableScope_new(VariableScope* _this, VariableScope* _outer, Environment* _env);
void VariableScope_destroy(VariableScope* _scope);

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
expr* cons(VariableScope *_env, expr* _car, expr* _cdr);
expr* real(VariableScope *_env, int _v);
expr* decimal(VariableScope *_env, float _v);
expr* symbol(VariableScope *_env, tstr _v);
expr* csymbol(VariableScope *_env, char* _v);
expr* string(VariableScope *_env, tstr _v);
void variable_delete(VariableScope *_scope, expr* _atom);

/*Printing*/
void printexpr(expr* _args);
tstr stringifyexpr(expr* _args);

/*Core*/
Result read(VariableScope* _env, char* _program_str);
Result eval(VariableScope* _env, expr* _in);
Result progc(VariableScope* _env, expr* _in);


/******************************************************************************/
#define YAL_IMPLEMENTATION
#ifdef YAL_IMPLEMENTATION


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
_variable_new(VariableScope *_scope)
{
    ASSERT_INV_SCOPE(_scope);
    expr* e = (expr*)sbAllocator_get(&_scope->env->variable_allocator);
    if (e == NULL)
        ASSERT_NOMOREMEMORY();
    return e;
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
symbol(VariableScope *_scope, tstr _v)
{
    ASSERT_INV_SCOPE(_scope);
    expr* e = _variable_new(_scope);
    e->type = TYPE_SYMBOL;
    e->symbol = _v;
    return e;
}

expr*
csymbol(VariableScope* _scope, char* _sym)
{
    return symbol(_scope, tstr_(_sym));
}

expr*
string(VariableScope *_scope, tstr _v)
{
    ASSERT_INV_SCOPE(_scope);
    expr* e = _variable_new(_scope);
    e->type = TYPE_STRING;
    e->string = _v;
    return e;
}

void
printexpr(expr* _arg)
{
    tstr str = stringifyexpr(_arg);
    printf("%s", str.c_str);
    tstr_destroy(&str);
}

tstr
_stringifycons(expr* _args, char* _open, char* _close)
{
    tstr dst = {0};
    tstr tmpcar = {0};
    tstr tmpcdr = {0};
    tstr open = tstr_view(_open);
    tstr close = tstr_view(_close);
    tstr space = tstr_view(" ");

    /*Handle CAR*/
    tmpcar = stringifyexpr(car(_args));
    tmpcdr = stringifyexpr(cdr(_args));

    if (is_cons(car(_args))) {
        tstr_add2end(&dst, &open);
        tstr_add2end(&dst, &tmpcar);
        tstr_add2end(&dst, &close);
    }
    else {
            tstr_add2end(&dst, &tmpcar);
    }
    /*Handle CDR*/
    if (!is_nil(cdr(cdr(_args)))) {
        tstr_add2end(&dst, &space);
        tstr_add2end(&dst, &tmpcdr);
    }
    return dst;
}

tstr
stringifyexpr(expr* _arg)
{
    tstr dst = {0};
    if (is_nil(_arg))
        return tstr_("NIL");

    if (is_dotted(_arg)) {
        tstr carstr = stringifyexpr(car(_arg));
        tstr cdrstr = stringifyexpr(cdr(_arg));
        dst = tstr_fmt("(%s . %s)", carstr.c_str, cdrstr.c_str);
        tstr_destroy(&carstr);
        tstr_destroy(&cdrstr);
        return dst;
    }
    switch (_arg->type) {
    case TYPE_CONS:
        dst = _stringifycons(_arg, "(", ")");
        break;
    case TYPE_ARRAY:
        dst  =_stringifycons(_arg, "[", "]");
        break;
    case TYPE_DICTIONARY:
        dst = _stringifycons(_arg, "#{", "}");
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
        dst = tstr_fmt("\"%s\"", &_arg->string);
        break;
    default:
        ASSERT_UNREACHABLE();
    };
    return dst;
}

char
_is_symbol_real_or_decimal(tstr* _num)
/*TODO: Make this more robust..*/
{
    tstr decimal_indicator = tstr_view(".");
    if (tstr_find(_num, &decimal_indicator) == TSTR_INVALID)
        return TYPE_REAL;
    return TYPE_DECIMAL;
}

//////////////////////////////////////////////////////////////
// TEST LEXING
//////////////////////////////////////////////////////////////

#define _IS_WHITESPACE(C) \
    (((C) == ' ' || (C) == '\t' || (C) == '\n') ? 1 : 0)


char
is_literal_token(char _t)    
{
    const char ltokens[] = {'(', ')', '\'','\'', '\"', '[', ']'};
    const char ltoken_len = sizeof(ltokens) / sizeof(ltokens[0]);
    char i;
    for (i = 0; i < ltoken_len; i++)
        if (ltokens[i] == _t)
            return _t;
    return 0;
}

tstr
_get_next_token(VariableScope *_scope, char** _cursor)
{
    tstr token = {0};
    int len = 0;
    while (!_IS_WHITESPACE(*_cursor[len]) && !is_literal_token(*_cursor[len]))
        len++;
    token = tstr_n(*_cursor, len);
    *_cursor += tstr_length(&token);
    return token;
}

void
_trim_whitespace(char** _cursor)
{
    while (_IS_WHITESPACE(**_cursor))
        (*_cursor)++;
}

char
tstr_equalc(tstr* _a, char* _b)
{
    tstr b = tstr_view(_b);
    return tstr_equal(_a, &b);
}

expr*
_lex(VariableScope* _scope, char* _cursor)
{
    /*TODO: Does not support special syntax for:
    quote = '
    array = []
    dotted lists (a . 34)
    */
    ASSERT_INV_SCOPE(_scope);
    assert(_cursor != NULL && "Invalid program given to lexer");
    assert(Buildins_len(&_scope->env->buildins) > 0 && "No buildin functions in environment!");

    tstr token;
    tstr cmp = {0};
    expr* lexed_program = cons(_scope, NULL, NULL);
    expr* curr = lexed_program;
    expr* extracted = lexed_program;

    while (*_cursor != '\0' || *_cursor == ')') {
        _trim_whitespace(&_cursor);
        token = _get_next_token(_scope, &_cursor);

        if (tstr_equalc(&token, "("))
            extracted = _lex(_scope, _cursor);
        else if (tstr_equalc(&token, "\""))
            extracted = _token2string(_scope, &token);
        else if (_LOOKS_LIKE_NUMBER(token.c_str[0]))
            extracted = _token2number(_scope, &token);
        else
            extracted = _token2symbol(_scope, &token);

        /*Insert extracted into lexed program*/
        curr->car = extracted;
        curr->cdr = cons(_scope, NULL, NULL);
        curr = curr->cdr;
    }
    return lexed_program;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

#if 0
expr*
_lex_number(VariableScope *_scope, char* _start, int* _cursor)
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
        return real(_scope, tstr_to_int(&str));
    return decimal(_scope, tstr_to_float(&str));
}

expr*
_lex_string(VariableScope *_scope, char* _start, int* _cursor)
{
    int len = 1;
    tstr str;
    while (_start[len] != '"')
        len++;
    str = tstr_n(_start, len);
    printf("lexed string [%s]\n", str.c_str);
    (*_cursor) += tstr_length(&str);
    return string(_scope, str);
}

expr*
_lex_symbol(VariableScope *_scope, char* _start, int* _cursor)
{
    int len = 0;
    tstr str;
    while (!_IS_WHITESPACE(_start[len]) && _start[len] != '(' && _start[len] != ')')
        len++;
    str = tstr_n(_start, len);
    (*_cursor) += tstr_length(&str)-1;
    return symbol(_scope, str);
}

expr*
_lex(VariableScope* _scope, char* _program, int* _cursor)
{
    /*TODO: Does not support special syntax for:
    quote = '
    array = []
    dotted lists (a . 34)
    */
    expr* first = NULL;
    expr* curr = NULL;
    expr* extracted = NULL;
    char* index = NULL;

    ASSERT_INV_SCOPE(_scope);
    assert(_program != NULL && "Invalid program given to lexer");
    assert(_cursor != NULL && "Invalid cursor given to lexer");
    assert(Buildins_len(&_scope->env->buildins) > 0 && "No buildin functions in environment!");

    curr = cons(_scope, NULL, NULL);
    while (1) {
        index = _program + (*_cursor)++;
        if (_IS_WHITESPACE(*index)) {
            continue;
        }
        else if (*index == '\0' || *index == ')') {
            return first;
        }
        else if (*index ==  '(') {
            extracted = _lex(_scope, _program, _cursor);
        }
        else if (*index == '"') {
            extracted = _lex_string(_scope, index+1, _cursor);
        }
        else if (_LOOKS_LIKE_NUMBER(*index)) {
            extracted = _lex_number(_scope, index, _cursor);
        }
        else {
            extracted = _lex_symbol(_scope, index, _cursor);
        }
        /*Insert extracted into expression*/
        curr->car = extracted;
        if (!first)
            first = curr; 
        curr->cdr = cons(_scope, NULL, NULL);
        curr = curr->cdr;
    }
    ASSERT_UNREACHABLE();
}

#endif

Result
read(VariableScope* _scope, char* _program_str)
{
    ASSERT_INV_SCOPE(_scope);
    assert(_program_str != NULL && "Given program str is NULL");
    int cursor = 0;
    expr* lexed = _lex(_scope, _program_str, &cursor);
    return RESULT_OK(lexed);
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

Result
progc(VariableScope* _scope, expr* _in)
/*progc() is a list evaller that expects a list of eval-able lists */
{
    //usermsg msg;
    expr* root = cons(_scope, NULL, NULL);
    expr* outtmp = root;
    expr* tmp = _in;
    Result curres;
    ASSERT_INV_SCOPE(_scope);
    //DBPRINT("input to progc: ", _in);
    if (!is_cons(_in))
        return RESULT_ERROR("'progc' Expected a list of eval'able arguments!"
                            "Got %s\n", stringifyexpr(_in));

    while_not_nil(tmp) {
        curres = eval(_scope, car(tmp));
        if (RESULT_NOT_OK(curres))
            return RESULT_ERROR("'progc' could not evaluate expr %s\n", stringifyexpr(car(tmp)));
        outtmp->car = curres.result;
        DBPRINT("progc evalled: ", car(tmp));
        DBPRINT("to ", car(outtmp));
        outtmp->cdr = cons(_scope, NULL, NULL);
        outtmp = cdr(outtmp);
        tmp = cdr(tmp);
    }
    return RESULT_OK(root);
}

Result
eval(VariableScope* _scope, expr* _in)
{
    expr* fn;
    expr* args;
    Result result;
    //usermsg msg = USERMSG_OK();
    Buildin* buildin;

    ASSERT_INV_SCOPE(_scope);
    switch (_in->type) {
    case TYPE_CONS:
        /*We are sanitizing inputs here by removing a cons cell*/
        /*TODO: This is what i think goes wrong*/
        _in = car(_in);
        fn = car(_in);
        args = cdr(_in);
        DBPRINT("(eval) NAME: ", fn);
        DBPRINT("(eval) ARGS: ", args);
        if (is_cons(fn) || fn->type != TYPE_SYMBOL)
            return RESULT_ERROR("'eval' expected callable function, got %s\n", stringifyexpr(fn));
        buildin = _find_buildin(_scope->env, fn);
        result = buildin->fn(_scope, args);
        break;
    case TYPE_SYMBOL:
        /*TODO: Check symbol table and return value of symbol*/
        return RESULT_OK(_in);
        break;
    default:
        return RESULT_OK(_in);
        break;
    };

    return result;
}

/*********************************************************************/
/* Buildin Functions */
/*********************************************************************/

Result
buildin_quote(VariableScope* _scope, expr* _in)
{
    ASSERT_INV_SCOPE(_scope);
    return RESULT_OK(_in);
}

Result
buildin_car(VariableScope* _scope, expr* _in)
{
    Result args;
    ASSERT_INV_SCOPE(_scope);
    args = progc(_scope, _in);
    if (RESULT_NOT_OK(args))
        return RESULT_ERROR("'car' could not evaluate arguments %s\n", _in);
    return RESULT_OK(car(args.result));
}

Result
buildin_cdr(VariableScope* _scope, expr* _in)
{
    Result args;
    ASSERT_INV_SCOPE(_scope);
    args = progc(_scope, _in);
    if (RESULT_NOT_OK(args))
        return RESULT_ERROR("'car' could not evaluate arguments %s\n", _in);
    return RESULT_OK(cdr(args.result));
}

Result
buildin_cons(VariableScope* _scope, expr* _in)
{
    Result args;
    ASSERT_INV_SCOPE(_scope);
    args = progc(_scope, _in);
    if (RESULT_NOT_OK(args))
        return RESULT_ERROR("'car' could not evaluate arguments %s\n", _in);
    return RESULT_OK(cons(_scope, first(args.result), second(args.result)));
}

Result
buildin_range(VariableScope* _scope, expr* _in)
{
    //Result result;
    Result args;
    //expr* args;
    expr* start;
    expr* end;
    expr* range;
    expr* curr;
    int i;

    ASSERT_INV_SCOPE(_scope);
    args = progc(_scope, _in);
    if (RESULT_NOT_OK(args))
        return RESULT_ERROR("'range' could not evaluate inputs %s\n", stringifyexpr(_in));
    if (len(args.result) != 2)
        return RESULT_ERROR("'range' expected 2 inputs, min & max, not %s\n", stringifyexpr(_in));

    start = first(args.result);
    end = second(args.result);
    if (start->type != TYPE_REAL)
        return RESULT_ERROR("'range' first input is not a REAL type.\n");
    if (end->type != TYPE_REAL)
        return RESULT_ERROR("'range' second input is not a REAL type.\n");
    if (start->real > end->real)
        return RESULT_ERROR("'range' first input is larger than the second input.\n");

    range = cons(_scope, NULL, NULL);
    curr = range;
    for (i = start->real; i < end->real; i++) {
        curr->car = real(_scope, i);
        curr->cdr = cons(_scope, NULL, NULL);
        curr = cdr(curr);
    }
    return RESULT_OK(range);
}

#if 0
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
    ASSERT_INV_SCOPE(_scope);
    msg = progc(_env, &args, _in);
    if (USERMSG_IS_ERROR(&msg)) return msg;
    //DBPRINT("result of progc in + ", args);

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
    ASSERT_INV_SCOPE(_scope);
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
#endif

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
    Env_add_buildin(_env, "range", buildin_range);
    Env_add_buildin(_env, "car", buildin_car);
    Env_add_buildin(_env, "cdr", buildin_cdr);
    Env_add_buildin(_env, "quote", buildin_quote);
    Env_add_buildin(_env, "cons", buildin_cons);
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
