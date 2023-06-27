#include "testlib.h"
#include "../yal.h"

#define PRINT_IF_ERROR(MSGPTR) \
    if (USERMSG_IS_ERROR((MSGPTR))) printf("USER ERROR:\n\t%s\n", (MSGPTR)->info)

char
read_test(Environment* _env, VariableScope* _scope, char* _p, char* _expected)
{
    Exception msg = {0};
    expr* result;
    ASSERT_INV_SCOPE(_scope);
    result = read(_env, _scope, &msg, _p);
    if (Exception_is_error(&msg))
        return 0;
    tstr lexed = stringify(result, "(", ")");
    tstr expected = tstr_view(_expected);
    printf("GT:    %s\n", expected.c_str);
    printf("LEXED: %s\n", lexed.c_str);
    Exception_destroy(&msg);
    return tstr_equal(&lexed, &expected);
}

char
repl_test(Environment* _env, VariableScope* _scope, char* _p, char* _gt)
{
    char are_equal = 0;
    tstr gt_str = tstr_view(_gt);
    tstr read_str = {0};
    tstr eval_str = {0};
    expr* read_result;
    expr* eval_result;
    Exception msg = {0};
    //expr* result = read_eval_print(_scope, &msg, _p);

    ASSERT_INV_SCOPE(_scope);
    read_result = read(_env, _scope, &msg, _p);
    if (Exception_is_error(&msg)) {
        printf("yal> ERROR: %s\n", msg.msg.c_str);
        return 0;
    }
    Exception_destroy(&msg);

    eval_result = eval(_env, _scope, &msg, read_result);
    if (Exception_is_error(&msg)) {
        printf("yal> ERROR: %s\n", msg.msg.c_str);
        return 0;
   }
    Exception_destroy(&msg);

    read_str = stringify(read_result, "(", ")");
    eval_str = stringify(eval_result, "(", ")");

    printf("GT:   %s\n", _gt);
    printf("READ: %s\n", read_str.c_str);
    printf("EVAL: %s\n", eval_str.c_str);

    are_equal = tstr_equal(&eval_str, &gt_str);
    tstr_destroy(&read_str);
    tstr_destroy(&eval_str);
    return are_equal;
}


tstr new_stringify(expr* _args, char* _open, char* _close);

char
expr_compare(expr* _p, char* _gt)
{
    tstr result_str = stringify(_p, "(", ")");
    printf("GT:          %s\n", _gt);
    printf("STRINGIFIED: %s\n", result_str.c_str);
    char eq = tstr_equalc(&result_str, _gt);
    tstr_destroy(&result_str);
    return eq;
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

    a = symbol(&e, "mysym");
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_SYMBOL);
    tmp = tstr_view("mysym");
    TL_TEST(tstr_equal(&a->symbol, &tmp));
    printexpr(a);
    printf("\n");

    a = string(&e, "my string");
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
                   symbol(&e, "print"),
                   cons(&e,
                        real(&e, 20),
                        NULL
                       )
        );

    TL_TEST(expr_compare(r, "(print 20)"));
    printf("\n");

    r = cons(&e,
             symbol(&e, "mydottedlistvar"),
             real(&e, 22)
        );
    TL_TEST(expr_compare(r, "(mydottedlistvar . 22)"));

    r = cons(&e,
             symbol(&e, "*"),
             cons(&e,
                  real(&e, 2),
                  cons(&e,
                       cons(&e,
                            symbol(&e, "+"),
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
    TL_TEST(expr_compare(r, "(* 2 (+ 3 4))"));

    expr* a = cons(&e,
                   symbol(&e, "inc"),
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
             symbol(&e, "+"),
             cons(&e, a, cons(&e, b, NIL()))
        );

    TL_TEST(expr_compare(r, "(+ (inc 20) (3.571000 -21))"));
    printf("\n");

    r = real(&e, 24);
    TL_TEST(expr_compare(r, "24"));
    printf("\n");

}

void
test_lex(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);
    TL_TEST(read_test(&e, &e.global, "(\t write  3.141592)", "(write 3.141592)"));
    TL_TEST(read_test(&e, &e.global, "(write  ( + 2 10))", "(write (+ 2 10))"));
    TL_TEST(read_test(&e, &e.global, "()", "(NIL)"));
    TL_TEST(read_test(&e, &e.global, "( 2 NIL )", "(2 NIL)"));
    TL_TEST(read_test(&e, &e.global, "(write  ( + 2    hi) )", "(write (+ 2 hi))"));


    TL_TEST(read_test(&e, &e.global, "\n(+ \t 2     5 \n  15.432 \n)",  "(+ 2 5 15.432000)"));
    //TL_TEST(read_test(&e.global, "(write      \"Hello, World!\")",  "(write \"Hello, World!\")"));

    TL_TEST(read_test(&e, &e.global,
                     "(+ (- (range -1 (- 17 4)) M:PI) (* 9 2 7 foo bar baz))",
                     "(+ (- (range -1 (- 17 4)) M:PI) (* 9 2 7 foo bar baz))"));

    TL_TEST(read_test(&e, &e.global, "(- (+ 3 4) 5 )",  "(- (+ 3 4) 5)"));
    TL_TEST(read_test(&e, &e.global,
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
test_buildin_list(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.global,
                       "(list 1 2 3 )",
                       "(1 2 3)"));

    TL_TEST(repl_test(&e, &e.global,
                       "(list 9.56700 -2.123000 4.345000)",
                       "(9.56700 -2.123000 4.345000)"));

    TL_TEST(repl_test(&e, &e.global,
                       "(list 1 (+ 1 1) 3 )",
                       "(1 2 3)"));

    TL_TEST(repl_test(&e, &e.global,
                       "(list 1 (+ -1 1 3) 3 )",
                       "(1 3 3)"));


    Env_destroy(&e);
}

void
test_list_management(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.global,
                       "(len (list 1 2 3 4))",
                       "4"));

    TL_TEST(repl_test(&e, &e.global,
                       "(put 0 (list 1 2 3))",
                       "(0 1 2 3)"));

    TL_TEST(repl_test(&e, &e.global,
                       "(pop (list 1 2 3))",
                       "1"));



    Env_destroy(&e);
}

void
test_buildin_quote(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.global,
                       "(quote (foo bar (quote (baz))))",
                       "(foo bar (quote (baz)))"));

    TL_TEST(repl_test(&e, &e.global,
                       "(quote (1 2 3 4))",
                       "(1 2 3 4)"));
    
    TL_TEST(repl_test(&e, &e.global,
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

    TL_TEST(repl_test(&e, &e.global,
                       "(range 2 5)",
                       "(2 3 4)"));

    TL_TEST(repl_test(&e, &e.global,
                       "(range (+ -1 2) 4)",
                       "(1 2 3)"));
    Env_destroy(&e);
}

void
test_buildin_accessors(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.global,
                       "(car (quote (1 2 3 4)))",
                       "1"));

    TL_TEST(repl_test(&e, &e.global,
                       "(cdr (quote (1 2 3 4)))",
                       "(2 3 4)"));

    TL_TEST(repl_test(&e, &e.global,
                       "(cons 1 2)",
                       "(1 . 2)"));

    TL_TEST(repl_test(&e, &e.global,
                       "(cons (quote (a b c)) 3)",
                       "((a b c) . 3)"));

    TL_TEST(repl_test(&e, &e.global,
                       "(first (quote (1 2 3 4)))",
                       "1"));

    TL_TEST(repl_test(&e, &e.global,
                       "(second (quote ((1 2) (3 4) (5 6)) ))",
                       "(3 4)"));

    TL_TEST(repl_test(&e, &e.global,
                       "(third (quote (1 2 3 4)))",
                       "3"));

    TL_TEST(repl_test(&e, &e.global,
                       "(fourth (quote (1 2 3 4)))",
                       "4"));

    Env_destroy(&e);
}

void
test_buildin_math(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.global, "(+)", "0"));
    TL_TEST(repl_test(&e, &e.global, "(-)", "0"));
    TL_TEST(repl_test(&e, &e.global, "(*)", "1"));
    TL_TEST(repl_test(&e, &e.global, "(/)", "1"));

    TL_TEST(repl_test(&e, &e.global, "(+ 1 2)", "3"));
    TL_TEST(repl_test(&e, &e.global, "(- 7 3)", "4"));
    TL_TEST(repl_test(&e, &e.global, "(* 4 3)", "12"));
    TL_TEST(repl_test(&e, &e.global, "(/ 8 2)", "4"));
    TL_TEST(repl_test(&e, &e.global, "(/ 0 2)", "0"));

    TL_TEST(repl_test(&e, &e.global, "(+ 2 (- 5 6) 1)", "2"));
    TL_TEST(repl_test(&e, &e.global, "(+ 4 (* 2 6) (- 10 5))", "21"));

    TL_TEST(repl_test(&e, &e.global, "(+ (range 0 10))", "55"));
    Env_destroy(&e);
}

void
test_buildin_equality(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.global, "(= 1 2)", "NIL"));
    TL_TEST(repl_test(&e, &e.global, "(= 7 3.431)", "NIL"));
    TL_TEST(repl_test(&e, &e.global, "(= 2 2)", "t"));
    TL_TEST(repl_test(&e, &e.global, "(= 2.34 2.34)", "t"));
    TL_TEST(repl_test(&e, &e.global, "(= 7)", "t"));
    TL_TEST(repl_test(&e, &e.global, "(=)", "t"));
    TL_TEST(repl_test(&e, &e.global, "(= (+ 2 2) 4 (- 10 6) (* 2 2))", "t"));

    Env_destroy(&e);
}

void
test_variables(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.global, "(global pi 3.14)", "pi"));

    TL_TEST(Variables_len(&e.global.variables) == 1);
    Variable* v;
    int i;
    tstr pi = tstr_("pi");
    char found = 0;
    for (i = 0; i < Variables_len(&e.global.variables); i++) {
        v = Variables_peek(&e.global.variables, i);
        if (tstr_equal(&v->symbol, &pi)) {
            printf("found '%s'\n", v->symbol.c_str);
        }
    }

    TL_TEST(found == 1);

    //TL_TEST(repl_test(&e.global, "(pi)", "3.14"));
    TL_TEST(repl_test(&e, &e.global, "(global name)", "name"));
    //TL_TEST(repl_test(&e.global, "(name)", "NIL"));

    Env_destroy(&e);
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	TL(test_datatypes());
	TL(test_print_manual());
	TL(test_lex());
	TL(test_buildin_quote());
	TL(test_buildin_list());
	//TL(test_buildin_range());
	//TL(test_buildin_accessors());
	//TL(test_buildin_math());
	//TL(test_buildin_equality());
	//TL(test_variables());
    //TL(test_list_management());

    tl_summary();

	return 0;
}
