#include <iostream>
#include <sstream>
#include <string>

#include <readline/readline.h>
#include <readline/history.h>

#define YALPP_IMPLEMENTATION
#include "../yal.hpp"

yal::Expr*
termread_real(yal::VariableScope* _s, yal::Expr* _e)
{
  if (len(_e) != 0)
      throw ProgramError("termread-real", "expects no arguments, got", _e);
  int real = 0;
  std::cin >> real;
  return _s->env()->real(real);
}

yal::Expr* termread_decimal(yal::VariableScope* _s, yal::Expr* _e)
{
  if (len(_e) != 0)
      throw ProgramError("termread-decimal", "expects no arguments, got", _e);
  float decimal = 0;
  std::cin >> decimal;
  return _s->env()->decimal(decimal);
}

yal::Expr*
termread_line(yal::VariableScope* _s, yal::Expr* _e)
{
  if (len(_e) != 0)
      throw ProgramError("termread-line", "expects no arguments, got", _e);
  std::string line = "";
  std::getline(std::cin, line);
  return _s->env()->string(line);
}

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

    char* input;
    yal::Environment e;
    int i = 0;

    e.load_std();

    e.add_buildin("read-real", termread_real);
    e.add_buildin("read-decimal", termread_decimal);
    e.add_buildin("read-line", termread_line);

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
