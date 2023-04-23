#include "../buildins.h"
#include "../core.h"

#include <readline/readline.h>
#include <readline/history.h>

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

    Environment env = {0};
    Environment_new(&env);
    Environment_add_core(&env);
    expr* program;
    expr* result;

    while (1) {
    char* input = readline("yal> ");
    add_history(input);
    program = str_read(&env, input);
    free(input);
    result = buildin_eval(&env, program);
    buildin_print(&env, result); printf("\n");
    }

    Environment_destroy(&env);
	return 0;
}
