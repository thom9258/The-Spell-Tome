#include "testlib.h"
#include "../buildins.h"
#include "../core.h"


/*https://en.wikipedia.org/wiki/S-cconsession*/

expr*
exprrepl(Environment* _e, expr* program)
{
    expr* res;
    printf("[inp] > "); buildin_print(_e, program); printf("\n");
    res = buildin_eval(_e, program);
    printf("[res] > "); buildin_print(_e, res); printf("\n");
    return res;
}

expr*
repl(Environment* _e, char* _p)
{
    expr* program = str_read(_e, _p);
    return exprrepl(_e, program);
}

/*===============================================================*/

void
test_str2expr(void)
{
    Environment e;
    Environment_new(&e);
    Environment_add_buildins(&e);
    str_readp(&e, "(\t write  3.14159)");
    str_readp(&e, "(write  ( + 2 4 ))");
    str_readp(&e, "\n(+ \t 2     5 \n  15.432 \n)");
    str_readp(&e, "(write \"Hello, World!\")");
    Environment_destroy(&e);
}

void
test_eval(void)
{
    Environment e;
    Environment_new(&e);
    Environment_add_buildins(&e);
    expr* program = cons(&e,
                         csymbol(&e, "write"),
                         cons(&e,
                              real(&e, 20),
                              NULL
                             )
        );
    //UNUSED(program);
    exprrepl(&e, program);
    //repl(&e, "(write 20)");

    //repl(&e, "write (quote ( + 4 (* 3 2 )) )");
    //repl(&e, "(write \"Hello, World!\")");
    //repl(&e, "(+ (2 2))");
    Environment_destroy(&e);
}

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

#if 0
void
test_buildin_math(void)
{
    Environment e;
    Environment_new(&e);
    expr* r = cons(&e,
                    symbol(&e, "+"),
                    cons(&e,
                         real(&e, 20),
                         cons(&e,
                              real(&e, 5),
                              NULL
                             )
                        )
        );
    fakerepl(&e, r);

    r = cons(&e,
             symbol(&e, "*"),
             cons(&e,
                  real(&e, 2),
                  cons(&e,
                       cons(&e,
                            symbol(&e, "-"),
                            cons(&e,
                                real(&e, 3),
                                cons(&e,
                                     real(&e, 1),
                                     NULL)
                                )
                           ),
                       NULL)
                 )
        );
    fakerepl(&e, r);
}

void
test_buildin_write_car_cdr(void)
{
    Environment e;
    expr* r = cons(&e,
                    symbol(&e, "write"),
                    cons(&e,
                         real(&e, -15),
                         NULL
                        )
        );
    fakerepl(&e, r);

    
}
#endif

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	TL(test_print());
	TL(test_str2expr());
	TL(test_eval());
	//TL(test_buildin_math());
	//TL(test_buildin_write_car_cdr());

    tl_summary();

	return 0;
}
