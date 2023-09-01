#include <iostream>
#include <string>

#include <readline/readline.h>
#include <readline/history.h>

#define YALPP_IMPLEMENTATION
#include "../yal.hpp"

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

    std::cout << "yal - Yet Another LISP" << std::endl
              << "Created by Thomas Alexgaard." << std::endl << std::endl;

    char* input;
    yal::Environment e;
    e.load_core();
    while (1) {
        input = readline("yal> ");
        if (input == std::string("(quit)"))
            break;
        add_history(input);
        yal::Expr* p = e.read(input);
        free(input);
        yal::Expr* res = e.eval(p);
        std::cout << yal::stringify(res) << std::endl;
   }
}
