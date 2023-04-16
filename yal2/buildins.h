#ifndef YAL_BUILDINS
#define YAL_BUILDINS

#include "tstr.h"
#include "defs.h"
#include "core.h"

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
    return cdr(_e);
}

expr*
buildin_first(Environment* _env, expr* _e)
{
    UNUSED(_env);
    return car(_e);
}

expr*
buildin_second(Environment* _env, expr* _e)
{
    UNUSED(_env);
    return car(cdr(_e));
}

expr*
buildin_third(Environment* _env, expr* _e)
{
    UNUSED(_env);
    return car(cdr(cdr(_e)));
}

expr*
buildin_len(Environment* _env, expr* _e)
{
    /*http://clhs.lisp.se/Body/f_list_l.htm*/
    UNUSED(_env);
    return real(_env, len(_e));
}

expr*
buildin_quote(Environment* _env, expr* _e)
/*TODO: I think quote return CAR, not CDAR!*/
{
    ERRCHECK_INV_ENV(_env);
    ERRCHECK_TYPECHECK(_e, TYPE_CCONS);
    if (_e->cdr == NULL || _e->cdr->car == NULL)
        return NIL;
    return _e->cdr->car;
}

expr*
buildin_eval(Environment* _env, expr* _e)
{
    ERRCHECK_INV_ENV(_env);
    ERRCHECK_NIL(_e);
    int i;
    Buildin* buildin = NULL;
    expr* fn = car(_e);
    expr* args = cdr(_e);
    //expr* fn = car(car(_e));
    //expr* args = car(cdr(_e));
    printf("EVAL function symbol: %s\n", fn->symbol.c_str);
    printf("EVAL function args:   "); buildin_print(_env, args); printf("\n");
    ERRCHECK_TYPECHECK(fn, TYPE_SYMBOL);
    ERRCHECK_TYPECHECK(args, TYPE_CCONS);
    if (fn != NULL) {
        for (i = 0; i < Buildins_len(&_env->buildins); i++) {
            buildin = Buildins_peek(&_env->buildins, i);
            if (tstr_equal(&buildin->name, &fn->symbol)) {
                return buildin->fn(_env, args);
            }
        }
    }
    /*NOTE: I do not know if this is a good thing*/
    //if (CDR(_e) != NULL) {
    //    _e = buildin_eval(_env, CDR(_e));
    //}
    ERRCHECK_UNREACHABLE();
    return _e;
}

expr*
buildin_write(Environment* _env, expr* _e)
{
    ERRCHECK_INV_ENV(_env);
    ERRCHECK_LEN(_e, 1);
    print_value(car(_e));
    /*TODO: write should technically not newline!*/
    printf("\n");
    return _e;
}

expr*
buildin_print(Environment* _env, expr* _e)
/*Check for dotted pair, and print: (a . b)*/
{
    if (is_nil(_e))
        return _e;
    /*Handle CAR*/
    if (!is_nil(_e->car)) {
        if (car(_e)->type != TYPE_CCONS) {
             /*NOTE: using tprint insead of print here for debugging!*/
            //print_value(_e->car);
            tprint_value(_e->car);
            printf(" ");
        }
        else if (car(_e)->type == TYPE_CCONS) {
            buildin_print(_env, _e->car);
        }
    }
    /*Handle CDR*/
    if (!is_nil(cdr(_e))) {
        if (!is_nil(car(cdr(_e))))
            printf("(");
        buildin_print(_env, cdr(_e));
        if (!is_nil(car(cdr(_e))))
            printf(")");
    }
    return _e;
}

expr*
buildin_read(Environment* _env, expr* _e)
{
    expr* res;
    int cursor = 0;
    ERRCHECK_INV_ENV(_env);
    ERRCHECK_NIL(_e);
    ERRCHECK_TYPECHECK(_e, TYPE_CCONS);
    ERRCHECK_TYPECHECK(_e->car, TYPE_STRING);
    res = expr_lex(_env, _e->car->string.c_str, &cursor);
    return res;
}

expr*
buildin_plus(Environment* _env, expr* _e)
{
    UNUSED(_env);
    int sum = 0;
    expr* tmp = NULL;
    tmp = cdr(_e);
    while (tmp != NULL) {
        /*Eval cons cell to number*/
        if (car(tmp)->type == TYPE_CCONS)
            tmp->car = buildin_eval(_env, car(tmp));
        assert(car(tmp)->type == TYPE_REAL);
        sum += car(tmp)->real;
        tmp = car(tmp);
    }
    return real(_env, sum);
}

expr*
buildin_minus(Environment* _env, expr* _e)
{
    UNUSED(_env);
    int sum = 0;
    expr* tmp = NULL;
    /*TODO: if only a single cdr exists, we need to be a negation operator instead*/
    tmp = cdr(_e);
    sum = car(tmp)->real;
    tmp = cdr(tmp);
    if (car(tmp) == NULL)
        return real(_env, sum);
    while (tmp != NULL) {
        /*Eval cons cell to number*/
        if (car(tmp)->type == TYPE_CCONS)
            tmp->car = buildin_eval(_env, car(tmp));
        assert(car(tmp)->type == TYPE_REAL);
        sum -= car(tmp)->real;
        tmp = cdr(tmp);
    }
    return real(_env, sum);
}

expr*
buildin_multiply(Environment* _env, expr* _e)
{
    UNUSED(_env);
    int sum = 1;
    expr* tmp = NULL;
    tmp = cdr(_e);
    while (tmp != NULL) {
        /*Eval cons cell to number*/
        if (car(tmp)->type == TYPE_CCONS)
            tmp->car = buildin_eval(_env, car(tmp));
        assert(car(tmp)->type == TYPE_REAL);
        sum *= car(tmp)->real;
        tmp = cdr(tmp);
    }
    return real(_env, sum);
}

expr*
buildin_divide(Environment* _env, expr* _e)
{
    UNUSED(_env);
    int sum = 1;
    expr* tmp = NULL;
    tmp = cdr(_e);
    while (tmp != NULL) {
        /*Eval cons cell to number*/
        if (car(tmp)->type == TYPE_CCONS)
            tmp->car = buildin_eval(_env, car(tmp));
        assert(car(tmp)->type == TYPE_REAL);
        sum /= car(tmp)->real;
        tmp = cdr(tmp);
    }
    return real(_env, sum);
}

Environment*
Environment_add_buildins(Environment* _env)
{
    ERRCHECK_INV_ENV(_env);
    Buildins_push(&_env->buildins, BUILDIN("car", buildin_car));
    Buildins_push(&_env->buildins, BUILDIN("cdr", buildin_cdr));
    Buildins_push(&_env->buildins, BUILDIN("+", buildin_plus));
    Buildins_push(&_env->buildins, BUILDIN("-", buildin_minus));
    Buildins_push(&_env->buildins, BUILDIN("*", buildin_multiply));
    Buildins_push(&_env->buildins, BUILDIN("/", buildin_divide));
    Buildins_push(&_env->buildins, BUILDIN("read", buildin_read));
    Buildins_push(&_env->buildins, BUILDIN("eval", buildin_eval));
    Buildins_push(&_env->buildins, BUILDIN("print", buildin_print));
    Buildins_push(&_env->buildins, BUILDIN("write", buildin_write));
    Buildins_push(&_env->buildins, BUILDIN("quote", buildin_quote));
    return _env;
}

#endif /*YAL_BUILDINS*/
