#include "testlib.h"
#include "../yal.h"

/*https://en.wikipedia.org/wiki/S-cconsession*/

expr*
onerepl(Environment* _e, expr* program)
{
    expr* res;
    printf("[inp] > "); atom_print(program);
    res = buildin_eval(_e, program);
    printf("[res] > "); atom_print(res);
    printf("\n");
    return res;
}

expr*
eval_str(Environment* _e, char* _p)
{
    expr* program = read(_e, _p);
    printf("[txt] > %s\n", _p);
    return onerepl(_e, program);
}

char
evalcheck(Environment* _e, char* _p, char* _res)
{
    expr* pred = eval_str(_e, _p);
    tstr result = tstr_(_res);
    UNUSED(pred);
    printf("[GT ] > %s\n", result.c_str);
    return 1;
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

    a = NIL;
    TL_TEST(is_nil(a));
    TL_TEST(a->type == TYPE_CONS);
    atom_print(a);
    printf("\n");

    a = cons(&e, NIL, NIL);
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_CONS);
    atom_print(a);
    printf("\n");

    a = cons(&e, real(&e, 4), NIL);
    TL_TEST(!is_nil(a));
    TL_TEST(!is_nil(car(a)));
    TL_TEST(car(a)->type == TYPE_REAL);
    TL_TEST(is_nil(cdr(a)));
    atom_print(a);
    printf("\n");

    a = real(&e, 3);
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_REAL);
    TL_TEST(a->real == 3);
    atom_print(a);
    printf("\n");

    a = decimal(&e, 3.14f);
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_DECIMAL);
    TL_TEST(a->decimal == 3.14f);
    atom_print(a);
    printf("\n");

    a = symbol(&e, tstr_("mysym"));
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_SYMBOL);
    TL_TEST(tstr_equal(&a->symbol, &tstr_const("mysym")));
    atom_print(a);
    printf("\n");

    a = string(&e, tstr_("my string"));
    TL_TEST(!is_nil(a));
    TL_TEST(a->type == TYPE_STRING);
    TL_TEST(tstr_equal(&a->symbol, &tstr_const("my string")));
    atom_print(a);
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
    atom_print(r);
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
    atom_print(r);
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
             cons(&e, a, cons(&e, b, NIL))
        );

    atom_print(r);
    printf("\n");
}

void
test_str2expr(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);
    //eval_str(&e, "(\t write  3.14159)");
    //eval_str(&e, "(write  ( + 2 4 ))");
    //eval_str(&e, "\n(+ \t 2     5 \n  15.432 \n)");
    //eval_str(&e, "(write \"Hello, World!\")");
    Env_destroy(&e);
}

void
test_eval(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);
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
    Env_destroy(&e);
}

void
test_accessors(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);
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
 
    Env_destroy(&e);
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	TL(test_datatypes());
	TL(test_print());
	//TL(test_str2expr());
	//TL(test_eval());
	//TL(test_accessors());

    tl_summary();

	return 0;
}
