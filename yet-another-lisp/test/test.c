#include "testlib.h"
#include "../yal.h"

#define PRINT_IF_ERROR(MSGPTR) \
    if (USERMSG_IS_ERROR((MSGPTR))) printf("USER ERROR:\n\t%s\n", (MSGPTR)->info)

char
lex_test(VariableScope* _scope, char* _p, char* _expected)
{
    yal_Msg msg = {0};
    expr* result;
    ASSERT_INV_SCOPE(_scope);
    result = read(_scope, &msg, _p);
    if (yal_is_error(&msg))
        return 0;
    tstr lexed = stringifyexpr(result);
    tstr expected = tstr_view(_expected);
    printf("LEXED:    %s\n", lexed.c_str);
    printf("EXPECTED: %s\n", expected.c_str);
    yal_msg_destroy(&msg);
    return tstr_equal(&lexed, &expected);
}

expr*
read_eval_print(VariableScope* _scope, yal_Msg* _msgdst, char* _p)
{
    expr* read_result;
    expr* eval_result;
    ASSERT_INV_SCOPE(_scope);
    read_result = read(_scope, _msgdst, _p);
    if (yal_is_error(_msgdst))
        return read_result;
    eval_result = eval(_scope, _msgdst, read_result);
    if (yal_is_error(_msgdst))
        return eval_result;
    DBPRINT("\n", eval_result);
    return eval_result;
}

char
repl_check(VariableScope* _scope, char* _p, char* _gt)
{
    char are_equal = 0;
    tstr gt_str = tstr_view(_gt);
    tstr result_str;
    yal_Msg msg = {0};
    expr* result = read_eval_print(_scope, &msg, _p);
    if (yal_is_error(&msg))
        printf("yal> ERROR: %s\n", msg.msg.c_str);
    yal_msg_destroy(&msg);

    result_str = stringifyexpr(result);
    printf("EXPECTED: %s\n"
           "GOT:      %s\n",
           gt_str.c_str, result_str.c_str);

    are_equal = tstr_equal(&result_str, &gt_str);
    tstr_destroy(&result_str);
    return are_equal;
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

    a = cons(&e.global, NIL(), NIL());
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_CONS);
    printexpr(a);
    printf("\n");

    a = cons(&e.global, real(&e.global, 4), NIL());
    TL_TEST(!is_nil(a));
    TL_TEST(!is_nil(car(a)));
    TL_TEST(car(a)->type == TYPE_REAL);
    TL_TEST(is_nil(cdr(a)));
    printexpr(a);
    printf("\n");

    a = real(&e.global, 3);
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_REAL);
    TL_TEST(a->real == 3);
    printexpr(a);
    printf("\n");

    a = decimal(&e.global, 3.14f);
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_DECIMAL);
    TL_TEST(a->decimal == 3.14f);
    printexpr(a);
    printf("\n");

    a = symbol(&e.global, "mysym");
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_SYMBOL);
    tmp = tstr_view("mysym");
    TL_TEST(tstr_equal(&a->symbol, &tmp));
    printexpr(a);
    printf("\n");

    a = string(&e.global, "my string");
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

    expr* r = cons(&e.global,
                   symbol(&e.global, "print"),
                   cons(&e.global,
                        real(&e.global, 20),
                        NULL
                       )
        );
    printexpr(r);
    printf("\n");

    r = cons(&e.global,
             symbol(&e.global, "mydottedlistvar"),
             real(&e.global, 22)
        );
    printexpr(r);
    printf("\n");

    r = cons(&e.global,
             symbol(&e.global, "*"),
             cons(&e.global,
                  real(&e.global, 2),
                  cons(&e.global,
                       cons(&e.global,
                            symbol(&e.global, "+"),
                            cons(&e.global,
                                real(&e.global, 3),
                                cons(&e.global,
                                     real(&e.global, 4),
                                     NULL)
                                )
                           ),
                       NULL)
                 )
        );
    printexpr(r);
    printf("\n");

    expr* a = cons(&e.global,
                   symbol(&e.global, "inc"),
                   cons(&e.global,
                        real(&e.global, 20),
                        NULL
                       )
        );
    expr* b = cons(&e.global,
                   decimal(&e.global, 3.571f),
                   cons(&e.global,
                        real(&e.global, -21),
                        NULL
                       )
        );
    r = cons(&e.global,
             symbol(&e.global, "+"),
             cons(&e.global, a, cons(&e.global, b, NIL()))
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
    TL_TEST(lex_test(&e.global, "(\t write  3.141592)", "(write 3.141592)"));
    TL_TEST(lex_test(&e.global, "(write  ( + 2 10))", "(write (+ 2 10))"));
    TL_TEST(lex_test(&e.global, "()", "(NIL)"));
    TL_TEST(lex_test(&e.global, "( 2 NIL )", "(2 NIL)"));
    TL_TEST(lex_test(&e.global, "(write  ( + 2    hi) )", "(write (+ 2 hi))"));


    TL_TEST(lex_test(&e.global, "\n(+ \t 2     5 \n  15.432 \n)",  "(+ 2 5 15.432000)"));
    //TL_TEST(lex_test(&e.global, "(write      \"Hello, World!\")",  "(write \"Hello, World!\")"));

    TL_TEST(lex_test(&e.global,
                     "(+ (- (range -1 (- 17 4)) M:PI) (* 9 2 7 foo bar baz))",
                     "(+ (- (range -1 (- 17 4)) M:PI) (* 9 2 7 foo bar baz))"));

    TL_TEST(lex_test(&e.global, "(- (+ 3 4) 5 )",  "(- (+ 3 4) 5)"));
    TL_TEST(lex_test(&e.global,
                     "(+ (- 3 4) (* 3 5 6 (/ 2 1)) )",
                     "(+ (- 3 4) (* 3 5 6 (/ 2 1)))"));


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

    TL_TEST(repl_check(&e.global,
                       "(quote (foo bar (quote (baz))))",
                       "(foo bar (quote (baz)))"));

    TL_TEST(repl_check(&e.global,
                       "(quote (1 2 3 4))",
                       "(1 2 3 4)"));
    
    TL_TEST(repl_check(&e.global,
                       "(quote ()  )",
                       "(NIL)"));

    Env_destroy(&e);
}

void
test_buildin_range(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_check(&e.global,
                       "(range 2 5)",
                       "2 3 4"));

    TL_TEST(repl_check(&e.global,
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

    TL_TEST(repl_check(&e.global, "(+ 1 2)", "3"));
    TL_TEST(repl_check(&e.global, "(- 7 3)", "4"));
    TL_TEST(repl_check(&e.global, "(+ 2 (- 5 6) 1)", "2"));
    Env_destroy(&e);
}

void
test_buildin_accessors(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_check(&e.global,
                       "(car (quote (1 2 3 4)))",
                       "1"));

    TL_TEST(repl_check(&e.global,
                       "(cdr (quote (1 2 3 4)))",
                       "(2 3 4)"));

    TL_TEST(repl_check(&e.global,
                       "(cons 1 2)",
                       "(1 . 2)"));

    TL_TEST(repl_check(&e.global,
                       "(cons (quote (a b c)) 3)",
                       "((a b c) . 3)"));

    TL_TEST(repl_check(&e.global,
                       "(first (quote (1 2 3 4)))",
                       "(1)"));

    TL_TEST(repl_check(&e.global,
                       "(second (quote ((1 2) (3 4) (5 6)) ))",
                       "(3 4)"));

    Env_destroy(&e);
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	TL(test_datatypes());
	//TL(test_print_manual());
	TL(test_lex());
	TL(test_buildin_quote());
	TL(test_buildin_range());
	//TL(test_buildin_math());
	//TL(test_buildin_accessors());
    tl_summary();

	return 0;
}
