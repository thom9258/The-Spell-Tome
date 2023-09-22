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

yal::Expr*
write_override(yal::VariableScope* _s, yal::Expr* _e)
{
    yal::Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) < 1)
        std::cout << "NIL" << std::endl;
    if (len(args) > 1)
        throw ProgramError("write", "expects single argument to write, not", args);
    std::cout << stringify(first(args), false);
    return first(args);
}

yal::Expr*
newline_override(yal::VariableScope* _s, yal::Expr* _e)
{
    if (len(_e) != 0)
        throw ProgramError("newline", "expects no args, got", _e);
    std::cout << std::endl;
    return _s->env()->nil();
}

int
main(int argc, char **argv)
{

    char* input;
    int i = 0;
    yal::Environment e;
    e.add_buildin("write", write_override);
    e.add_buildin("newline", newline_override);
    e.add_buildin("read-real", termread_real);
    e.add_buildin("read-decimal", termread_decimal);
    e.add_buildin("read-line", termread_line);
    e.load_core();
    e.load(yal::std_lib());

    if (argc == 2) {
        std::cout << "Running file " << argv[1] << std::endl;
        e.load_file(argv[1]);
    }

    /*REPL loop*/
    std::cout << "yal - Yet Another LISP"
            << " - Created by Thomas Alexgaard." << std::endl
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
