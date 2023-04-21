#ifndef YAL_BUILDINS
#define YAL_BUILDINS

#include "tstr.h"
#include "defs.h"
#include "core.h"

expr*
buildin_car(Environment* _env, expr* _e)
{
    UNUSED(_env);
    _e = _try_nest_sanitize(_e);
    return car(_e);
}

expr*
buildin_cdr(Environment* _env, expr* _e)
{
    UNUSED(_env);
    _e = _try_nest_sanitize(_e);
    return cdr(_e);
}

expr*
buildin_cons(Environment* _env, expr* _e)
{
    /*TODO: Not working! we need to chain first and second in _e with cons*/
    UNUSED(_env);
    UNIMPLEMENTED("buildin_cons");
    return _e;
}

expr*
buildin_first(Environment* _env, expr* _e)
{
    UNUSED(_env);
    _e = _try_nest_sanitize(_e);
    return first(_e);
}

expr*
buildin_second(Environment* _env, expr* _e)
{
    UNUSED(_env);
    _e = _try_nest_sanitize(_e);
    return second(_e);
}

expr*
buildin_third(Environment* _env, expr* _e)
{
    UNUSED(_env);
    _e = _try_nest_sanitize(_e);
    return third(_e);
}

expr*
buildin_fourth(Environment* _env, expr* _e)
{
    UNUSED(_env);
    _e = _try_nest_sanitize(_e);
    return fourth(_e);
}

expr*
buildin_fifth(Environment* _env, expr* _e)
{
    UNUSED(_env);
    _e = _try_nest_sanitize(_e);
    return fifth(_e);
}

expr*
buildin_sixth(Environment* _env, expr* _e)
{
    UNUSED(_env);
    _e = _try_nest_sanitize(_e);
    return sixth(_e);
}

expr*
buildin_seventh(Environment* _env, expr* _e)
{
    UNUSED(_env);
    _e = _try_nest_sanitize(_e);
    return seventh(_e);
}

expr*
buildin_eighth(Environment* _env, expr* _e)
{
    UNUSED(_env);
    _e = _try_nest_sanitize(_e);
    return eighth(_e);
}

expr*
buildin_ninth(Environment* _env, expr* _e)
{
    UNUSED(_env);
    _e = _try_nest_sanitize(_e);
    return ninth(_e);
}

expr*
buildin_tenth(Environment* _env, expr* _e)
{
    UNUSED(_env);
    _e = _try_nest_sanitize(_e);
    return tenth(_e);
}

expr*
buildin_nth(Environment* _env, expr* _e)
{
    UNUSED(_env);
    ERRCHECK_TYPECHECK(first(_e), TYPE_REAL);
    _e = _try_nest_sanitize(_e);
    int n = first(_e)->real;
    return nth(n, second(_e));
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
{
    ERRCHECK_INV_ENV(_env);
    return _e;
}

expr*
buildin_eval(Environment* _env, expr* _e)
{
    ERRCHECK_INV_ENV(_env);
    ERRCHECK_NIL(_e);
    int i;
    Buildin* buildin = NULL;
    expr* fn;
    expr* args;
    _e = _try_nest_sanitize(_e);
    fn = car(_e);
    args = cdr(_e);
    //printf("EVAL fnsym=%s, args=(", fn->symbol.c_str); buildin_print(_env, args); printf(")\n");
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
    ERRCHECK_UNREACHABLE();
    return _e;
}

expr*
buildin_write(Environment* _env, expr* _e)
{
    ERRCHECK_INV_ENV(_env);
    if (car(_e)->type == TYPE_CCONS)
        _e->car = buildin_eval(_env, _e->car);
    ERRCHECK_LEN(car(_e), 1);

    //ERRCHECK_TYPECHECK(car(_e), TYPE_CCONS);
    ERRCHECK_NIL(car(_e));
    print_value(car(_e));
    /*TODO: write should technically not newline!*/
    printf("\n");
    return _e;
}

expr*
buildin_print(Environment* _env, expr* _e)
/*Check for dotted pair, and print: (a . b)*/
/*TODO: print needs to check car and cdr, and print value if car is a value instead of recursive on value*/
{
    //if (is_nil(_e))
    //return NIL;
    if (_e->type != TYPE_CCONS)
    {
        print_value(_e);
        printf(" ");
        return NIL;
    }
    if (car(_e)->type == TYPE_CCONS && len(car(_e)) > 1) printf("(");
    buildin_print(_env, car(_e));
    if (car(_e)->type == TYPE_CCONS && len(car(_e)) > 1) printf(")");
    buildin_print(_env, cdr(_e));
    return NIL;
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
    res = cons(_env, NIL, NIL);
    //printf("read given: %s\n", _e->car->string.c_str);
    expr_lex(_env, res, _e->car->string.c_str, &cursor);
    return res;
}

expr*
buildin_plus(Environment* _env, expr* _e)
{
    UNUSED(_env);
    int sum = 0;
    int i;
    expr* tmp;
    printf("plus given ("); buildin_print(_env, _e); printf(")\n");
    printf("first="); print_value(first(_e)); printf("\n");
    printf("second="); print_value(second(_e)); printf("\n");
    printf("third="); print_value(third(_e)); printf("\n");

    for (i = 0; i < len(_e); i++) {
        printf("tmp=");print_value(tmp); printf("\n");
        tmp = nth(i, _e);
        if (tmp->type == TYPE_CCONS)
            tmp = buildin_eval(_env, tmp);
        if (tmp->type != TYPE_REAL/* || tmp->type != TYPE_DECIMAL*/)
            ERRCHECK_UNREACHABLE();
        sum += tmp->real;
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
Environment_add_core(Environment* _env)
{
    ERRCHECK_INV_ENV(_env);
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
