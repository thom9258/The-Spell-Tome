#include "testlib.h"
#include "../yal.h"

#define PRINT_IF_ERROR(MSGPTR) \
    if (USERMSG_IS_ERROR((MSGPTR))) printf("USER ERROR:\n\t%s\n", (MSGPTR)->info)

char
lex_test(Environment* _e, char* _p, char* _expected)
{
    expr* prediction;
    ASSERT_INV_ENV(_e);
    read(_e, &prediction, _p);
    tstr lexed = stringifyexpr(prediction);
    tstr expected = tstr_view(_expected);
    printf("LEXED:    %s\n", lexed.c_str);
    printf("EXPECTED: %s\n", expected.c_str);
    return tstr_equal(&lexed, &expected);
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

char
repl_check(Environment* _e, char* _p, char* _gt)
{
    usermsg msg;
    expr* program;
    char are_equal = 0;
    expr* result;
    tstr gt_str = tstr_view(_gt);
    tstr result_str;
    ASSERT_INV_ENV(_e);
    msg = read(_e, &program, _p);
    PRINT_IF_ERROR(&msg);
    printf("YAL> "); printexpr(program); printf("\n");
    msg = eval(_e, &result, program);
    PRINT_IF_ERROR(&msg);
    result_str = stringifyexpr(result);
    printf("EXPECTED: %s\nGOT: %s\n", gt_str.c_str, result_str.c_str);
    are_equal = tstr_equal(&result_str, &gt_str);
    tstr_destroy(&result_str);
    return are_equal;
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
    tstr tmp; 

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
    tmp = tstr_view("mysym");
    TL_TEST(tstr_equal(&a->symbol, &tmp));
    printexpr(a);
    printf("\n");

    a = string(&e, tstr_("my string"));
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_STRING);
    tmp = tstr_view("my string");
    TL_TEST(tstr_equal(&a->symbol, &tmp));
    printexpr(a);
    printf("\n");

    Env_destroy(&e);
}

void test_print_manual(void)
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
test_lex(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);
    TL_TEST(lex_test(&e, "(\t write  3.141592)", "(write 3.141592)"));
    TL_TEST(lex_test(&e, "(write  ( + 2 10))", "(write (+ 2 10))"));
    TL_TEST(lex_test(&e, "(write  ( + 2    hi) )", "(write (+ 2 hi))"));
    TL_TEST(lex_test(&e, "\n(+ \t 2     5 \n  15.432 \n)",  "(+ 2 5 15.432000)"));
    TL_TEST(lex_test(&e, "(write      \"Hello, World!\")",  "(write \"Hello, World!\")"));

    TL_TEST(lex_test(&e, "(+ (- (range -1 (- 17 4)) M:PI) (* 9 2 7 foo bar baz))",
                         "(+ (- (range -1 (- 17 4)) M:PI) (* 9 2 7 foo bar baz))"));

    TL_TEST(lex_test(&e, "(- (+ 3 4) 5 )",  "(- (+ 3 4) 5)"));
    TL_TEST(lex_test(&e, "(+ (- 3 4) (* 3 5 6 (/ 2 1)) )",  "(+ (- 3 4) (* 3 5 6 (/ 2 1)))"));
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
    tstr res = tstr_view("(print 20)");
    printf("res = %s\n", res.c_str);
    printf("stringified = %s\n", s.c_str);
    TL_TEST(tstr_equal(&s, &res));
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

    TL_TEST(repl_check(&e,
                       "(quote (1 2 3 4))",
                       "(1 2 3 4)"));
    TL_TEST(repl_check(&e,
                       "(quote (foo bar (quote (baz))))",
                       "(foo bar (quote (baz)))"));
    TL_TEST(repl_check(&e,
                       "(quote ()  )",
                       "NIL"));

    Env_destroy(&e);
}

void
test_buildin_range(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_check(&e,
                       "(range 2 5)",
                       "(2 3 4)"));

    TL_TEST(repl_check(&e,
                       "(range (+ -1 2) 4)",
                       "(1 2 3)"));


    Env_destroy(&e);
}

void
test_buildin_math(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_check(&e,
                                    "(+ 1 2)",
                                    "3"));
    TL_TEST(repl_check(&e,
                                    "(- 7 3)",
                                    "4"));
    TL_TEST(repl_check(&e,
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

    TL_TEST(repl_check(&e,
                       "(car (quote (1 2 3 4)))",
                       "1"));

    TL_TEST(repl_check(&e,
                       "(cdr (quote (1 2 3 4)))",
                       "(2 3 4)"));

    TL_TEST(repl_check(&e,
                       "(cons 1 2)",
                       "(1 . 2)"));

    TL_TEST(repl_check(&e,
                       "(cons (quote (a b c)) 3)",
                       "(1 . 2)"));

    TL_TEST(repl_check(&e,
                       "(first (quote (1 2 3 4)))",
                       "(1)"));

    TL_TEST(repl_check(&e,
                       "(second (quote ((1 2) (3 4) (5 6)) ))",
                       "(3 4)"));

    Env_destroy(&e);
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	TL(test_datatypes());
	TL(test_print_manual());
	TL(test_lex());
	//TL(test_str2expr());
	//TL(test_buildin_quote());
	TL(test_buildin_range());
	//TL(test_buildin_math());
	//TL(test_buildin_accessors());
    tl_summary();

	return 0;
}
