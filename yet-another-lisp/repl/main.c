#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>

#define YAL_IMPLEMENTATION
#include "../yal.h"

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

    Environment env = {0};
    char* input;
    usermsg msg;
    expr* program;
    expr* result;
    Env_new(&env);
    Env_add_core(&env);

    while (1) {
        input = readline("yal> ");
        ASSERT_INV_ENV(&env);
        msg = read(&env, &program, input);
        if (USERMSG_IS_ERROR(&msg))
            printf("Parsing error:\n\t%s\n", msg.info);
        msg = eval(&env, &result, program);
        if (USERMSG_IS_ERROR(&msg))
            printf("Eval error:\n\t%s\n", msg.info);
        print(result); printf("\n");
        add_history(input);
        free(input);
    }

    Env_destroy(&env);
	return 0;
}
