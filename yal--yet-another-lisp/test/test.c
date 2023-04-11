#include "testlib.h"
#include "../yal.h"


#define YAL_PRINT_AST(env, ast) \
    do { \
    tstring_s _tmp = tstring("");                  \
    yal_AST2str(&env, ast, &_tmp);              \
    printf("%s\n", _tmp.c_str); \
    tstring_destroy(&_tmp);      \
 } while (0)

#define YAL_VISUAL_EVAL(env, root) \
    YAL_PRINT_AST(env, root);      \
    yal_eval(&env, root); \
    yal_flush_print_buffer(&env); \
    YAL_PRINT_AST(env, root);      \
    printf("\n");


#define YAL_QUICK_EVAL(internal)                    \
    do { \
    yal_Environment env = yal_env_new(100); \
    yal_env_add_buildins(&env); \
    internal \
    YAL_VISUAL_EVAL(env, root); \
    yal_env_delete(&env); \
} while (0)

#define YAL_RESCOMPARE(env, c_res, atom_res)     \
    TL_TEST(!yal_atom_equal(yal_atom(env, yal_atom_decimal_new(env, c_res)), yal_atom(env, atom_res)))

#define YAL_MATH_ENV_TEST(c_res, internal)                    \
    do { \
    yal_Environment env = yal_env_new(100); \
    yal_env_add_buildins(&env); \
    TL_PRINT("Expecting equation result to be %lf\n", (float)c_res);    \
    internal \
    YAL_VISUAL_EVAL(env, root); \
    YAL_RESCOMPARE(&env, c_res, root); \
    yal_env_delete(&env); \
} while (0)

void test_init_destroy(void)
{
    yal_Environment env = yal_env_new(0);
    TL_TEST(env.memory != NULL);
    yal_env_delete(&env);
}

void test_buildin_write(void)
{
    yal_AtomHandle root;
    YAL_QUICK_EVAL(
        root = yal_AST_new_va(&env, 2,
                              yal_atom_symbol_new(&env, "write"),
                              yal_atom_real_new(&env, 4)
            );
        );

    YAL_QUICK_EVAL(
        root = yal_AST_new_va(&env, 2,
                              yal_atom_symbol_new(&env, "write"),
                              yal_atom_decimal_new(&env, 3.1415)
            );
        );

    YAL_QUICK_EVAL(
        root = yal_AST_new_va(&env, 2,
                              yal_atom_symbol_new(&env, "write"),
                              yal_atom_string_new(&env, "Hello, World!")
            );
        );
}

void test_buildin_comparison_ops(void)
{
    yal_AtomHandle root;
    YAL_MATH_ENV_TEST( ( 2 == 2 ),
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, "=="),
                              yal_atom_real_new(&env, 2),
                              yal_atom_real_new(&env, 2)
            );
        );
    YAL_MATH_ENV_TEST( (3 == 5),
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, "=="),
                              yal_atom_real_new(&env, 3),
                              yal_atom_real_new(&env, 5)
            );
        );

    YAL_MATH_ENV_TEST( (2 != 2),
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, "!="),
                              yal_atom_real_new(&env, 2),
                              yal_atom_real_new(&env, 2)
            );
        );
    YAL_MATH_ENV_TEST( (3 != 5),
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, "!="),
                              yal_atom_real_new(&env, 3),
                              yal_atom_real_new(&env, 5)
            );
        );

    YAL_MATH_ENV_TEST( (2 < 2),
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, "<"),
                              yal_atom_real_new(&env, 2),
                              yal_atom_real_new(&env, 2)
            );
        );
    YAL_MATH_ENV_TEST( (3 < 5),
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, "<"),
                              yal_atom_real_new(&env, 3),
                              yal_atom_real_new(&env, 5)
            );
        );

    YAL_MATH_ENV_TEST( (2 > 2),
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, ">"),
                              yal_atom_real_new(&env, 2),
                              yal_atom_real_new(&env, 2)
            );
        );
    YAL_MATH_ENV_TEST( (3 > 5),
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, ">"),
                              yal_atom_real_new(&env, 3),
                              yal_atom_real_new(&env, 5)
            );
        );

    YAL_MATH_ENV_TEST( (2 >= 2),
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, ">="),
                              yal_atom_real_new(&env, 2),
                              yal_atom_real_new(&env, 2)
            );
        );
    YAL_MATH_ENV_TEST( (3 >= 5),
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, ">="),
                              yal_atom_real_new(&env, 3),
                              yal_atom_real_new(&env, 5)
            );
        );

    YAL_MATH_ENV_TEST( (2 <= 2),
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, "<="),
                              yal_atom_real_new(&env, 2),
                              yal_atom_real_new(&env, 2)
            );
        );
    YAL_MATH_ENV_TEST( (3 <= 5),
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, "<="),
                              yal_atom_real_new(&env, 3),
                              yal_atom_real_new(&env, 5)
            );
        );
}

void test_math_1(void)
{
    yal_AtomHandle root;
    YAL_MATH_ENV_TEST( (2 + 4 + 11 + -100),
        root = yal_AST_new_va(&env, 5,
                              yal_atom_symbol_new(&env, "+"),
                              yal_atom_real_new(&env, 2),
                              yal_atom_real_new(&env, 4),
                              yal_atom_real_new(&env, 11),
                              yal_atom_real_new(&env, -100)
            );
        );

    YAL_MATH_ENV_TEST( (-4.37 + 11.2 + 3.1415),
        root = yal_AST_new_va(&env, 4,
                              yal_atom_symbol_new(&env, "+"),
                              yal_atom_decimal_new(&env, -4.37),
                              yal_atom_decimal_new(&env, 11.2),
                              yal_atom_decimal_new(&env, 3.1415)
            
            );
        );

    YAL_MATH_ENV_TEST( (-13 + 11.2 + 300),
        root = yal_AST_new_va(&env, 4,
                              yal_atom_symbol_new(&env, "+"),
                              yal_atom_real_new(&env, -13),
                              yal_atom_decimal_new(&env, 11.2),
                              yal_atom_real_new(&env, 300)
            );
        );

    YAL_MATH_ENV_TEST( (-10),
        root = yal_AST_new_va(&env, 2,
                              yal_atom_symbol_new(&env, "-"),
                              yal_atom_real_new(&env, 10)
            );
        );

    YAL_MATH_ENV_TEST( (10),
        root = yal_AST_new_va(&env, 2,
                              yal_atom_symbol_new(&env, "-"),
                              yal_atom_real_new(&env, -10)
            );
        );

    YAL_MATH_ENV_TEST( (2 - 11),
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, "-"),
                              yal_atom_real_new(&env, 2),
                              yal_atom_real_new(&env, 11)
            );
        );

    YAL_MATH_ENV_TEST( (10 - -110),
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, "-"),
                              yal_atom_decimal_new(&env, 10),
                              yal_atom_real_new(&env, -110)
            );
        );

    YAL_MATH_ENV_TEST( (-4.37 - 11.2 - 3.1415),
        root = yal_AST_new_va(&env, 4,
                              yal_atom_symbol_new(&env, "-"),
                              yal_atom_decimal_new(&env, -4.37),
                              yal_atom_decimal_new(&env, 11.2),
                              yal_atom_decimal_new(&env, 3.1415)
            );
        );

    YAL_MATH_ENV_TEST( (-13 - 11.2 - 300),
        root = yal_AST_new_va(&env, 4,
                              yal_atom_symbol_new(&env, "-"),
                              yal_atom_real_new(&env, -13),
                              yal_atom_decimal_new(&env, 11.2),
                              yal_atom_real_new(&env, 300)
            );
        );

    YAL_MATH_ENV_TEST( (2*4),
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, "*"),
                              yal_atom_real_new(&env, 2),
                              yal_atom_real_new(&env, 4)
            );
        );

    YAL_MATH_ENV_TEST( (10*5.5*1.01),
        root = yal_AST_new_va(&env, 4,
                              yal_atom_symbol_new(&env, "*"),
                              yal_atom_real_new(&env, 10),
                              yal_atom_decimal_new(&env, 5.5),
                              yal_atom_decimal_new(&env, 1.01)
            );
        );

    YAL_MATH_ENV_TEST( (-4.37 * 11.2 * 3.1415),
        root = yal_AST_new_va(&env, 4,
                              yal_atom_symbol_new(&env, "*"),
                              yal_atom_decimal_new(&env, -4.37),
                              yal_atom_decimal_new(&env, 11.2),
                              yal_atom_decimal_new(&env, 3.1415)
            );
        );

    YAL_MATH_ENV_TEST( (4/2),
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, "/"),
                              yal_atom_real_new(&env, 4),
                              yal_atom_real_new(&env, 2)
            );
        );

    YAL_MATH_ENV_TEST( (10 / 5.5 / 1.1),
        root = yal_AST_new_va(&env, 4,
                              yal_atom_symbol_new(&env, "/"),
                              yal_atom_real_new(&env, 10),
                              yal_atom_decimal_new(&env, 5.5),
                              yal_atom_decimal_new(&env, 1.1)
            );
        );

    YAL_MATH_ENV_TEST( (-4.37 / 11.2 / 3.1415),
        root = yal_AST_new_va(&env, 4,
                              yal_atom_symbol_new(&env, "/"),
                              yal_atom_decimal_new(&env, -4.37),
                              yal_atom_decimal_new(&env, 11.2),
                              yal_atom_decimal_new(&env, 3.1415)
            );
        );
}



void test_math_2(void)
{
    yal_AtomHandle root;

    YAL_MATH_ENV_TEST( ( 4 * 2 * (10 / 5) * 40 * (200 - 10) * (42 + 90) ),
        root = yal_AST_new_va(&env, 7,
                              yal_atom_symbol_new(&env, "*"),
                              yal_atom_real_new(&env, 4),
                              yal_atom_real_new(&env, 2),
                              yal_AST_new_va(&env, 3,
                                             yal_atom_symbol_new(&env, "/"),
                                             yal_atom_real_new(&env, 10),
                                             yal_atom_real_new(&env, 5)
                                  ),
                              yal_atom_real_new(&env, 40),
                              yal_AST_new_va(&env, 3,
                                             yal_atom_symbol_new(&env, "-"),
                                             yal_atom_real_new(&env, 200),
                                             yal_atom_real_new(&env, 10)
                                  ),
                              yal_AST_new_va(&env, 3,
                                             yal_atom_symbol_new(&env, "+"),
                                             yal_atom_real_new(&env, 42),
                                             yal_atom_real_new(&env, 90)
                                  )
            );
        );

    YAL_MATH_ENV_TEST( ( 69 + 420 + 42 + (900 - 376 - ( 200 / 10)) + ( 55 * 10 ) ),
        root = yal_AST_new_va(&env, 6,
                              yal_atom_symbol_new(&env, "+"),
                              yal_atom_real_new(&env, 69),
                              yal_atom_real_new(&env, 420),
                              yal_atom_real_new(&env, 42),
                              yal_AST_new_va(&env, 4,
                                             yal_atom_symbol_new(&env, "-"),
                                             yal_atom_real_new(&env, 900),
                                             yal_atom_real_new(&env, 376),
                                             yal_AST_new_va(&env, 3,
                                                            yal_atom_symbol_new(&env, "/"),
                                                            yal_atom_real_new(&env, 200),
                                                            yal_atom_real_new(&env, 10)
                                                 )
                                  ),
                              yal_AST_new_va(&env, 3,
                                             yal_atom_symbol_new(&env, "*"),
                                             yal_atom_real_new(&env, 55),
                                             yal_atom_real_new(&env, 10)
                                  )
            );
        );

}

void test_buildin_marks(void)
{
    yal_AtomHandle root;

    YAL_QUICK_EVAL(
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, "+"),
                              yal_atom_decimal_new(&env, 3.1415),
                              yal_atom_decimal_new(&env, 1.2));
        yal_atom(&env, root)->is_marked = 1;
        );

    YAL_QUICK_EVAL(
        root = yal_AST_new_va(&env, 3,
                              yal_atom_real_new(&env, 1),
                              yal_atom_real_new(&env, 2),
                              yal_atom_real_new(&env, 3));
        yal_atom(&env, root)->is_marked = 1;
        );

    YAL_QUICK_EVAL(
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, "SomeInvalidSymbol"),
                              yal_atom_decimal_new(&env, 3.1415),
                              yal_atom_decimal_new(&env, 1.2));
        yal_atom(&env, root)->is_marked = 1;
        );

    YAL_QUICK_EVAL(
        root = yal_AST_new_va(&env, 3,
                              yal_atom_symbol_new(&env, "write"),
                              yal_atom_decimal_new(&env, 3.1415),
                              yal_atom_decimal_new(&env, 1.2));
        yal_atom(&env, root)->is_marked = 1;
        );


    YAL_QUICK_EVAL(
        root = yal_AST_new_va(&env, 2,
                              yal_atom_symbol_new(&env, "write"),
                              yal_atom_string_new(&env, "Unmarked write will write!")
            );
        );
    YAL_QUICK_EVAL(
        root = yal_AST_new_va(&env, 2,
                              yal_mark(&env, yal_atom_symbol_new(&env, "write")),
                              yal_atom_string_new(&env, "marks are great at not evaluating functions!")
            );
        );
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	TL(test_init_destroy());
	TL(test_buildin_write());
	TL(test_buildin_comparison_ops());
	TL(test_math_1());
	TL(test_math_2());
	TL(test_buildin_marks());

    tl_summary();
	return 0;
}
