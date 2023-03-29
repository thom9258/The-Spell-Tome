#include "testlib.h"
#include "../yal.h"

#define ATOM_APPEND(envPtr, listHandle, AtomHandle)       \
    yal_AtomList_push(&yal_atom(envPtr, listHandle)->List, AtomHandle);


#define YAL_VISUAL_EVAL(env, root) \
    yal_print_chain(&env, root); printf("\n"); \
    yal_parse(&env, root); \
    yal_print_chain(&env, root); printf("\n"); \
    printf("\n");


#define YAL_QUICK_EVAL(internal)                    \
    do { \
    yal_Environment env = yal_env_new(100); \
    yal_env_add_buildins(&env); \
    yal_AtomHandle root = yal_atom_list_new(&env); \
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
    yal_AtomHandle root = yal_atom_list_new(&env); \
    internal \
    YAL_VISUAL_EVAL(env, root); \
    TL_PRINT("Expecting equation result to be %lf\n", (float)c_res);    \
    YAL_RESCOMPARE(&env, c_res, root); \
    yal_env_delete(&env); \
} while (0)

void test_init_destroy(void)
{
    yal_Environment env = yal_env_new(0);
    TL_TEST(env.memory != NULL);
    yal_env_delete(&env);
}

void test_buildin_equal(void)
{
    YAL_MATH_ENV_TEST( (1),
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "equal"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 2));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 2));
        );

    YAL_MATH_ENV_TEST( (0),
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "equal"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 3));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 5));
        );

}

void test_buildin_plus(void)
{
    YAL_MATH_ENV_TEST( (2 + 4 + 11 + -100),
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "+"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 2));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 4));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 11));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, -100));
        );

    YAL_MATH_ENV_TEST( (-4.37 + 11.2 + 3.1415),
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "+"));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, -4.37));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 11.2));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 3.1415));
        );

    YAL_MATH_ENV_TEST( (-13 + 11.2 + 300),
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "+"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, -13));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 11.2));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 300));
        );
}

void test_buildin_minus(void)
{
    YAL_MATH_ENV_TEST( (-10),
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "-"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 10));
        );
    YAL_MATH_ENV_TEST( (10),
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "-"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, -10));
        );

    YAL_MATH_ENV_TEST( (2 - 11),
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "-"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 2));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 11));
        );

    YAL_MATH_ENV_TEST( (10 - -110),
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "-"));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 10));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, -110));
        );

    YAL_MATH_ENV_TEST( (-4.37 - 11.2 - 3.1415),
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "-"));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, -4.37));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 11.2));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 3.1415));
        );

    YAL_MATH_ENV_TEST( (-13 - 11.2 - 300),
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "-"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, -13));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 11.2));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 300));
        );
}

void test_buildin_multiply(void)
{
    YAL_MATH_ENV_TEST( (2*4),
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "*"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 2));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 4));
        );

    YAL_MATH_ENV_TEST( (10*5.5*1.01),
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "*"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 10));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 5.5));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 1.01));
        );

    YAL_MATH_ENV_TEST( (-4.37 * 11.2 * 3.1415),
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "*"));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, -4.37));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 11.2));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 3.1415));
        );
}

void test_buildin_divide(void)
{
    YAL_MATH_ENV_TEST( (4/2),
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "/"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 4));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 2));
        );

    YAL_MATH_ENV_TEST( (10 / 5.5 / 1.1),
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "/"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 10));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 5.5));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 1.1));
        );

    YAL_MATH_ENV_TEST( (-4.37 / 11.2 / 3.1415),
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "/"));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, -4.37));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 11.2));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 3.1415));
        );
}



void test_buildin_write(void)
{
    YAL_QUICK_EVAL(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "write"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 4));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 2));
        );

    YAL_QUICK_EVAL(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "write"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 10));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 5.5));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 1));
        );

    YAL_QUICK_EVAL(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "write"));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, -4.37));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 11.2));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 3.1415));
        );
}

void test_math(void)
{
    YAL_MATH_ENV_TEST( ( 4 + 2 + (10*5)),
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "+"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 4));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 2));
    yal_AtomHandle l1 = yal_atom_list_new(&env);
    ATOM_APPEND(&env, l1, yal_atom_symbol_new(&env, "*"));
    ATOM_APPEND(&env, l1, yal_atom_real_new(&env, 10));
    ATOM_APPEND(&env, l1, yal_atom_real_new(&env, 5));
    ATOM_APPEND(&env, root, l1);
        );
}


int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	TL(test_init_destroy());
	TL(test_buildin_equal());
	TL(test_buildin_plus());
	TL(test_buildin_minus());
	TL(test_buildin_multiply());
	TL(test_buildin_divide());
	TL(test_math());
	//TL(test_buildin_write());

    tl_summary();
	return 0;
}
