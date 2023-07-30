#include "testlib.h"
#include "../yal.h"

#define PRINT_IF_ERROR(MSGPTR) \
    if (USERMSG_IS_ERROR((MSGPTR))) printf("USER ERROR:\n\t%s\n", (MSGPTR)->info)

char
lex_type_test(Environment* _env, VariableScope* _scope, char* _p, const int _type)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    Exception msg = {0};
    expr* result;

    result = read(_env, _scope, &msg, _p);
    if (Exception_is_error(&msg)) {
        printf("YAL ERROR: %s\n", msg.msg.c_str);
    }
    Exception_destroy(&msg);
    DBPRINT("LEXED: ", result);
    printf("Expected type %s, got %s\n",
           type_to_str(_type),
           type_to_str(result->type));

    return _type == result->type;
}

char
exception_test(Environment* _env, VariableScope* _scope, char* _p, const int _exc)
{
    ASSERT_INV_ENV(_env);
    ASSERT_INV_SCOPE(_scope);
    Exception msg = {0};
    expr* result;
    char out;

    result = read(_env, _scope, &msg, _p);
    if (Exception_is_error(&msg)) {
        printf("YAL ERROR: %s\n", msg.msg.c_str);
    }
    Exception_destroy(&msg);
    result = eval_expr(_env, _scope, &msg, result);
    if (Exception_is_error(&msg)) {
        printf("yal> ERROR: %s\n", msg.msg.c_str);
   }
    out = (_exc == msg.type);
    DBPRINT("result = ", result);
    printf("Expected Exception %s, got %s\n",
           Exception_to_str(_exc),
           Exception_to_str(msg.type));
    Exception_destroy(&msg);
    return out;
}

char
read_test(Environment* _env, VariableScope* _scope, char* _p, char* _expected)
{
    Exception msg = {0};
    char eq = 0;
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
    eq = tstr_equal(&lexed, &expected);
    tstr_destroy(&lexed);
    tstr_destroy(&expected);
    return eq;
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

    ASSERT_INV_SCOPE(_scope);
    read_result = read(_env, _scope, &msg, _p);

    if (Exception_is_error(&msg)) {
        printf("yal> ERROR: %s\n", msg.msg.c_str);
    }
    Exception_destroy(&msg);

    eval_result = eval_expr(_env, _scope, &msg, read_result);
    if (Exception_is_error(&msg)) {
        printf("yal> ERROR: %s\n", msg.msg.c_str);
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

    tstr tmp; 
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

    Env_destroy(&e);
}


void
test_lex_types(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(lex_type_test(&e, &e.globals, "myvar", TYPE_SYMBOL));
    TL_TEST(lex_type_test(&e, &e.globals, "my/other-var_", TYPE_SYMBOL));
    TL_TEST(lex_type_test(&e, &e.globals, "2", TYPE_REAL));
    TL_TEST(lex_type_test(&e, &e.globals, "-2", TYPE_REAL));
    TL_TEST(lex_type_test(&e, &e.globals, "3.14", TYPE_DECIMAL));
    TL_TEST(lex_type_test(&e, &e.globals, "-3.14", TYPE_DECIMAL));
    TL_TEST(lex_type_test(&e, &e.globals, "9.56700", TYPE_DECIMAL));
    TL_TEST(lex_type_test(&e, &e.globals, "\"MY STRING!\"", TYPE_STRING));
    TL_TEST(lex_type_test(&e, &e.globals, "(1 2 3)", TYPE_CONS));

    Env_destroy(&e);
}



void
test_lex(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);
    TL_TEST(read_test(&e, &e.globals, "(\t write  3.141592)", "(write 3.141592)"));
    TL_TEST(read_test(&e, &e.globals, "(write  ( + 2 10))", "(write (+ 2 10))"));
    /*TODO: This is an error i think*/
    TL_TEST(read_test(&e, &e.globals, "()", "(NIL)"));
    TL_TEST(read_test(&e, &e.globals, "( 2 NIL )", "(2 NIL)"));
    TL_TEST(read_test(&e, &e.globals, "(write  ( + 2    hi) )", "(write (+ 2 hi))"));

    TL_TEST(read_test(&e, &e.globals, "\n(+ \t 2     5 \n  15.432 \n)",  "(+ 2 5 15.432000)"));
    //TL_TEST(read_test(&e.globals, "(write      \"Hello, World!\")",  "(write \"Hello, World!\")"));

    TL_TEST(read_test(&e, &e.globals,
                     "(+ (- (range -1 (- 17 4)) M:PI) (* 9 2 7 foo bar baz))",
                     "(+ (- (range -1 (- 17 4)) M:PI) (* 9 2 7 foo bar baz))"));

    TL_TEST(read_test(&e, &e.globals, "(- (+ 3 4) 5 )",  "(- (+ 3 4) 5)"));
    TL_TEST(read_test(&e, &e.globals,
                     "(+ (- 3 4) (* 3 5 6 (/ 2 1)) )",
                     "(+ (- 3 4) (* 3 5 6 (/ 2 1)))"));


    TL_TEST(read_test(&e, &e.globals, "(3 . 4)", "(3 . 4)"));
    TL_TEST(read_test(&e, &e.globals, "'(1 2 3)", "(quote (1 2 3))"));
    TL_TEST(read_test(&e, &e.globals, "('foo 2 3)", "((quote foo) 2 3)"));

    TL_TEST(read_test(&e, &e.globals, "[1 2 3]", "(list 1 2 3)"));
    TL_TEST(read_test(&e, &e.globals, "[1 [2 3]]", "(list 1 (list 2 3))"));


    TL_TEST(read_test(&e, &e.globals,
                      "(\"my\" \"list of\" \"strings\")",
                      "(\"my\" \"list of\" \"strings\")"));

    Env_destroy(&e);
}

void
test_errors(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(exception_test(&e, &e.globals, "[1 invalidsym]", EXCEPTION_SYMNOTFOUND));
    TL_TEST(exception_test(&e, &e.globals, "(invalidfunciton 1 2 3)", EXCEPTION_FNNOTFOUND));


    TL_TEST(exception_test(&e, &e.globals, "(range 5 2 3)", EXCEPTION_INVALIDINPUT));

    Env_destroy(&e);
}

void
test_buildin_list(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.globals,
                       "(list 1 2 3 )",
                       "(1 2 3)"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(list 2.4 -2 5 1)",
                       "(2.400000 -2 5 1)"));

    /*TODO: Not valid!*/
    /*
    TL_TEST(repl_test(&e, &e.globals,
                       "(list 2. .1)",
                       "(2.000000 0.100000)"));
    */

    TL_TEST(repl_test(&e, &e.globals,
                       "(list 9.566000 -2.123000 4.345000)",
                       "(9.566000 -2.123000 4.345000)"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(list 1 (+ 1 1) 3 )",
                       "(1 2 3)"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(list 3 (+ -1 1 3) (+ 1 1 1))",
                       "(3 3 3)"));


    Env_destroy(&e);
}

void
test_list_management(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.globals,
                       "(len (list 1 2 3 4))",
                       "4"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(put 0 (list 1 2 3))",
                       "(0 1 2 3)"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(put \"myname\" '(23 m))",
                       "(\"myname\" 23 m)"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(put nil '(t nil t))",
                       "(NIL T NIL T)"));



    Env_destroy(&e);
}

void
test_buildin_quote(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.globals,
                       "(quote (foo bar (quote (baz))))",
                       "(foo bar (quote (baz)))"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(quote (1 2 3 4))",
                       "(1 2 3 4)"));

     TL_TEST(repl_test(&e, &e.globals,
                       "'(1 2 '(3 4) (quote 5))",
                       "(1 2 (quote (3 4)) (quote 5))"));
    
    TL_TEST(repl_test(&e, &e.globals,
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

    TL_TEST(repl_test(&e, &e.globals,
                       "(range 2 5)",
                       "(2 3 4)"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(range (+ -1 2) 4)",
                       "(1 2 3)"));
    Env_destroy(&e);
}

void
test_buildin_cons(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.globals,
                       "(cons 1 2)",
                       "(1 . 2)"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(cons (+ 2 2) 5)",
                       "(4 . 5)"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(cons (list 2 2) 1)",
                       "((2 2) . 1)"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(cons (quote (a b c)) 3)",
                       "((a b c) . 3)"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(cons 1 '(2 3))",
                       "(1 2 3)"));

    Env_destroy(&e);
}


void
test_buildin_accessors(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.globals,
                       "(car (quote (1 2 3 4)))",
                       "1"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(cdr (quote (1 2 3 4)))",
                       "(2 3 4)"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(first (quote (1 2 3 4)))",
                       "1"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(second (quote ((1 2) (3 4) (5 6)) ))",
                       "(3 4)"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(third (quote (1 2 3 4)))",
                       "3"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(fourth (quote (1 2 3 4)))",
                       "4"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(nth 9 (range 0 20))",
                       "9"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(nth 17 (range 0 20))",
                       "17"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(nth 21 (range 0 20))",
                       "NIL"));


    Env_destroy(&e);
}

void
test_buildin_math(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.globals, "(+)", "0"));
    TL_TEST(repl_test(&e, &e.globals, "(-)", "0"));
    TL_TEST(repl_test(&e, &e.globals, "(*)", "1"));
    TL_TEST(repl_test(&e, &e.globals, "(/)", "1"));

    TL_TEST(repl_test(&e, &e.globals, "(+ 1 2)", "3"));
    TL_TEST(repl_test(&e, &e.globals, "(- 7 3)", "4"));
    TL_TEST(repl_test(&e, &e.globals, "(* 4 3)", "12"));
    TL_TEST(repl_test(&e, &e.globals, "(/ 8 2)", "4"));
    TL_TEST(repl_test(&e, &e.globals, "(/ 0 2)", "0"));

    TL_TEST(repl_test(&e, &e.globals, "(+ 2 (- 5 6) 1)", "2"));
    TL_TEST(repl_test(&e, &e.globals, "(+ 4 (* 2 6) (- 10 5))", "21"));

    TL_TEST(repl_test(&e, &e.globals, "(- 5 2)", "3"));
    /*TODO: This is not valid, needs an apply function for this*/
    //TL_TEST(repl_test(&e, &e.globals, "(+ (range 0 10))", "55"));

    TL_TEST(repl_test(&e, &e.globals, "(/ (- (+ 515 (* -87 311)) 296) 27)", "-994"));


    Env_destroy(&e);
}

void
test_buildin_equality(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.globals, "(= 1 2)", "NIL"));
    TL_TEST(repl_test(&e, &e.globals, "(= 'somesym 'somesym)", "T"));
    TL_TEST(repl_test(&e, &e.globals, "(= \"Hello!\" \"Hello!\")", "T"));
    TL_TEST(repl_test(&e, &e.globals, "(= \"1\" 1)", "NIL"));
    TL_TEST(repl_test(&e, &e.globals, "(= 7 3.431)", "NIL"));
    TL_TEST(repl_test(&e, &e.globals, "(= 2 2)", "T"));
    TL_TEST(repl_test(&e, &e.globals, "(= 2.34 2.34)", "T"));
    TL_TEST(repl_test(&e, &e.globals, "(= 7)", "T"));
    TL_TEST(repl_test(&e, &e.globals, "(=)", "T"));
    TL_TEST(repl_test(&e, &e.globals, "(= (+ 2 2) 4 (- 10 6) (* 2 2))", "T"));
    TL_TEST(repl_test(&e, &e.globals, "(= (1 2 3) (1 2 3))", "NIL"));

    TL_TEST(repl_test(&e, &e.globals, "(eq '(1 2 3) [1 2 3] (range 1 4))", "T"));
    TL_TEST(repl_test(&e, &e.globals, "(eq 4 (+ 2 2))", "T"));

    TL_TEST(repl_test(&e, &e.globals, "(equal 4 (+ 2 2))", "NIL"));
    TL_TEST(repl_test(&e, &e.globals, "(equal (1 2 3) (1 2 3) (1 2 3))", "T"));

    Env_destroy(&e);
}

void
test_global(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);


    TL_TEST(repl_test(&e, &e.globals, "PI", "3.141592"));
    TL_TEST(repl_test(&e, &e.globals, "E", "2.718280"));
    TL_TEST(repl_test(&e, &e.globals, "E-constant", "0.577210"));
    TL_TEST(repl_test(&e, &e.globals, "(global age 33)", "age"));

    TL_TEST(Variables_len(&e.globals.variables) == 1);
    Variable* v;
    int i;
    tstr pi = tstr_view("age");
    char found = 0;
    for (i = 0; i < Variables_len(&e.globals.variables); i++) {
        v = Variables_peek(&e.globals.variables, i);
        if (tstr_equal(&v->symbol, &pi)) {
            printf("found '%s'\n", v->symbol.c_str);
            found = 1;
        }
    }
    TL_TEST(found == 1);

    TL_TEST(repl_test(&e, &e.globals, "(global name)", "name"));
    TL_TEST(repl_test(&e, &e.globals, "name", "NIL"));
    TL_TEST(Variables_len(&e.globals.variables) == 2);
    TL_TEST(repl_test(&e, &e.globals, "(global thislist [1 2 3]", "thislist"));
    TL_TEST(repl_test(&e, &e.globals, "thislist", "(1 2 3)"));
    TL_TEST(repl_test(&e, &e.globals, "(global my/list [2 (+ 3 3)]", "my/list"));
    TL_TEST(repl_test(&e, &e.globals, "my/list", "(2 6)"));
    TL_TEST(repl_test(&e, &e.globals, "(global)", "NIL"));


    Env_destroy(&e);
}

void
test_variable_duplicate(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    Env_destroy(&e);
}

void
test_variables(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.globals, "(var player-name \"player1\")", "player-name"));
    TL_TEST(repl_test(&e, &e.globals, "(var health (+ 2 2))", "health"));
    TL_TEST(repl_test(&e, &e.globals, "(var weapons '(pistol lazergun shotgun))", "weapons"));

    TL_TEST(repl_test(&e, &e.globals, "player-name", "\"player1\""));
    TL_TEST(repl_test(&e, &e.globals, "health", "4"));
    TL_TEST(repl_test(&e, &e.globals, "weapons", "(pistol lazergun shotgun)"));

    TL_TEST(repl_test(&e, &e.globals, "(var thislist [1 2 3])", "thislist"));
    TL_TEST(repl_test(&e, &e.globals, "(var my/list [2 (+ 3 3)])", "my/list"));

    TL_TEST(repl_test(&e, &e.globals, "thislist", "(1 2 3)"));
    TL_TEST(repl_test(&e, &e.globals, "my/list", "(2 6)"));

    TL_TEST(repl_test(&e, &e.globals, "(var my/var 10)", "my/var"));
    TL_TEST(repl_test(&e, &e.globals, "(* my/var 2)", "20"));

    Env_destroy(&e);
}

void
test_set_variables(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.globals, "(const max-health 10)", "max-health"));
    TL_TEST(repl_test(&e, &e.globals, "(var health (+ 2 2))", "health"));
    TL_TEST(repl_test(&e, &e.globals, "(set 'health (- health 1))", "3"));
    TL_TEST(repl_test(&e, &e.globals, "health", "3"));
    TL_TEST(repl_test(&e, &e.globals, "(set 'health max-health)", "10"));
    TL_TEST(repl_test(&e, &e.globals, "(set 'health (- health 1))", "9"));
    /*TODO: This one has an error, it is sometimes settable!*/
    TL_TEST(repl_test(&e, &e.globals, "(set 'invalid-name 7)", "NIL"));
    TL_TEST(repl_test(&e, &e.globals, "(set 'health)", "NIL"));

    Env_destroy(&e);
}

void
test_buildin_do(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.globals,
                       "(do)",
                       "NIL"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(do (+ 2 3) (- 5 2))",
                       "3"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(do (+ 30 51) (* 10 3 2) (range 0 5) [1 2 3])",
                       "(1 2 3)"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(do 5)",
                       "5"));

    Env_destroy(&e);
}

void
test_lambda(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.globals,
                       "(lambda (a) a)",
                       "#<lambda>"));

    TL_TEST(repl_test(&e, &e.globals,
                       "((lambda (a) a) 12)",
                       "12"));

    TL_TEST(repl_test(&e, &e.globals,
                       "((lambda () 5))",
                       "5"));

    TL_TEST(repl_test(&e, &e.globals,
                       "((lambda () '(1 1 1)))",
                       "(1 1 1)"));

    TL_TEST(repl_test(&e, &e.globals,
                       "((lambda (n) [n 2 3]) 2)",
                       "(2 2 3)"));

    TL_TEST(repl_test(&e, &e.globals,
                       "((lambda (a) (+ a 10)) 4)",
                       "14"));

    TL_TEST(repl_test(&e, &e.globals,
                       "((lambda (b c) (+ b c)) 4 6)",
                       "10"));

     TL_TEST(repl_test(&e, &e.globals,
                       "((lambda (a b) (+ a b)) 10 20)",
                       "30"));
 
    TL_TEST(repl_test(&e, &e.globals,
                       "((lambda () (* 5 3)))",
                       "15"));

    TL_TEST(repl_test(&e, &e.globals,
                       "((lambda (a b) (* a b)) 4 6)",
                       "24"));
    TL_TEST(repl_test(&e, &e.globals,
                      "((lambda (a b c) "
                      "(var s (* a b))"
                      "(+ s c))"
                      "2 4 3)",
                      "11"));

    Env_destroy(&e);
}

void
test_fn(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.globals,
                       "(fn same (a) a)",
                       "same"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(same 12)",
                       "12"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(fn add2 (a b) (+ a b))",
                       "add2"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(add2 5 10)",
                       "15"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(add2 5 (+ 5 5))",
                       "15"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(fn move (this scale offset) (add2 offset (* this scale)))",
                       "move"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(move 5 10 3)",
                       "53"));

    TL_TEST(repl_test(&e, &e.globals,
                      "(fn with-docstring (a b) "
                      "\"This is my optional string for my function!\""
                      "(+ a b))",
                       "with-docstring"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(with-docstring 5 10)",
                       "15"));

    /*
    TL_TEST(repl_test(&e, &e.globals,
                      "(cusum (l h) "
                      "(var s (range l h))"
                      "(apply '+ s)",
                      "cusum"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(cusum 1 10)",
                       "55"));

     */
    Env_destroy(&e);
}

void
test_macros(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.globals,
                       "(macro setto10 (var) (set var 10))",
                       "setto10"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(var myvar 2)",
                       "myvar"));

    TL_TEST(repl_test(&e, &e.globals,
                       "myvar",
                       "2"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(setto10 myvar)",
                       "10"));

    TL_TEST(repl_test(&e, &e.globals,
                       "myvar",
                       "10"));

    TL_TEST(repl_test(&e, &e.globals,
                      "(macro setq (var val) (set 'var val))",
                      "setq"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(setq myvar 5)",
                       "5"));

    TL_TEST(repl_test(&e, &e.globals,
                       "myvar",
                       "5"));

    TL_TEST(repl_test(&e, &e.globals,
                      "(macro inc! (var) ['set 'var [('+ 1 var)]])",
                      "inc!"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(inc! myvar)",
                       "6"));

    Env_destroy(&e);
}

void
test_conditionals(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);

    TL_TEST(repl_test(&e, &e.globals,
                       "(if (= 2 2) 4)",
                       "4"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(if (= 2 2) 4 0)",
                       "4"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(if t 2)",
                       "2"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(if '(1 2 3) t)",
                       "T"));


    TL_TEST(repl_test(&e, &e.globals,
                       "(if nil 2 3)",
                       "3"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(if () t nil)",
                       "NIL"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(if (NIL) t nil)",
                       "NIL"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(if (= 2 3) t nil)",
                       "NIL"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(if (= 2 3) 1 0)",
                       "0"));

    TL_TEST(repl_test(&e, &e.globals,
                      "(fn is10 (v) "
                      "  (if (= v 10) t nil))",
                      "is10"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(is10 7)",
                       "NIL"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(is10 10)",
                       "T"));

    /*
    TL_TEST(repl_test(&e, &e.globals,
                      "(fn upto10 (a)"
                      "  (if (= a 10) "
                      "    (+ 1 (upto10 (+ a 1)))"
                      "    0"
                      "))",
                      "upto10"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(upto10 0)",
                       "10"));
     */

    Env_destroy(&e);
}

void
test_functions_and_recursion(void)
{
    Environment e;
    Env_new(&e);
    Env_add_core(&e);
    /*
(defun factorial (n)
  (if (= n 0)
      1
      (* n (factorial (- n 1))) ) )
    */

    TL_TEST(repl_test(&e, &e.globals,
                      "(fn factorial (v)"
                      "  (if (= v 0) "
                      "    1"
                      "    (* v (factorial (- v 1)))"
                      "))",
                      "factorial"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(factorial 5)",
                       "120"));
    TL_TEST(repl_test(&e, &e.globals,
                       "(factorial 3)",
                       "6"));
    TL_TEST(repl_test(&e, &e.globals,
                       "(factorial 10)",
                       "3628800"));


    TL_TEST(repl_test(&e, &e.globals,
                      "(fn list-len (list)"
                      "  (if list "
                      "    (+ 1 (list-len (cdr list)))"
                      "    -1"
                      "))",
                      "list-len"));

    TL_TEST(repl_test(&e, &e.globals,
                       "(list-len '(1 2 3))",
                       "3"));
    TL_TEST(repl_test(&e, &e.globals,
                       "(list-len [1 (+ 2 2) 3])",
                       "3"));
    TL_TEST(repl_test(&e, &e.globals,
                       "(list-len '(1 ((((3))))))",
                       "2"));

    TL_TEST(repl_test(&e, &e.globals,
                      "(fn isin (v l)"
                      "  (if l"
                      "     (if (eq (car l) v)"
                      "      t (isin (cdr v) l))"
                      "nil))",
                      "isin"));

    /*
    TL_TEST(repl_test(&e, &e.globals,
                       "(isin 3 '(1 2 3 4))",
                       "t"));

     TL_TEST(repl_test(&e, &e.globals,
                       "(isin 5 '(1 2 3 4))",
                       "nil"));
    */
    /*https://www.omnicalculator.com/math/binomial-coefficient*/
    TL_TEST(repl_test(&e, &e.globals,
                      "(fn bin-coeff (n k)"
                      "(/ (factorial n) (factorial (- n k)) (factorial k)"
                      ")",
                      "bin-coeff"));

     TL_TEST(repl_test(&e, &e.globals,
                       "(bin-coeff 5 2)",
                       "10"));

     TL_TEST(repl_test(&e, &e.globals,
                       "(bin-coeff 5 4)",
                       "5"));
     /*
     TL_TEST(repl_test(&e, &e.globals,
                       "(bin-coeff 22 13)",
                       "497.4200000"));
      */

    Env_destroy(&e);
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	TL(test_datatypes());
	TL(test_print_manual());
    TL(test_lex_types());
	TL(test_lex());
    TL(test_buildin_quote());
	TL(test_buildin_list());
	TL(test_buildin_accessors());
	TL(test_buildin_range());
    TL(test_list_management());
	TL(test_buildin_cons());
	TL(test_buildin_math());
    TL(test_variable_duplicate());
	TL(test_variables());
	TL(test_global());
	TL(test_errors());
	TL(test_buildin_do());
	TL(test_set_variables());
	TL(test_lambda());
	TL(test_fn());
	TL(test_conditionals());
	TL(test_buildin_equality());
    TL(test_functions_and_recursion());
	TL(test_macros());

    tl_summary();

	return 0;
}
