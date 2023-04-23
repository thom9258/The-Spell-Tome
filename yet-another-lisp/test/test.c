#include "testlib.h"
#include "../yal.h"

/*https://en.wikipedia.org/wiki/S-cconsession*/

expr*
onerepl(Environment* _e, expr* program)
{
    expr* res;
    printf("[inp] > "); buildin_print(_e, program);
    res = buildin_eval(_e, program);
    printf("[res] > "); buildin_print(_e, res);
    printf("\n");
    return res;
}

expr*
eval_str(Environment* _e, char* _p)
{
    expr* program = str_read(_e, _p);
    printf("[txt] > %s\n", _p);
    return onerepl(_e, program);
}

char
evalcheck(Environment* _e, char* _p, char* _res)
{
    expr* pred = eval_str(_e, _p);
    tstr result = tstr_(_res);
    //printf("[res] > "); buildin_print(_e, pred); printf("\n");
    UNUSED(pred);
    printf("[GT ] > %s\n", result.c_str);
    return 1;
}

/*===============================================================*/

void test_print(void)
{
    Environment e;
    Environment_new(&e);

    expr* r = cons(&e,
                   csymbol(&e, "write"),
                   cons(&e,
                        real(&e, 20),
                        NULL
                       )
        );
    buildin_print(&e, r);
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
    buildin_print(&e, r);
    printf("\n");

    r = cons(&e,
             csymbol(&e, "+"),
             cons(&e,
                  real(&e, 20),
                  cons(&e,
                       real(&e, 5),
                       NULL
                      )
                 )
        );
    buildin_print(&e, r);
    printf("\n");
}

void
test_str2expr(void)
{
    Environment e;
    Environment_new(&e);
    Environment_add_core(&e);
    //eval_str(&e, "(\t write  3.14159)");
    //eval_str(&e, "(write  ( + 2 4 ))");
    //eval_str(&e, "\n(+ \t 2     5 \n  15.432 \n)");
    //eval_str(&e, "(write \"Hello, World!\")");
    Environment_destroy(&e);
}

void
test_eval(void)
{
    Environment e;
    Environment_new(&e);
    Environment_add_core(&e);
    expr* program = cons(&e,
                         csymbol(&e, "write"),
                         cons(&e,
                              real(&e, 20),
                              NULL
                             )
        );
    onerepl(&e, program);
    eval_str(&e, "(write 21)");
    eval_str(&e, "(quote (write 34))");
    //repl(&e, "(write (+ 2 2))");
    //repl(&e, "write (quote ( + 4 (* 3 2 )) )");
    //repl(&e, "(write \"Hello, World!\")");
    //repl(&e, "(+ (2 2))");
    Environment_destroy(&e);
}

void
test_accessors(void)
{
    Environment e;
    Environment_new(&e);
    Environment_add_core(&e);
    TL_TEST(evalcheck(&e,
                   "(car (1 2 3 4))",
                   "(1)"));
    TL_TEST(evalcheck(&e,
                   "(cdr (1 2 3 4))",
                   "(2 3 4)"));
    TL_TEST(evalcheck(&e,
                   "(cons (1) (2))",
                   "((1) (2))"));
    TL_TEST(evalcheck(&e,
                   "(first (1 2 3 4))",
                   "(1)"));
    TL_TEST(evalcheck(&e,
                   "(third (1 2 3 4))",
                   "(3)"));
    TL_TEST(evalcheck(&e,
                   "(seventh (1 2 3 4))",
                   "(NIL)"));
    /*TODO: Lexing or printing is wrong! this is not formatted correctly!*/
    TL_TEST(evalcheck(&e,
                   "(first ((1 2) (3 4)))",
                   "(1 2)"));
    TL_TEST(evalcheck(&e,
                   "(second ((1 2) (3 4) (5 6)))",
                   "(3 4)"));

    eval_str(&e, "(nth 0 (1 2 3 4) )");
    eval_str(&e, "(nth 1 (1 2 3 4) )");
    eval_str(&e, "(nth 2 (1 2 3 4) )");
    eval_str(&e, "(nth 4 (1 2 3 4) )");
    eval_str(&e, "(nth -1 (1 2 3 4) )");
 
    Environment_destroy(&e);
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	TL(test_print());
	TL(test_str2expr());
	TL(test_eval());
	TL(test_accessors());

    tl_summary();

	return 0;
}
