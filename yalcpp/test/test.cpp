#include <iostream>

#define YALCPP_IMPLEMENTATION
#include "../yal.hpp"

#include "testlib.h"


void
test_types_creation(void)
{
    yal::Environment e;
    yal::Expr* real = e.real(4);
    TL_TEST(real->type == yal::TYPE_REAL);
    TL_TEST(real->real == 4);
    std::string realstr = yal::stream(real).str();
    std::cout << realstr << std::endl;
    TL_TEST(realstr == "4");

    yal::Expr* deci = e.decimal(4.3f);
    TL_TEST(deci->type == yal::TYPE_DECIMAL);
    TL_TEST(deci->decimal == 4.3f);
    std::string decistr = yal::stream(deci).str();
    std::cout << decistr << std::endl;
    TL_TEST(decistr == "4.3");

    yal::Expr* cns = e.cons(real, deci);
    TL_TEST(cns->type == yal::TYPE_CONS);
    TL_TEST(cns->cons.car == real);
    TL_TEST(cns->cons.cdr == deci);
    std::string cnsstr = yal::stream(cns).str();
    std::cout << cnsstr << std::endl;
    TL_TEST(cnsstr == "(4 . 4.3)");

    yal::Expr* str = e.string("mystr");
    TL_TEST(str->type == yal::TYPE_STRING);
    TL_TEST(str->string == std::string("mystr"));
    std::string strstr = yal::stream(str).str();
    std::cout << strstr << std::endl;
    TL_TEST(strstr == "\"mystr\"");

    yal::Expr* sym = e.symbol("mysym");
    TL_TEST(sym->type == yal::TYPE_SYMBOL);
    TL_TEST(sym->symbol == std::string("mysym"));
    std::string symstr = yal::stream(sym).str();
    std::cout << symstr << std::endl;
    TL_TEST(symstr == "mysym");

    yal::Expr* lst = e.list({e.symbol("*"), e.real(2), e.decimal(3.14)});
    TL_TEST(lst->type == yal::TYPE_CONS);
    std::string lststr = yal::stream(lst).str();
    std::cout << lststr << std::endl;
    TL_TEST(lststr == std::string("(* 2 3.14)"));

    yal::Expr* lst2 = e.list({e.symbol("+"), e.real(5), e.list({e.symbol("*"), e.real(4), e.decimal(2.5)})});
    std::string lst2str = yal::stream(lst2).str();
    std::cout << lst2str << std::endl;
    TL_TEST(lst2str == std::string("(+ 5 (* 4 2.5))"));

}

void
test_len(void)
{
    yal::Environment e;

    yal::Expr* list = e.list({e.real(1)});
    std::cout << "list: " << yal::stringify(list) << std::endl;
    std::cout << "len of list: " << yal::len(list) << std::endl;
    TL_TEST(yal::len(list) == 1);

    list = e.list({e.real(1), e.real(2)});
    std::cout << "list: " << yal::stringify(list) << std::endl;
    std::cout << "len of list: " << yal::len(list) << std::endl;
    TL_TEST(yal::len(list) == 2);

    list = e.list({e.real(1), e.real(2), e.real(3), e.real(4)});
    std::cout << "list: " << yal::stringify(list) << std::endl;
    std::cout << "len of list: " << yal::len(list) << std::endl;
    TL_TEST(yal::len(list) == 4);

    list = e.list({
            e.real(1), e.real(2), e.real(3), e.real(4),
            e.real(1), e.real(2), e.real(3), e.real(4)});
    std::cout << "list: " << yal::stringify(list) << std::endl;
    std::cout << "len of list: " << yal::len(list) << std::endl;
    TL_TEST(yal::len(list) == 8);

    list = e.list({});
    std::cout << "list: " << yal::stringify(list) << std::endl;
    std::cout << "len of list: " << yal::len(list) << std::endl;
    TL_TEST(yal::len(list) == 0);

    list = e.string("not a list..");
    std::cout << "list: " << yal::stringify(list) << std::endl;
    std::cout << "len of list: " << yal::len(list) << std::endl;
    TL_TEST(yal::len(list) == 1);

}

void
test_assoc(void)
{
    yal::Environment e;

    yal::Expr* res = nullptr;
    yal::Expr* alist = e.list({
            e.cons(e.symbol("a"), e.real(2)),
            e.cons(e.symbol("b"), e.decimal(4.2)),
            e.list({e.symbol("c"), e.string("bob"), e.real(65)}),
            });

    std::cout << "len of alist: " << yal::len(alist) << std::endl;
    TL_TEST(yal::len(alist) == 3);
    std::cout << "alist: " << yal::stringify(alist) << std::endl;
    TL_TEST(yal::stringify(alist) == "((a . 2) (b . 4.2) (c \"bob\" 65))");


    res = yal::assoc(e.symbol("a"), alist);
    std::cout << "a: " << yal::stringify(res) << std::endl;
    TL_TEST(yal::stringify(res) == "(a . 2)");

    res = yal::assoc(e.symbol("b"), alist);
    std::cout << "b: " << yal::stringify(res) << std::endl;
    TL_TEST(yal::stringify(res) == "(b . 4.2)");

    res = yal::assoc(e.symbol("c"), alist);
    std::cout << "c: " << yal::stringify(res) << std::endl;
    TL_TEST(yal::stringify(res) == "(c \"bob\" 65)");

    res = yal::assoc(e.symbol("d"), alist);
    std::cout << "d: " << yal::stringify(res) << std::endl;
    TL_TEST(yal::stringify(res) == "NIL");
}

void
test_globals(void)
{
    yal::Environment e;
    const float pi = 3.141592; 

    e.add_constant("*Creator*", e.string("Thomas Alexgaard"));
    e.add_constant("PI", e.decimal(pi));

    std::cout << "len of constants: " << yal::len(e.constants()) << std::endl;
    TL_TEST(yal::len(e.constants()) == 2);

    std::cout << "constants: " << yal::stringify(e.constants()) << std::endl;
    TL_TEST(yal::stringify(e.constants()) == "((PI . 3.14159) (*Creator* . \"Thomas Alexgaard\"))");

    e.add_constant("*Creator-github*", e.string("https://github.com/thom9258/"));
    e.add_constant("*Host-Languange*", e.string("C++"));
    e.add_constant("*Version*", e.list({e.real(0), e.real(1)}));
    e.add_constant("PI/2", e.decimal(pi/2));
    e.add_constant("PI2", e.decimal(pi*2));

    yal::Expr* pisym = yal::cdr(yal::assoc(e.symbol("PI"), e.constants()));
    std::cout << "pisym-value: " << yal::stringify(pisym) << std::endl;
    TL_TEST(yal::stringify(pisym) == "3.14159");
}

void
print_core(void)
{
    yal::Environment e;
    e.load_core();
    std::cout << yal::stringify(e.constants()) << std::endl;
}

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

    TL(test_types_creation());
    TL(test_len());
    TL(test_assoc());
    TL(test_globals());
    TL(print_core());
}
