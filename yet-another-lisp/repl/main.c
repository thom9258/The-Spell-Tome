#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>

#define YAL_IMPLEMENTATION
#include "../yal.h"

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;
    char* input;
    Environment e;
    Exception msg = {0};
    expr* read_result;
    expr* eval_result;

    Env_new(&e);
    Env_add_core(&e);
    ASSERT_INV_ENV(&e);
    while (1) {
        input = readline("yal> ");
        read_result = read(&e, &e.globals, &msg, input);
        if (Exception_is_error(&msg)) {
            printf("yal> ERROR: %s\n", msg.msg.c_str);
        }
        Exception_destroy(&msg);

        eval_result = eval_expr(&e, &e.globals, &msg, read_result);
        if (Exception_is_error(&msg)) {
            printf("yal> ERROR: %s\n", msg.msg.c_str);
        }
        Exception_destroy(&msg);

        WITH_STRINGEXPR(res, eval_result,
                        printf("EVAL: %s\n", res.c_str);
            );

        add_history(input);
        free(input);
    }

    Env_destroy(&e);
    Exception_destroy(&msg);
	return 0;
}
