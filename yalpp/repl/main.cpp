#include <iostream>
#include <sstream>
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

    char* input;
    yal::Environment e;
    int i = 0;

    e.load_std();
    std::cout << "yal - Yet Another LISP" << std::endl
              << "\tCreated by Thomas Alexgaard." << std::endl
              << "(quit) to quit." << std::endl << std::endl;

    while (1) {
        std::stringstream greet;
        yal::Expr* p = nullptr;
        yal::Expr* res = nullptr;
        greet << "yal [" << i++ << "] -> ";
        input = readline(greet.str().c_str());
        if (input == std::string("(quit)"))
            break;
        add_history(input);
        p = e.read(input);
        free(input);
        res = e.eval(p);
        std::string outbuf = e.outbuffer_getreset();
        if (outbuf != "")
            std::cout << outbuf;
        std::cout << yal::stringify(res) << std::endl;
   }
}
