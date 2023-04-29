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
    //expr* program;
    //expr* result;

    Env_new(&env);
    //Env_add_core(&env);
    while (1) {
        printf("looking for input!\n");
        input = readline("yal> ");
        add_history(input);
        printf("got input %s\n", input);
        //program = read(&env, input);
        free(input);
        //result = buildin_eval(&env, program);
        //buildin_print(&env, result); printf("\n");
    }

    Env_destroy(&env);
	return 0;
}
