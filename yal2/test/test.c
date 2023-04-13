#include "testlib.h"
#include "../buildins.h"
#include "../core.h"


/*https://en.wikipedia.org/wiki/S-cconsession*/

expr*
fakerepl(Environment* e, expr *r)
{
    printf("[inp] > ");
    buildin_print(e, r);
    printf("\n");
    r = buildin_eval(e, r);
    printf("[res] > ");
    buildin_print(e, r);
    printf("\n");
    return r;
}

/*
expr*
repltest(Environment* e, char* prog, char* res)
{

}
*/

void test_print(void)
{
    Environment e;
    Environment_new(&e);

    expr* r = cons(&e,
                   symbol(&e, "write"),
                   cons(&e,
                        real(&e, 20),
                        NULL
                       )
        );
    buildin_print(&e, r);
    printf("\n");

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
    buildin_print(&e, r);
    printf("\n");

    r = cons(&e,
             symbol(&e, "+"),
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

void
test_eval(void)
{
    Environment e;
    Environment_new(&e);
    Environment_add_buildins(&e);
    //char* prog = "write (quote ( + 4 (* 3 2 )) )";
    char* prog = "(write 3)";

    expr* r = cons(&e,
                    symbol(&e, "read"),
                    cons(&e,
                         string(&e, tstr_(prog)),
                         NULL
                        )
        );
    fakerepl(&e, r);
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	TL(test_print());
	//TL(test_buildin_math());
	//TL(test_buildin_write_car_cdr());
	TL(test_eval());

    tl_summary();

	return 0;
}
