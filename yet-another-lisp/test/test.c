#include "testlib.h"
#include "../yal.h"

#define PRINT_IF_ERROR(MSGPTR) \
    if (USERMSG_IS_ERROR((MSGPTR))) printf("USER ERROR:\n\t%s\n", (MSGPTR)->info)

expr*
read_verbose(Environment* _e, char* _p)
{
    expr* prediction;
    ASSERT_INV_ENV(_e);
    printf("string: %s\n", _p);
    read(_e, &prediction, _p);
    printf("lexed:  "); printexpr(prediction); printf("\n");
    return prediction;
}

expr*
read_eval_print(Environment* _e, char* _p)
{
    usermsg msg;
    expr* program;
    expr* result;
    ASSERT_INV_ENV(_e);
    msg = read(_e, &program, _p);
    PRINT_IF_ERROR(&msg);
    msg = eval(_e, &result, program);
    PRINT_IF_ERROR(&msg);
    printexpr(result); printf("\n");
    return result;
}

expr*
read_eval_print_verbose(Environment* _e, char* _p, char* _gt)
{
    usermsg msg;
    expr* program;
    expr* result;
    ASSERT_INV_ENV(_e);
    msg = read(_e, &program, _p);
    PRINT_IF_ERROR(&msg);
    printf("YAL> "); printexpr(program); printf("\n");
    msg = eval(_e, &result, program);
    PRINT_IF_ERROR(&msg);
    printf("EXPECTED> "); printf("%s\n", _gt);
    printf("RES> "); printexpr(result); printf("\n");
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
    printexpr(a);
    printf("\n");

    a = cons(&e, NIL(), NIL());
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_CONS);
    printexpr(a);
    printf("\n");

    a = cons(&e, real(&e, 4), NIL());
    TL_TEST(!is_nil(a));
    TL_TEST(!is_nil(car(a)));
    TL_TEST(car(a)->type == TYPE_REAL);
    TL_TEST(is_nil(cdr(a)));
    printexpr(a);
    printf("\n");

    a = real(&e, 3);
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_REAL);
    TL_TEST(a->real == 3);
    printexpr(a);
    printf("\n");

    a = decimal(&e, 3.14f);
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_DECIMAL);
    TL_TEST(a->decimal == 3.14f);
    printexpr(a);
    printf("\n");

    a = symbol(&e, tstr_("mysym"));
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_SYMBOL);
    TL_TEST(tstr_equal(&a->symbol, &tstr_const("mysym")));
    printexpr(a);
    printf("\n");

    a = string(&e, tstr_("my string"));
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_STRING);
    TL_TEST(tstr_equal(&a->symbol, &tstr_const("my string")));
    printexpr(a);
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
    printexpr(r);
    printf("\n");

    r = cons(&e,
             csymbol(&e, "mydottedlistvar"),
             real(&e, 22)
        );
    printexpr(r);
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
    printexpr(r);
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

    printexpr(r);
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
test_str2expr2str(void)
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
    tstr s = stringifyexpr(r);
    printf("stringified = %s\n", s.c_str);
    tstr_destroy(&s);

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
test_buildin_quote(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(read_eval_print_verbose(&e,
                                    "(quote (1 2 3 4))",
                                    "(1 2 3 4)"));
    TL_TEST(read_eval_print_verbose(&e,
                                    "(quote (foo bar (quote (baz))))",
                                    "(foo bar (quote (baz)))"));
    TL_TEST(read_eval_print_verbose(&e,
                                    "(quote ()  )",
                                    "NIL"));

    Env_destroy(&e);
}


void
test_buildin_math(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(read_eval_print_verbose(&e,
                                    "(+ 1 2)",
                                    "3"));
    TL_TEST(read_eval_print_verbose(&e,
                                    "(- 7 3)",
                                    "4"));
    TL_TEST(read_eval_print_verbose(&e,
                                    "(+ 2 (- 5 6) 1)",
                                    "2"));
    Env_destroy(&e);
}

void
test_buildin_accessors(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(read_eval_print_verbose(&e,
                                    "(car (quote (1 2 3 4)))",
                                    "1"));

    TL_TEST(read_eval_print_verbose(&e,
                                    "(cdr (quote (1 2 3 4)))",
                                    "(2 3 4)"));

    TL_TEST(read_eval_print_verbose(&e,
                                    "(cons 1 2)",
                                    "(1 . 2)"));

    TL_TEST(read_eval_print_verbose(&e,
                                    "(cons (quote (a b c)) 3)",
                                    "(1 . 2)"));

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
	TL(test_str2expr2str());
	TL(test_str2expr());
	TL(test_buildin_quote());
	TL(test_buildin_math());
	TL(test_buildin_accessors());

    tl_summary();

	return 0;
}
