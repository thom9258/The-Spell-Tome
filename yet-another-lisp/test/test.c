#include "testlib.h"
#include "../yal.h"

expr*
read_verbose(Environment* _e, char* _p)
{
    ASSERT_INV_ENV(_e);
    printf("string: %s\n", _p);
    expr* prediction = read(_e, _p);
    printf("lexed:  "); print(prediction); printf("\n");
    return prediction;
}

expr*
read_eval_print(Environment* _e, char* _p)
{
    ASSERT_INV_ENV(_e);
    expr* program = read(_e, _p);
    expr* result = eval(_e, program);
    print(result); printf("\n");
    return result;
}

expr*
read_eval_print_verbose(Environment* _e, char* _p, char* _gt)
{
    ASSERT_INV_ENV(_e);
    expr* program = read(_e, _p);
    printf("YAL> "); print(program); printf("\n");
    printf("EXP> "); printf("%s\n", _gt);
    expr* result = eval(_e, program);
    printf("RES> "); print(result); printf("\n");
    return result;
}

char
read_eval_print_compare(Environment* _e, char* _p, char* _cmp)
{
    expr* res = read_eval_print(_e, _p);
    printf("===\n");
    UNUSED(res);
    UNUSED(_cmp);
    /*if res == _cmp
          return 1
     */
    return 0;
}



expr* csymbol(Environment* _env, char* _sym)
{
    return symbol(_env, tstr_(_sym));
}

/*===============================================================*/

void
test_datatypes(void)
{
    expr* a;
    Environment e;
    Env_new(&e);
    Env_add_core(&e);
    TL_TEST(e.buildins.count > 0);
    printf("buildin count: %d\n", e.buildins.count);

    a = NIL();
    TL_TEST(is_nil(a));
    TL_TEST(a->type == TYPE_CONS);
    print(a);
    printf("\n");

    a = cons(&e, NIL(), NIL());
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_CONS);
    print(a);
    printf("\n");

    a = cons(&e, real(&e, 4), NIL());
    TL_TEST(!is_nil(a));
    TL_TEST(!is_nil(car(a)));
    TL_TEST(car(a)->type == TYPE_REAL);
    TL_TEST(is_nil(cdr(a)));
    print(a);
    printf("\n");

    a = real(&e, 3);
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_REAL);
    TL_TEST(a->real == 3);
    print(a);
    printf("\n");

    a = decimal(&e, 3.14f);
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_DECIMAL);
    TL_TEST(a->decimal == 3.14f);
    print(a);
    printf("\n");

    a = symbol(&e, tstr_("mysym"));
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_SYMBOL);
    TL_TEST(tstr_equal(&a->symbol, &tstr_const("mysym")));
    print(a);
    printf("\n");

    a = string(&e, tstr_("my string"));
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_STRING);
    TL_TEST(tstr_equal(&a->symbol, &tstr_const("my string")));
    print(a);
    printf("\n");

    Env_destroy(&e);
}

void test_print(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    expr* r = cons(&e,
                   csymbol(&e, "print"),
                   cons(&e,
                        real(&e, 20),
                        NULL
                       )
        );
    print(r);
    printf("\n");

    r = cons(&e,
             csymbol(&e, "*"),
             cons(&e,
                  real(&e, 2),
                  cons(&e,
                       cons(&e,
                            csymbol(&e, "+"),
                            cons(&e,
                                real(&e, 3),
                                cons(&e,
                                     real(&e, 4),
                                     NULL)
                                )
                           ),
                       NULL)
                 )
        );
    print(r);
    printf("\n");

    expr* a = cons(&e,
                   csymbol(&e, "inc"),
                   cons(&e,
                        real(&e, 20),
                        NULL
                       )
        );
    expr* b = cons(&e,
                   decimal(&e, 3.571f),
                   cons(&e,
                        real(&e, -21),
                        NULL
                       )
        );
    r = cons(&e,
             csymbol(&e, "+"),
             cons(&e, a, cons(&e, b, NIL()))
        );

    print(r);
    printf("\n");
}

void
test_str2expr(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);
    read_verbose(&e, "(\t write  3.14159)");
    read_verbose(&e, "(write  ( + 2 10))");
    read_verbose(&e, "(print  ( + 2 hi))");
    read_verbose(&e, "\n(+ \t 2     5 \n  15.432 \n)");
    read_verbose(&e, "(write      \"Hello, World!\")");

    read_verbose(&e, "(- (+ 3 4) 5 )");
    read_verbose(&e, "(+ (- 3 4) (* 3 5 6 (/ 2 1)) )");
    Env_destroy(&e);
}

void
test_eval(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);
    Env_destroy(&e);
}

void
test_accessors(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

        TL_TEST(read_eval_print_verbose(&e,
                    "(quote (1 2 3 4))",
                    "(1 2 3 4)"));
        TL_TEST(read_eval_print_verbose(&e,
                    "(+ 1 2)",
                    "(3)"));
        TL_TEST(read_eval_print_verbose(&e,
                    "(- 7 3)",
                    "(4)"));
        TL_TEST(read_eval_print_verbose(&e,
                    "(+ 2 (- 5 6) 1)",
                    "(2)"));

        TL_TEST(read_eval_print_verbose(&e,
                    "(car (quote (1 2 3 4)))",
                    "(1)"));
        TL_TEST(read_eval_print_verbose(&e,
                    "(cdr (quote (1 2 3 4)))",
                    "(2 3 4)"));
        TL_TEST(read_eval_print_verbose(&e,
                    "(cons (quote (1)) (quote (2)))",
                    "((1) (2))"));
        TL_TEST(read_eval_print_verbose(&e,
                    "(first (quote (1 2 3 4)))",
                    "(1)"));
        TL_TEST(read_eval_print_verbose(&e,
                    "(second (quote ((1 2) (3 4) (5 6)) ))",
                    "(3 4)"));

    Env_destroy(&e);
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	TL(test_datatypes());
	TL(test_print());
	TL(test_str2expr());
	TL(test_accessors());

    tl_summary();

	return 0;
}
