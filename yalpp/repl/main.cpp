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
    while (1) {
        input = readline("yal> ");
        std::cout << input << std::endl;
        add_history(input);
        free(input);
   }
}
