#ifndef YAL_BUILDINS
#define YAL_BUILDINS

#include "defs.h"
#include "yal2.h"

expr*
buildin_car(Environment* _env, expr* _e)
{
    /*"Head"*/
    UNUSED(_env);
    return (_e == NULL) ? _e->car : NULL;
}

expr*
buildin_cdr(Environment* _env, expr* _e)
{
    /*"Tail"*/
    UNUSED(_env);
    return (_e == NULL) ? _e->cdr : NULL;
}

expr*
buildin_first(Environment* _env, expr* _e)
{
    if (_e == NULL) return _e;
    expr* p = buildin_car(_env, _e);
    if (p == NULL) return p;
    return buildin_cdr(_env, p);
}

expr*
buildin_second(Environment* _env, expr* _e)
{
    return buildin_first(_env, buildin_first(_env, _e));
}

int
len(Environment* _env, expr* _e)
{
    /*http://clhs.lisp.se/Body/f_list_l.htm*/
    UNUSED(_env);
    int cnt = 0;
    expr* tmp = _e;
    while (tmp != NULL) {
        tmp = buildin_first(_env, tmp);
        if (tmp == NULL)
            break;
        cnt++;
    }
    return cnt;
}

expr*
buildin_len(Environment* _env, expr* _e)
{
    /*http://clhs.lisp.se/Body/f_list_l.htm*/
    UNUSED(_env);
    int cnt = 0;
    expr* tmp = _e;
    while (tmp != NULL) {
        tmp = buildin_first(_env, tmp);
        if (tmp == NULL)
            break;
        cnt++;
    }
    return real(_env, cnt);
}

expr*
buildin_eval(Environment* _env, expr* _e)
{
    /*TODO: Create Lookup Table for buildins so that they can be symbols!*/
    UNUSED(_env);
    if (_e == NULL)
        return NULL;

    if (CAR(_e) != NULL) {
        assert(CAR(_e)->type == TYPE_BUILDIN);
        _e = CAR(_e)->buildin.fn(_env, _e);
    }
    if (CDR(_e) != NULL) {
        _e = buildin_eval(_env, CDR(_e));
    }
    return _e;
}

expr*
buildin_write(Environment* _env, expr* _e)
{
    ERROR_INV_ENV(_env);
    assert(len(_env, _e) == 1);
    print_value(_env, buildin_first(_env, _e));
    /*TODO: write should not newline!*/
    printf("\n");
    return _e->cdr;
}

expr*
buildin_print(Environment* _env, expr* _e)
/*Check for dotted pair, and print: (a . b) */
{
    if (is_nil(_e))
        return _e;
    /*Handle CAR*/
    if (_e->car->type != TYPE_CCONS) {
        print_value(_env, _e->car);
        printf(" ");
    }
    else if (_e->car->type == TYPE_CCONS) {
            buildin_print(_env, _e->car);
    }

    /*Handle CDR*/
    if (_e->cdr != NULL) {
        if (_e->cdr->car->type == TYPE_CCONS) printf("(");
        buildin_print(_env, _e->cdr);
        if (_e->cdr->car->type == TYPE_CCONS) printf(")");
    }
    return _e;
}

expr*
buildin_read(Environment* _env, expr* _e)
{
    expr* res;
    int cursor = 0;
    assert(_e != NULL);
    assert(_e->cdr != NULL);
    assert(_e->cdr->car != NULL);
    assert(_e->cdr->car->type == TYPE_STRING);
    res = expr_lex(_env, _e->cdr->car->string.c_str, &cursor);
    printf("Tree: "); buildin_print(_env, res); printf("\n");
    return res;
}

expr*
buildin_plus(Environment* _env, expr* _e)
{
    UNUSED(_env);
    int sum = 0;
    expr* tmp = NULL;
    tmp = CDR(_e);
    while (tmp != NULL) {
        /*Eval cons cell to number*/
        if (CAR(tmp)->type == TYPE_CCONS)
            tmp->car = buildin_eval(_env, CAR(tmp));
        assert(CAR(tmp)->type == TYPE_REAL);
        sum += CAR(tmp)->real;
        tmp = CDR(tmp);
    }
    return real(_env, sum);
}

expr*
buildin_minus(Environment* _env, expr* _e)
{
    UNUSED(_env);
    int sum = 0;
    expr* tmp = NULL;
    /*TODO: if only a single CDR exists, we need to be a negation operator instead*/
    tmp = CDR(_e);
    sum = CAR(tmp)->real;
    tmp = CDR(tmp);
    if (CAR(tmp) == NULL)
        return real(_env, sum);
    while (tmp != NULL) {
        /*Eval cons cell to number*/
        if (CAR(tmp)->type == TYPE_CCONS)
            tmp->car = buildin_eval(_env, CAR(tmp));
        assert(CAR(tmp)->type == TYPE_REAL);
        sum -= CAR(tmp)->real;
        tmp = CDR(tmp);
    }
    return real(_env, sum);
}

expr*
buildin_multiply(Environment* _env, expr* _e)
{
    UNUSED(_env);
    int sum = 1;
    expr* tmp = NULL;
    tmp = CDR(_e);
    while (tmp != NULL) {
        /*Eval cons cell to number*/
        if (CAR(tmp)->type == TYPE_CCONS)
            tmp->car = buildin_eval(_env, CAR(tmp));
        assert(CAR(tmp)->type == TYPE_REAL);
        sum *= CAR(tmp)->real;
        tmp = CDR(tmp);
    }
    return real(_env, sum);
}

expr*
buildin_divide(Environment* _env, expr* _e)
{
    UNUSED(_env);
    int sum = 1;
    expr* tmp = NULL;
    tmp = CDR(_e);
    while (tmp != NULL) {
        /*Eval cons cell to number*/
        if (CAR(tmp)->type == TYPE_CCONS)
            tmp->car = buildin_eval(_env, CAR(tmp));
        assert(CAR(tmp)->type == TYPE_REAL);
        sum /= CAR(tmp)->real;
        tmp = CDR(tmp);
    }
    return real(_env, sum);
}

#endif /*YAL_BUILDINS*/