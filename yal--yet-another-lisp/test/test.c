#include "testlib.h"
#include "../yal.h"

#define ATOM_APPEND(envPtr, listHandle, AtomHandle)       \
    yal_AtomList_push(&yal_atom(envPtr, listHandle)->List, AtomHandle);


#define YAL_VISUAL_EVAL(env, root) \
    yal_print_chain(&env, root); printf("\n"); \
    yal_parse(&env, root); \
    yal_print_chain(&env, root); printf("\n"); \
    printf("\n");


#define YAL_QUICK_ENV(internal)       \
    do { \
    yal_Environment env = yal_env_new(100); \
    yal_env_add_buildins(&env); \
    yal_AtomHandle root = yal_atom_list_new(&env); \
    internal \
    YAL_VISUAL_EVAL(env, root); \
    yal_env_delete(&env); \
} while (0)

void test_init_destroy(void)
{
    yal_Environment env = yal_env_new(0);
    TL_TEST(env.memory != NULL);
    yal_env_delete(&env);
}

void test_print_atoms_1(void)
{
    // (var 2)
    // ---------------------------------------------
    yal_Environment env = yal_env_new(10);
    yal_AtomHandle root = yal_atom_list_new(&env);
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "var"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 2));

    yal_print_chain(&env, root);
    printf("\n");
    yal_env_delete(&env);
}

void test_print_atoms_2(void)
{
    //
    // (write (+ 2 4 -3))
    // ---------------------------------------------

    yal_Environment env = yal_env_new(10);
    yal_AtomHandle root = yal_atom_list_new(&env);

    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "write"))

    yal_AtomHandle l1 = yal_atom_list_new(&env);
    ATOM_APPEND(&env, l1, yal_atom_symbol_new(&env, "+"));
    ATOM_APPEND(&env, l1, yal_atom_real_new(&env, 2));
    ATOM_APPEND(&env, l1, yal_atom_real_new(&env, 4));
    ATOM_APPEND(&env, l1, yal_atom_real_new(&env, -3));
    ATOM_APPEND(&env, root, l1);

    yal_print_chain(&env, root);
    printf("\n");

    yal_env_delete(&env);
}

void test_buildin_plus(void)
{
    /* (+ 2 4 11 -100) */
    YAL_QUICK_ENV(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "+"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 2));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 4));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 11));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, -100));
        );

    YAL_QUICK_ENV(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "+"));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, -4.37));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 11.2));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 3.1415));
        );

    YAL_QUICK_ENV(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "+"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, -13));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 11.2));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 300));
        );
}

void test_buildin_minus(void)
{
    YAL_QUICK_ENV(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "-"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 2));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 4));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 11));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, -100));
        );

    YAL_QUICK_ENV(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "-"));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 10));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 5.5));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 1));
        );

    YAL_QUICK_ENV(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "-"));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, -4.37));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 11.2));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 3.1415));
        );

    YAL_QUICK_ENV(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "-"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, -13));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 11.2));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 300));
        );
}

void test_buildin_multiply(void)
{
    YAL_QUICK_ENV(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "*"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 2));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 4));
        );

    YAL_QUICK_ENV(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "*"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 10));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 5.5));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 1.01));
        );

    YAL_QUICK_ENV(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "*"));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, -4.37));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 11.2));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 3.1415));
        );
}

void test_buildin_divide(void)
{
    YAL_QUICK_ENV(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "/"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 4));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 2));
        );

    YAL_QUICK_ENV(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "/"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 10));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 5.5));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 1.1));
        );

    YAL_QUICK_ENV(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "/"));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, -4.37));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 11.2));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 3.1415));
        );
}

void test_buildin_write(void)
{
    YAL_QUICK_ENV(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "write"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 4));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 2));
        );

    YAL_QUICK_ENV(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "write"));
    ATOM_APPEND(&env, root, yal_atom_real_new(&env, 10));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 5.5));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 1));
        );

    YAL_QUICK_ENV(
    ATOM_APPEND(&env, root, yal_atom_symbol_new(&env, "write"));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, -4.37));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 11.2));
    ATOM_APPEND(&env, root, yal_atom_decimal_new(&env, 3.1415));
        );
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	TL(test_init_destroy());
	TL(test_print_atoms_1());
	TL(test_print_atoms_2());
    
	TL(test_buildin_plus());
	TL(test_buildin_minus());
	TL(test_buildin_multiply());
	TL(test_buildin_divide());
	//TL(test_buildin_write());

    tl_summary();
	return 0;
}
