#ifndef YAL_H
#define YAL_H

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define TSTRING_IMPLEMENTATION
#include "vendor/tstring.h"

#define TH_ALLOCATOR_IMPLEMENTATION
#include "vendor/th_allocator.h"

#define UNUSED(x) (void)(x)
#define UNIMPLEMENTED(fun) assert(1 && "UNIMPLEMENTED: " && fun)

#define YAL_INVALID_ATOM_HANDLE TH_INVALID_HANDLE

#define YAL_LIST_INIT_SIZE 3

#define YAL_ASSERT(expr, msg) assert((expr) && msg)

#define YAL_UNREACHABLE() \
    assert(1 && "CRITICAL ERROR: REACHED UNREACHABLE CODE!")

#define YAL_ASSERT_INV_ENV(env) \
    YAL_ASSERT(env != NULL, "Env Ptr is NULL!")

#define YAL_ASSERT_INV_HANDLE(env, atom) \
    YAL_ASSERT(yal_atom(env, atom) != NULL, "Atom uses an invalid handle!")

#define YAL_ASSERT_TYPECHECK(env, atom, expected) \
    YAL_ASSERT(yal_atom(env, atom)->type == expected, "Atom has wrong type!")

#define YAL_ASSERT_NOTNUMBER(env, atom) \
    YAL_ASSERT(yal_atom(env, atom)->type == YAL_REAL || yal_atom(env, atom)->type == YAL_DECIMAL, \
               "Atom is not a number!")

#define YAL_ASSERT_LISTLEN(env, list, check) \
    do { \
        YAL_ASSERT_INV_HANDLE(env, list);          \
        YAL_ASSERT_TYPECHECK(env, list, YAL_LIST);          \
        YAL_ASSERT(yal_AtomList_len(&yal_atom(env, list)->List) check, "List len does not comply based on given operator and length!");   \
    } while (0)

#define YAL_ASSERT_ZEROLIST(env, h) \
    YAL_ASSERT_LISTLEN(env, h, > 0)

enum YAL_TYPE {
    YAL_TYPE_INVALID = 0,
    /*Atom types*/
    YAL_NIL,
    YAL_LIST,
    YAL_SYMBOL,
    YAL_REAL,
    YAL_DECIMAL,

    /*Enviroment function/variable types*/
    YAL_ENV_BUILDIN_FN,
    YAL_ENV_VARIABLE,

    YAL_TYPE_COUNT
};

typedef struct yal_Environment yal_Environment;
typedef th_handle yal_AtomHandle;
#define t_type int
#define arr_t_name yal_AtomList
#include "vendor/tsarray.h"

typedef struct {
    char type;
    union {
        int Real;
        float Decimal;
        tstring_s Symbol;
        yal_AtomList List;
    };
}yal_Atom;

typedef void(*yal_buildin_fn)(yal_Environment*, yal_AtomHandle);

typedef struct {
    char type;
    tstring_s symbol;
    tstring_s info;
    union {
        yal_AtomHandle Variable;
        yal_buildin_fn Builin;
    };
}yal_Variable;
#define t_type yal_Variable
#define arr_t_name yal_VariableList
#include "vendor/tsarray.h"


struct yal_Environment {
    uint8_t* memory;
    th_blockallocator atom_allocator;

    yal_VariableList known;
};

/*Environment management*/
yal_Environment yal_env_new(uint32_t _max_atoms);
void yal_env_delete(yal_Environment* _env);
void yal_env_add_buildin_fn(yal_Environment* _env, char* symbol, char* info, yal_buildin_fn _fn);
void yal_env_add_buildins(yal_Environment* _env);
yal_Variable* yal_env_find_buildin(yal_Environment* _env, yal_AtomHandle _h);

/*Atom Types Creation and management*/
void yal_atom_print(yal_Atom* _a);
yal_Atom* yal_atom(yal_Environment* _env, yal_AtomHandle _h);
yal_AtomHandle yal_atom_nil_new(yal_Environment* _env);
yal_AtomHandle yal_atom_real_new(yal_Environment* _env, int _v);
yal_AtomHandle yal_atom_decimal_new(yal_Environment* _env, float _v);
yal_AtomHandle yal_atom_symbol_new(yal_Environment* _env, char* _sym);
void yal_atom_clear(yal_Environment* _env, yal_AtomHandle _h);
char yal_atom_equal(yal_Atom* _a, yal_Atom* _b);
void yal_atom_delete(yal_Environment* _env, yal_AtomHandle _h);

/*Evaluation and Debugging of the languane*/
void yal_print_chain(yal_Environment* _env, yal_AtomHandle _h);
void yal_parse(yal_Environment* _env, yal_AtomHandle _h);

/******************************************************************************/
#define YAL_IMPLEMENTATION
#ifdef YAL_IMPLEMENTATION

yal_AtomHandle
_yal_atom_memory_alloc(yal_Environment* _env)
{
    assert(_env !=  NULL);
    return th_blockallocator_take(&_env->atom_allocator);
}

void
_yal_atom_memory_free(yal_Environment* _env, yal_AtomHandle _h)
{
    assert(_env !=  NULL);
    th_blockallocator_return(&_env->atom_allocator, _h);
    
}

yal_AtomHandle
_yal_yal_buildin_add_atom_to_env(yal_Environment* _env, yal_AtomHandle _this)
{
    assert(_env != NULL);
    UNIMPLEMENTED("add_atom_to_env");
    return _this;
}

void
_yal_plus_2(yal_Atom* _a, yal_Atom* _b)
{
    if (_a->type == YAL_REAL && _b->type == YAL_REAL) {
        _a->Real += _b->Real;
    }
    else if (_a->type == YAL_DECIMAL && _b->type == YAL_DECIMAL) {
        _a->Decimal += _b->Decimal;
    }
    else if (_a->type == YAL_DECIMAL && _b->type == YAL_REAL) {
        _a->Decimal += _b->Real;
    }
    else if (_a->type == YAL_REAL && _b->type == YAL_DECIMAL) {
        _a->type = YAL_DECIMAL;
        _a->Decimal = _a->Real;
        _a->Decimal += _b->Decimal;
    }
}

#define _YAL_ARIHMETIC_2(op, a, b)        \
do { \
    if (a->type == YAL_REAL && b->type == YAL_REAL) a->Real op b->Real; \
    else if (a->type == YAL_DECIMAL && b->type == YAL_DECIMAL) a->Decimal op b->Decimal; \
    else if (a->type == YAL_DECIMAL && b->type == YAL_REAL) a->Decimal op b->Real; \
    else if (a->type == YAL_REAL && b->type == YAL_DECIMAL) { \
        a->type = YAL_DECIMAL; a->Decimal = a->Real; a->Decimal op b->Decimal; } \
} while (0)

void
_yal_buildin_multiply(yal_Environment* _env, yal_AtomHandle _this)
/*TODO: Does not work!*/
{
    yal_AtomHandle res;
    yal_AtomHandle tmp;
    yal_Atom* ths;

    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _this);
    YAL_ASSERT_TYPECHECK(_env, _this, YAL_LIST);
    YAL_ASSERT_ZEROLIST(_env, _this);

    ths = yal_atom(_env, _this);
    res = yal_AtomList_pop_front(&ths->List);
    YAL_ASSERT_INV_HANDLE(_env, res);
    YAL_ASSERT_NOTNUMBER(_env, res);

    while (yal_AtomList_len(&ths->List) > 0) {
        /*Pop each atom and sum its value*/
        tmp = yal_AtomList_pop_front(&ths->List);
        YAL_ASSERT_INV_HANDLE(_env, tmp);
        YAL_ASSERT_NOTNUMBER(_env, tmp);
        _YAL_ARIHMETIC_2(*=, yal_atom(_env, res), yal_atom(_env, tmp));
        yal_atom_delete(_env, tmp);
    }
    yal_AtomList_push(&ths->List, res);
}

void
_yal_buildin_divide(yal_Environment* _env, yal_AtomHandle _this)
/*TODO: Does not work!*/
{
    yal_AtomHandle res;
    yal_AtomHandle tmp;
    yal_Atom* ths;

    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _this);
    YAL_ASSERT_TYPECHECK(_env, _this, YAL_LIST);
    YAL_ASSERT_ZEROLIST(_env, _this);

    ths = yal_atom(_env, _this);
    res = yal_AtomList_pop_front(&ths->List);
    YAL_ASSERT_INV_HANDLE(_env, res);
    YAL_ASSERT_NOTNUMBER(_env, res);

    while (yal_AtomList_len(&ths->List) > 0) {
        /*Pop each atom and sum its value*/
        tmp = yal_AtomList_pop_front(&ths->List);
        YAL_ASSERT_INV_HANDLE(_env, tmp);
        YAL_ASSERT_NOTNUMBER(_env, tmp);
        _YAL_ARIHMETIC_2(/=, yal_atom(_env, res), yal_atom(_env, tmp));
        yal_atom_delete(_env, tmp);
    }
    yal_AtomList_push(&ths->List, res);
}
void
_yal_buildin_plus(yal_Environment* _env, yal_AtomHandle _this)
{
    yal_AtomHandle res;
    yal_AtomHandle tmp;
    yal_Atom* ths;

    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _this);
    YAL_ASSERT_TYPECHECK(_env, _this, YAL_LIST);
    YAL_ASSERT_ZEROLIST(_env, _this);

    ths = yal_atom(_env, _this);
    res = yal_AtomList_pop_front(&ths->List);
    YAL_ASSERT_INV_HANDLE(_env, res);
    YAL_ASSERT_NOTNUMBER(_env, res);


    while (yal_AtomList_len(&ths->List) > 0) {
        /*Pop each atom and sum its value*/
        tmp = yal_AtomList_pop_front(&ths->List);
        YAL_ASSERT_INV_HANDLE(_env, tmp);
        YAL_ASSERT_NOTNUMBER(_env, tmp);
        _YAL_ARIHMETIC_2(+=, yal_atom(_env, res), yal_atom(_env, tmp));
        yal_atom_delete(_env, tmp);
    }
    yal_AtomList_push(&ths->List, res);
}

void
_yal_buildin_minus(yal_Environment* _env, yal_AtomHandle _this)
{
    yal_AtomHandle res;
    yal_AtomHandle tmp;
    yal_Atom* ths;

    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _this);
    YAL_ASSERT_TYPECHECK(_env, _this, YAL_LIST);
    YAL_ASSERT_ZEROLIST(_env, _this);

    /*Minus can act as a negation operator too, meaning that:
      (- x) is the same as (* x -1)
     */
    if (yal_AtomList_len(&yal_atom(_env, _this)->List) == 1) {
        yal_AtomList_push(&yal_atom(_env, _this)->List, yal_atom_real_new(_env, -1));
        _yal_buildin_multiply(_env, _this);
        return;
    }

    ths = yal_atom(_env, _this);
    res = yal_AtomList_pop_front(&ths->List);
    YAL_ASSERT_INV_HANDLE(_env, res);
    YAL_ASSERT_NOTNUMBER(_env, res);

    while (yal_AtomList_len(&ths->List) > 0) {
        /*Pop each atom and sum its value*/
        tmp = yal_AtomList_pop_front(&ths->List);
        YAL_ASSERT_INV_HANDLE(_env, tmp);
        YAL_ASSERT_NOTNUMBER(_env, tmp);
        _YAL_ARIHMETIC_2(-=, yal_atom(_env, res), yal_atom(_env, tmp));
        yal_atom_delete(_env, tmp);
    }
    yal_AtomList_push(&ths->List, res);
}

void
_yal_buildin_equal(yal_Environment* _env, yal_AtomHandle _this)
{
    yal_AtomHandle res;
    yal_AtomHandle a;
    yal_AtomHandle b;
    yal_Atom* ths;

    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _this);
    YAL_ASSERT_TYPECHECK(_env, _this, YAL_LIST);
    YAL_ASSERT_LISTLEN(_env, _this, == 2);
    ths = yal_atom(_env, _this);

    a = yal_AtomList_pop_front(&ths->List);
    b = yal_AtomList_pop_front(&ths->List);
    YAL_ASSERT_NOTNUMBER(_env, a);
    YAL_ASSERT_NOTNUMBER(_env, b);

    if (yal_atom_equal(yal_atom(_env, a), yal_atom(_env, b)))
        res = yal_atom_real_new(_env, 1);
    else
        res = yal_atom_real_new(_env, 0);

    yal_atom_delete(_env, a);
    yal_atom_delete(_env, b);
    yal_AtomList_push(&ths->List, res);
}

void
_yal_buildin_write(yal_Environment* _env, yal_AtomHandle _this)
{
    yal_AtomHandle curr;
    yal_Atom* ths;

    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _this);
    YAL_ASSERT_TYPECHECK(_env, _this, YAL_LIST);
    YAL_ASSERT_ZEROLIST(_env, _this);

    ths = yal_atom(_env, _this);
    while (yal_AtomList_len(&ths->List) > 0) {
        curr = yal_AtomList_pop_front(&ths->List);
        yal_atom_print(yal_atom(_env, curr));
        printf(" ");
        yal_atom_delete(_env, curr);
    }
}

yal_Environment
yal_env_new(uint32_t _max_atoms)
{
    th_status ok;
    yal_Environment env = {0};

    if (_max_atoms == 0) _max_atoms = 128;

    env.memory = (uint8_t*)malloc(TH_BLOCKALLOCATOR_OPTIMAL_MEMSIZE(_max_atoms, sizeof(yal_Atom)));
    assert(env.memory != NULL);

    ok = th_blockallocator_setup(&env.atom_allocator, env.memory, sizeof(yal_Atom), _max_atoms);
    assert(ok == TH_OK);

    yal_VariableList_initn(&env.known, 10);
    return env;
}

void
yal_env_delete(yal_Environment* _env)
{
    assert(_env !=  NULL);
    th_blockallocator_destroy(&_env->atom_allocator);
    free(_env->memory);

    yal_VariableList_destroy(&_env->known);
}

void yal_env_add_buildin_fn(yal_Environment* _env, char* symbol, char* info, yal_buildin_fn _fn)
{
    assert(_env !=  NULL);
    yal_Variable v;
    v.type = YAL_ENV_BUILDIN_FN;
    v.info = tstring(info);
    v.symbol = tstring(symbol);
    v.Builin = _fn;
    yal_VariableList_push(&_env->known, v);
}

void
yal_env_add_buildins(yal_Environment* _env)
/*TODO: add support for:
- minus;
- multiply;
- divide;
- var;
- fn;
- read;
- env;
- version;
*/
{
    yal_env_add_buildin_fn(_env, "equal", "comparison operator.", _yal_buildin_equal);
    yal_env_add_buildin_fn(_env, "+", "Plus operator.", _yal_buildin_plus);
    yal_env_add_buildin_fn(_env, "-", "Minus operator.", _yal_buildin_minus);
    yal_env_add_buildin_fn(_env, "*", "Multiply operator.", _yal_buildin_multiply);
    yal_env_add_buildin_fn(_env, "/", "Divide operator.", _yal_buildin_divide);
    yal_env_add_buildin_fn(_env, "write", "Print operator", _yal_buildin_write);
}

yal_Variable*
yal_env_find_buildin(yal_Environment* _env, yal_AtomHandle _h)
{
    int i;
    int imax;
    yal_Variable* tmp;
    imax = yal_VariableList_len(&_env->known);
    for (i = 0; i < imax; i++) {
         tmp = yal_VariableList_peek(&_env->known, i);
         if (tstring_equal(&yal_atom(_env, _h)->Symbol, &tmp->symbol))
             return tmp;
    }
    return NULL;
}

yal_Atom*
yal_atom(yal_Environment* _env, yal_AtomHandle _h)
{
    assert(_env !=  NULL);
    return TH_BLOCK_ACCESS(&_env->atom_allocator, _h, yal_Atom);
}

void
yal_atom_print(yal_Atom* _a)
{
    if (_a == NULL) {
        printf("<UNPRINTABLE ATOM>");
        return;
    }
    switch (_a->type) {
    case YAL_NIL:
        printf("<NIL>");
        break;
    case YAL_LIST:
        printf("<LIST>");
        break;
    case YAL_SYMBOL:
        printf("%s", _a->Symbol.c_str);
        break;
    case YAL_REAL:
        printf("%d", _a->Real);
        break;
    case YAL_DECIMAL:
        printf("%f", _a->Decimal);
        break;
    default:
        assert(1 && "Unreachable");
    }
}

yal_AtomHandle yal_atom_nil_new(yal_Environment* _env)
{
    assert(_env != NULL);
    yal_AtomHandle h = _yal_atom_memory_alloc(_env);
    yal_atom(_env, h)->type = YAL_NIL;
    return h;
}

yal_AtomHandle
yal_atom_real_new(yal_Environment* _env, int _v)
{
    assert(_env != NULL);
    yal_AtomHandle h = _yal_atom_memory_alloc(_env);
    YAL_ASSERT_INV_HANDLE(_env, h);
    yal_atom(_env, h)->type = YAL_REAL;
    yal_atom(_env, h)->Real = _v;
    return h;
}

yal_AtomHandle
yal_atom_decimal_new(yal_Environment* _env, float _v)
{
    assert(_env != NULL);
    yal_AtomHandle h = _yal_atom_memory_alloc(_env);
    YAL_ASSERT_INV_HANDLE(_env, h);
    yal_atom(_env, h)->type = YAL_DECIMAL;
    yal_atom(_env, h)->Decimal = _v;
    return h;
}
   
yal_AtomHandle
yal_atom_symbol_new(yal_Environment* _env, char* _sym)
{
    assert(_env != NULL);
    yal_AtomHandle h = _yal_atom_memory_alloc(_env);
    YAL_ASSERT_INV_HANDLE(_env, h);
    assert(yal_atom(_env, h) != NULL);
    yal_atom(_env, h)->type = YAL_SYMBOL;
    yal_atom(_env, h)->Symbol = tstring(_sym);
    return h;
}

yal_AtomHandle
yal_atom_list_new(yal_Environment* _env)
{
    yal_AtomHandle h = _yal_atom_memory_alloc(_env);
    YAL_ASSERT_INV_HANDLE(_env, h);
    yal_atom(_env, h)->type = YAL_LIST;
    yal_AtomList_initn(&yal_atom(_env, h)->List, YAL_LIST_INIT_SIZE);
    return h;
}

void
yal_atom_clear(yal_Environment* _env, yal_AtomHandle _h)
{
    if (_env == NULL)
        return;
    YAL_ASSERT_INV_HANDLE(_env, _h);
    yal_Atom* p = yal_atom(_env, _h);
    if (p == NULL)
        return;

    /*Cleanup internal memory of associated type*/
    switch (p->type) {
    case YAL_SYMBOL:
        tstring_destroy(&p->Symbol);
        break;
    case YAL_LIST:
        assert(yal_AtomList_len(&p->List) == 0);
        yal_AtomList_destroy(&p->List);
        break;
    case YAL_REAL:
        break;
    case YAL_DECIMAL:
        break;
    default:
        assert(1 && "Unreachable");
    }
}

char
yal_atom_equal(yal_Atom* _a, yal_Atom* _b)
{
    if (_a->type == YAL_REAL && _b->type == YAL_REAL)
        return (_a->Real == _b->Real);
    else if (_a->type == YAL_DECIMAL && _b->type == YAL_DECIMAL)
        return (_a->Decimal == _b->Decimal);
    else if (_a->type == YAL_DECIMAL && _b->type == YAL_REAL)
        return (_a->Decimal == _b->Real);
    else 
        return (_a->Real == _b->Decimal);
}

void
yal_atom_delete(yal_Environment* _env, yal_AtomHandle _h)
{
    if (_env == NULL)
        return;
    YAL_ASSERT_INV_HANDLE(_env, _h);
    yal_atom_clear(_env, _h);
    _yal_atom_memory_free(_env, _h);
}

void
yal_print_chain(yal_Environment* _env, yal_AtomHandle _h)
{
    yal_Atom* p = yal_atom(_env, _h);

    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_INV_HANDLE(_env, _h);

    if (p->type == YAL_LIST) {
        printf("( ");
        int i;
        int imax = yal_AtomList_len(&p->List);
        for (i = 0; i < imax; i++) {
            yal_print_chain(_env, *yal_AtomList_peek(&p->List, i));
        }
        printf(") ");
    } else {
        yal_atom_print(p);
        printf(" ");
    }
}

void
_yal_parse_symbol(yal_Environment* _env, yal_AtomHandle _h)
{
    yal_Variable* var;
    var = yal_env_find_buildin(_env, _h);

    YAL_ASSERT(var->type != YAL_ENV_VARIABLE, "FOUND FUNCTION AS NON-FIRST ATOM IN LIST");
    
    /*Replace sym with value of enviroment symbol*/
    yal_atom_clear(_env, _h);
    *yal_atom(_env, _h) = *yal_atom(_env, var->Variable);
}

void _yal_parse_sanitize(yal_Environment *_env, yal_AtomHandle _h)
/*Sanitize list, sanitazion is:
*/
{
    YAL_ASSERT_INV_ENV(_env);
    YAL_ASSERT_ZEROLIST(_env, _h);
    
}

void
_yal_parse_list(yal_Environment* _env, yal_AtomHandle _h)
{
    char has_buildin_fn = 0;
    int i;
    yal_AtomHandle tmp;
    yal_Variable* var;
    yal_Atom* p;

    YAL_ASSERT_INV_HANDLE(_env, _h);
    YAL_ASSERT_TYPECHECK(_env, _h, YAL_LIST);

    p = yal_atom(_env, _h);
    YAL_ASSERT_ZEROLIST(_env, _h);

    yal_AtomHandle first = yal_AtomList_pop_front(&p->List);
    if (yal_atom(_env, first)->type == YAL_SYMBOL) {
        /*Save atom for evaluation after all the other elements*/
        var = yal_env_find_buildin(_env, first);
        if (var->type == YAL_ENV_BUILDIN_FN) {
            has_buildin_fn = 1;
        }
    }
    else {
        /*Put first atom back as if nothing happened*/
        yal_AtomList_push_front(&p->List, first);
    }

    for (i = 0; i < yal_AtomList_len(&p->List); i++) {
        tmp = *yal_AtomList_peek(&p->List, i);
        YAL_ASSERT_INV_HANDLE(_env, tmp);
        yal_parse(_env, tmp);
    }

    if (has_buildin_fn) {
        /*Evaluate the function found at start of list*/
        var->Builin(_env, _h);
    }
}

void
yal_parse(yal_Environment* _env, yal_AtomHandle _h)
{
// Check this guy:
// https://buildyourownlisp.com/chapter7_evaluation#trees
    assert(_env != NULL);
    yal_Atom* p;

    YAL_ASSERT_INV_HANDLE(_env, _h);
    p = yal_atom(_env, _h);

    //printf("yal_parse() recieved:\n"); yal_print_chain(_env, _h); printf("\n");

    switch (p->type) {
    case YAL_LIST: 
        _yal_parse_list(_env, _h);
        break;
    case YAL_SYMBOL: 
        _yal_parse_symbol(_env, _h);
        break;
    case YAL_DECIMAL: 
        /*FALLTHROUGH*/
    case YAL_REAL: 
        break;
    case YAL_NIL: 
        /*FALLTHROUGH*/
    default:
        assert(0 && "UNREACHABLE");
        break;
    };
}

#endif /*YAL_IMPLEMENTATION*/
#endif /*YAL_H*/
