#ifndef YAL_BUILDINS
#define YAL_BUILDINS

#include "tstr.h"
#include "defs.h"
#include "core.h"

expr*
buildin_car(Environment* _env, expr* _e)
{
    _e = _try_nest_sanitize(_e);
    /*NOTE: we dont need the exact handle, but a
            cons cell that points to car!!*/
    return cons(_env, car(_e), NIL);
}

expr*
buildin_cdr(Environment* _env, expr* _e)
{
    UNUSED(_env);
    _e = _try_nest_sanitize(_e);
    return cdr(_e);
}

expr*
buildin_first(Environment* _env, expr* _e)
{
    _e = _try_nest_sanitize(_e);
    return cons(_env, first(_e), NIL);
}

expr*
buildin_second(Environment* _env, expr* _e)
{
    _e = _try_nest_sanitize(_e);
    return cons(_env, second(_e), NIL);
}

expr*
buildin_third(Environment* _env, expr* _e)
{
    _e = _try_nest_sanitize(_e);
    return cons(_env, third(_e), NIL);
}

expr*
buildin_fourth(Environment* _env, expr* _e)
{
    _e = _try_nest_sanitize(_e);
    return cons(_env, fourth(_e), NIL);
}

expr*
buildin_fifth(Environment* _env, expr* _e)
{
    _e = _try_nest_sanitize(_e);
    return cons(_env, fifth(_e), NIL);
}

expr*
buildin_sixth(Environment* _env, expr* _e)
{
    _e = _try_nest_sanitize(_e);
    return cons(_env, sixth(_e), NIL);
}

expr*
buildin_seventh(Environment* _env, expr* _e)
{
    _e = _try_nest_sanitize(_e);
    return cons(_env, seventh(_e), NIL);
}

expr*
buildin_eighth(Environment* _env, expr* _e)
{
    _e = _try_nest_sanitize(_e);
    return cons(_env, eighth(_e), NIL);
}

expr*
buildin_ninth(Environment* _env, expr* _e)
{
    _e = _try_nest_sanitize(_e);
    return cons(_env, ninth(_e), NIL);
}

expr*
buildin_tenth(Environment* _env, expr* _e)
{
    _e = _try_nest_sanitize(_e);
    return cons(_env, tenth(_e), NIL);
}

expr*
buildin_nth(Environment* _env, expr* _e)
{
    UNUSED(_env);
    ERRCHECK_TYPE(_env, first(_e), TYPE_REAL);
    _e = _try_nest_sanitize(_e);
    int n = first(_e)->real;
    return nth(n, second(_e));
}

expr*
buildin_cons(Environment* _env, expr* _e)
{
    ASSERT_INV_ENV(_env);
    ERRCHECK_LEN(_env, _e, 2);
    ERRCHECK_NIL(_env, _e);
    return cons(_env, buildin_first(_env, _e), buildin_second(_env, _e));
}

expr*
buildin_len(Environment* _env, expr* _e)
{
    ASSERT_INV_ENV(_env);
    /*http://clhs.lisp.se/Body/f_list_l.htm*/
    return real(_env, len(_e));
}

expr*
buildin_quote(Environment* _env, expr* _e)
{
    ASSERT_INV_ENV(_env);
    return _e;
}

expr*
buildin_eval(Environment* _env, expr* _e)
{
    ASSERT_INV_ENV(_env);
    ERRCHECK_NIL(_env, _e);
    int i;
    Buildin* buildin = NULL;
    expr* fn;
    expr* args;
    _e = _try_nest_sanitize(_e);
    fn = car(_e);
    args = cdr(_e);
    //printf("EVAL fnsym=%s, args=(", fn->symbol.c_str); buildin_print(_env, args); printf(")\n");
    ERRCHECK_TYPE(_env, fn, TYPE_SYMBOL);
    ERRCHECK_TYPE(_env, args, TYPE_CCONS);
    if (fn != NULL) {
        for (i = 0; i < Buildins_len(&_env->buildins); i++) {
            buildin = Buildins_peek(&_env->buildins, i);
            if (tstr_equal(&buildin->name, &fn->symbol)) {
                return buildin->fn(_env, args);
            }
        }
    }
    ASSERT_UNREACHABLE();
    return _e;
}

expr*
buildin_write(Environment* _env, expr* _e)
{
    tstr w = (tstr) {0};
    ASSERT_INV_ENV(_env);
    if (car(_e)->type == TYPE_CCONS)
        _e->car = buildin_eval(_env, _e->car);
    ERRCHECK_LEN(_env, car(_e), 1);
    ERRCHECK_NIL(_env, car(_e));
    value2str(&w, car(_e));
    printf("%s\n", w.c_str);
    tstr_destroy(&w);
    return _e;
}

expr*
buildin_print(Environment* _env, expr* _e)
/*Check for dotted pair, and print: (a . b)*/
/*TODO: print needs to check car and cdr, and print value if car is a value instead of recursive on value*/
{
    tstr w = (tstr) {0};
    ASSERT_INV_ENV(_env);
    ERRCHECK_NIL(_env, _e);
    expr2str(&w, _e);

    printf("(%s)\n", w.c_str);
    //printf("(");
    //expr_print(_env, _e);
    //printf(")");
    return NIL;
}

expr*
buildin_read(Environment* _env, expr* _e)
{
    expr* res;
    int cursor = 0;
    ASSERT_INV_ENV(_env);
    ERRCHECK_NIL(_env, _e);
    ERRCHECK_TYPE(_env, _e, TYPE_CCONS);
    ERRCHECK_TYPE(_env, _e->car, TYPE_STRING);
    res = cons(_env, NIL, NIL);
    //printf("read given: %s\n", _e->car->string.c_str);
    expr_lex(_env, res, _e->car->string.c_str, &cursor);
    res = _try_nest_sanitize(res);
    return res;
}

expr*
buildin_plus(Environment* _env, expr* _e)
{
    ASSERT_INV_ENV(_env);
    _e = _try_nest_sanitize(_e);
    printf("plus given ("); buildin_print(_env, _e); printf(")\n");
    //printf("first="); print_value(first(_e)); printf("\n");
    //printf("second="); print_value(second(_e)); printf("\n");
    //printf("third="); print_value(third(_e)); printf("\n");

    int sum = 1;
    expr* tmp = NULL;
    tmp = cdr(_e);
    while (tmp != NULL) {
        /*Eval cons cell to number*/
        if (car(tmp)->type == TYPE_CCONS)
            tmp->car = buildin_eval(_env, car(tmp));
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
    ASSERT_INV_ENV(_env);
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
    ASSERT_INV_ENV(_env);
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
Environment_add_core(Environment* _env)
{
    ASSERT_INV_ENV(_env);
    /*List management*/
    Buildins_push(&_env->buildins, BUILDIN("cons", buildin_cons));
    Buildins_push(&_env->buildins, BUILDIN("car", buildin_car));
    Buildins_push(&_env->buildins, BUILDIN("cdr", buildin_cdr));
    Buildins_push(&_env->buildins, BUILDIN("nth", buildin_nth));
    Buildins_push(&_env->buildins, BUILDIN("quote", buildin_quote));
    Buildins_push(&_env->buildins, BUILDIN("first", buildin_first));
    Buildins_push(&_env->buildins, BUILDIN("second", buildin_second));
    Buildins_push(&_env->buildins, BUILDIN("third", buildin_third));
    Buildins_push(&_env->buildins, BUILDIN("fourth", buildin_fourth));
    Buildins_push(&_env->buildins, BUILDIN("fifth", buildin_fifth));
    Buildins_push(&_env->buildins, BUILDIN("sixth", buildin_sixth));
    Buildins_push(&_env->buildins, BUILDIN("seventh", buildin_seventh));
    Buildins_push(&_env->buildins, BUILDIN("eighth", buildin_eighth));
    Buildins_push(&_env->buildins, BUILDIN("ninth", buildin_ninth));
    Buildins_push(&_env->buildins, BUILDIN("tenth", buildin_tenth));

    /*Arimetrics*/
    Buildins_push(&_env->buildins, BUILDIN("+", buildin_plus));
    Buildins_push(&_env->buildins, BUILDIN("-", buildin_minus));
    Buildins_push(&_env->buildins, BUILDIN("*", buildin_multiply));
    Buildins_push(&_env->buildins, BUILDIN("/", buildin_divide));

    /*Utils*/
    Buildins_push(&_env->buildins, BUILDIN("read", buildin_read));
    Buildins_push(&_env->buildins, BUILDIN("eval", buildin_eval));
    Buildins_push(&_env->buildins, BUILDIN("print", buildin_print));
    Buildins_push(&_env->buildins, BUILDIN("write", buildin_write));
    return _env;
}

#endif /*YAL_BUILDINS*/
