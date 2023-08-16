#include <stdlib.h>

#include <readline/readline.h>
#include <readline/history.h>

#define YAL_IMPLEMENTATION
#include "../yal.h"

int
main(int argc, char **argv) {
	(void)argc;
	(void)argv;
    Environment e = {0};
    Exception exception = {0};
    expr* read_result;
    expr* eval_result;
    char* input = NULL;

    Env_new(&e);
    Env_add_core(&e);
    while (1) {
        input = readline("yal> ");
        if (input == NULL)
            break;
        printf("you wrote: %s\n", input);
        read_result = read(&e, &e.globals, &exception, input);
        if (Exception_is_error(&exception)) {
        printf("yal> ERROR: %s\n", exception.msg.c_str);
        }
        Exception_destroy(&exception);
        
        eval_result = eval_expr(&e, &e.globals, &exception, read_result);
        if (Exception_is_error(&exception)) {
        printf("yal> ERROR: %s\n", exception.msg.c_str);
        }
        Exception_destroy(&exception);
        
        WITH_STRINGEXPR(res, eval_result,
        printf("EVAL: %s\n", res.c_str);
        );
        
        add_history(input);
        free(input);
    }

    Env_destroy(&e);
    Exception_destroy(&exception);
	return 0;
}
