#include <iostream>

#define YALCPP_IMPLEMENTATION
#include "../yal.hpp"

#include "testlib.h"

bool
eval_test(yal::Environment* _e, yal::Expr* _prg, const char* _gt)    
{
    yal::Expr* res;
    std::cout << "program: " << yal::stringify(_prg) << std::endl;
    res = _e->eval(_prg);
    std::cout << "res: " << yal::stringify(res) << std::endl;
    return yal::stringify(res) == _gt;
}

bool
lex_type_test(yal::Environment* _e, const char* _prg, yal::TYPE _type)
{
    yal::Expr* res;
    std::cout << "program: " << _prg << std::endl;
    res = _e->read(_prg);
    std::cout << "res: " << yal::stringify(res) << std::endl;
    if (is_nil(res))
        return false;
    return _type == res->type;
}

bool
read_test(yal::Environment* _e, const char* _prg)
{
    yal::Expr* res;
    std::string resstr;
    std::cout << "program: " << _prg << std::endl;
    res = _e->read(_prg);
    resstr = yal::stringify(res);
    std::cout << "res: " << resstr << std::endl;
    return resstr == _prg;
}

bool
read_test(yal::Environment* _e, const char* _prg, const char* _gt)
{
    yal::Expr* res;
    std::string resstr;
    std::cout << "program: " << _prg << std::endl;
    res = _e->read(_prg);
    resstr = yal::stringify(res);
    std::cout << "res: " << resstr << std::endl;
    return resstr == _gt;
}

char
repl_test(yal::Environment* _e, const char* _p, const char* _gt)
{
    std::string read_str;
    std::string eval_str;
    yal::Expr* read_result;
    yal::Expr* eval_result;

    read_result = _e->read(_p);
    std::cout << "read res: " << stringify(read_result) << std::endl;
    eval_result = _e->eval(read_result);
    std::cout << "eval res: " << stringify(eval_result) << std::endl;
    return stringify(eval_result) == _gt;
}

/*======================================================================*/
/*======================================================================*/
/*======================================================================*/

void
test_sizes(void)
{
    const size_t exprsz = sizeof(yal::Expr);
    const size_t conssz = sizeof(yal::Expr*) * 2;
    const size_t enumtypesz = sizeof(yal::TYPE);
    std::cout << "sizeof Expr = " << exprsz << std::endl; 
    std::cout << "sizeof cons = " << conssz << std::endl; 
    std::cout << "sizeof smallsz = " << yal::smallXsz << std::endl; 
    std::cout << "sizeof type = " << enumtypesz << std::endl; 
}

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

    yal::Expr* sym = yal::cdr(yal::assoc(e.symbol("*Host-Languange*"), e.constants()));
    std::cout << "symbol-value: " << yal::stringify(sym) << std::endl;
    TL_TEST(yal::stringify(sym) == "\"C++\"");
}

void
test_ipreverse(void)
{
    yal::Environment e;
    yal::Expr* list;
    yal::Expr* rev;

    list = e.list({e.real(1), e.real(2), e.real(3), e.real(4)});
    std::cout << "list:     " << yal::stringify(list) << std::endl;
    rev = ipreverse(list);
    std::cout << "reversed: " << yal::stringify(rev) << std::endl;
    TL_TEST(yal::stringify(rev) == "(4 3 2 1)");
}

void
test_simple_eval(void)
{
    yal::Environment e;
    e.load_core();

    yal::Expr* program;

    program = e.symbol("PI2");
    TL_TEST(eval_test(&e, program, "6.28318"));
    program = e.symbol("PI/2");
    TL_TEST(eval_test(&e, program, "1.5708"));

    program = e.list({e.symbol("quote"), e.list({e.real(2), e.real(5)})});
    TL_TEST(eval_test(&e, program, "(2 5)"));

    program = e.list({e.symbol("+"), e.real(2), e.real(5)});
    TL_TEST(eval_test(&e, program, "7"));

    program = e.list({
            e.symbol("list"),
            e.list({e.symbol("+"), e.real(2), e.real(2)}),
            e.symbol("PI/2")});
    TL_TEST(eval_test(&e, program, "(4 1.5708)"));
}

void
test_lex_types(void)
{
    yal::Environment e;
    e.load_core();

    TL_TEST(lex_type_test(&e, "myvar", yal::TYPE_SYMBOL));
    TL_TEST(lex_type_test(&e, "my/other-var_", yal::TYPE_SYMBOL));
    TL_TEST(lex_type_test(&e, "2", yal::TYPE_REAL));
    TL_TEST(lex_type_test(&e, "-2", yal::TYPE_REAL));
    TL_TEST(lex_type_test(&e, "3.14", yal::TYPE_DECIMAL));
    TL_TEST(lex_type_test(&e, "-3.14", yal::TYPE_DECIMAL));
    //TL_TEST(lex_type_test(&e, ".14", yal::TYPE_DECIMAL));
    TL_TEST(lex_type_test(&e, "9.56700", yal::TYPE_DECIMAL));
    TL_TEST(lex_type_test(&e, "\"MY STRING!\"", yal::TYPE_STRING));
    TL_TEST(lex_type_test(&e, "(1 2 3)", yal::TYPE_CONS));
}

void
test_read(void)
{
    yal::Environment e;
    e.load_core();
    TL_TEST(read_test(&e, "myvar"));
    TL_TEST(read_test(&e, "my/other-var_"));
    TL_TEST(read_test(&e, "2"));
    TL_TEST(read_test(&e, "-2"));
    TL_TEST(read_test(&e, "3.14"));
    TL_TEST(read_test(&e, "-3.14"));
    //TL_TEST(read_test(&e, ".14", "0.14"));
    TL_TEST(read_test(&e, "9.56723"));
    TL_TEST(read_test(&e, "\"MY STRING!\""));
    TL_TEST(read_test(&e, "(1 2 3)"));
    TL_TEST(read_test(&e, "(mysym 2.34 3 \"mystr\")"));
    TL_TEST(read_test(&e, "'(1 2 3)", "(quote (1 2 3))"));
    TL_TEST(read_test(&e, "[1 2 3]", "(list 1 2 3)"));
    TL_TEST(read_test(&e, "'[1 2 3]", "(quote (list 1 2 3))"));
    //TL_TEST(read_test(&e, "{1 2 3}", "(vector 1 2 3)"));
    TL_TEST(read_test(&e, "'(1 (+ 3 2) (/ 2 4))", "(quote (1 (+ 3 2) (/ 2 4)))"));
    TL_TEST(read_test(&e, "(fn pow2 (x) (* x x))", "(fn pow2 (x) (* x x))"));

    TL_TEST(read_test(&e, "('hi 9 0 1 \"friends\")",  "((quote hi) 9 0 1 \"friends\")"));
    TL_TEST(read_test(&e, "'hi", "(quote hi)"));
    TL_TEST(read_test(&e, "'7.245", "(quote 7.245)"));

    TL_TEST(read_test(&e, "['hello 'friend]", "(list (quote hello) (quote friend))"));
    TL_TEST(read_test(&e, "['hi '(1 2 3)]", "(list (quote hi) (quote (1 2 3)))"));

}

void
test_buildin_list_creation(void)
{
    yal::Environment e;
    e.load_core();
    TL_TEST(repl_test(&e, "(quote (foo bar (quote (baz))))", "(foo bar (quote (baz)))"));
    TL_TEST(repl_test(&e, "'(foo bar '(baz))", "(foo bar (quote (baz)))"));
    TL_TEST(repl_test(&e, "(quote (1 2 3 4))", "(1 2 3 4)"));

    TL_TEST(repl_test(&e, "'(1 2 '(3 4) (quote 5))", "(1 2 (quote (3 4)) (quote 5))"));

    TL_TEST(repl_test(&e, "(quote ()  )", "NIL"));
    TL_TEST(repl_test(&e, "'(+ 2 3 (* 1 2 3 4))", "(+ 2 3 (* 1 2 3 4))"));

    TL_TEST(repl_test(&e, "(list 1 2 3 )", "(1 2 3)"));
    TL_TEST(repl_test(&e, "(list 2.4 -2 5 1)", "(2.4 -2 5 1)"));
    TL_TEST(repl_test(&e, "(list 9.5667 -2.123 4.345)", "(9.5667 -2.123 4.345)"));
    TL_TEST(repl_test(&e, "(list 1 (+ 1 1) 3 )", "(1 2 3)"));
    TL_TEST(repl_test(&e, "(list 3 (+ -1 1 3) (+ 1 1 1))", "(3 3 3)"));

    TL_TEST(repl_test(&e, "(cons 3 4)", "(3 . 4)"));
    TL_TEST(repl_test(&e, "(cons 'name \"Henry\")", "(name . \"Henry\")"));
    TL_TEST(repl_test(&e, "(cons 3 (* 2 2))", "(3 . 4)"));
    TL_TEST(repl_test(&e, "(cons 'mylist '(2 2))", "(mylist 2 2)"));
}


void
test_buildin_list_management(void)
{
    yal::Environment e;
    e.load_core();
    TL_TEST(repl_test(&e,
                      "(put 0 (list 1 2 3))",
                      "(0 1 2 3)"));
    TL_TEST(repl_test(&e,
                      "(put nil '(t nil t))",
                      "(NIL T NIL T)"));

    TL_TEST(repl_test(&e,
                      "(put \"myname\" '(23 m))",
                      "(\"myname\" 23 m)"));
}

void
test_buildin_range(void)
{
    yal::Environment e;
    e.load_core();
    TL_TEST(repl_test(&e,
                       "(range 2 5)",
                       "(2 3 4 5)"));

    TL_TEST(repl_test(&e,
                       "(range (+ -1 2) 4)",
                       "(1 2 3 4)"));

    TL_TEST(repl_test(&e,
                       "(range 0 10)",
                       "(0 1 2 3 4 5 6 7 8 9 10)"));

    TL_TEST(repl_test(&e, "(range 5 1)", "(5 4 3 2 1)"));
}

void
test_buildin_accessors(void)
{

    yal::Environment e;
    e.load_core();
    TL_TEST(repl_test(&e,
                       "(car '(1 2 3 4))",
                       "1"));

    TL_TEST(repl_test(&e,
                       "(cdr '(1 2 3 4))",
                       "(2 3 4)"));

    TL_TEST(repl_test(&e,
                       "(first (quote (1 2 3 4)))",
                       "1"));

    TL_TEST(repl_test(&e,
                       "(second (quote ((1 2) (3 4) (5 6)) ))",
                       "(3 4)"));

    TL_TEST(repl_test(&e,
                       "(third (quote (1 2 3 4)))",
                       "3"));

    TL_TEST(repl_test(&e,
                       "(fourth (quote (1 2 3 4)))",
                       "4"));

    TL_TEST(repl_test(&e,
                       "(nth 9 (range 0 20))",
                       "9"));

    TL_TEST(repl_test(&e,
                       "(nth 17 (range 0 20))",
                       "17"));

    TL_TEST(repl_test(&e,
                       "(nth 21 (range 0 20))",
                       "NIL"));
}

void
test_buildin_math(void)
{

    yal::Environment e;
    e.load_core();

    TL_TEST(repl_test(&e, "(+)", "0"));
    TL_TEST(repl_test(&e, "(-)", "0"));
    TL_TEST(repl_test(&e, "(*)", "1"));
    TL_TEST(repl_test(&e, "(/)", "1"));
    TL_TEST(repl_test(&e, "(+ 1 2)", "3"));
    TL_TEST(repl_test(&e, "(- 7 3)", "4"));
    TL_TEST(repl_test(&e, "(* 4 3)", "12"));
    TL_TEST(repl_test(&e, "(/ 8 2)", "4"));
    TL_TEST(repl_test(&e, "(/ 0 2)", "0"));
    TL_TEST(repl_test(&e, "(+ 2 (- 5 6) 1)", "2"));
    TL_TEST(repl_test(&e, "(+ 4 (* 2 6) (- 10 5))", "21"));
    TL_TEST(repl_test(&e, "(- 5 2)", "3"));
    TL_TEST(repl_test(&e, "(/ (- (+ 515 (* -87 311)) 296) 27)", "-994"));
}

void
test_buildin_equality(void)
{
    yal::Environment e;
    e.load_core();

    TL_TEST(repl_test(&e, "(= 1 2)", "NIL"));
    TL_TEST(repl_test(&e, "(= 'somesym 'somesym)", "T"));
    TL_TEST(repl_test(&e, "(= \"Hello!\" \"Hello!\")", "T"));
    TL_TEST(repl_test(&e, "(= \"1\" 1)", "NIL"));
    TL_TEST(repl_test(&e, "(= 7 3.431)", "NIL"));
    TL_TEST(repl_test(&e, "(= 2 2)", "T"));
    TL_TEST(repl_test(&e, "(= 2.34 2.34)", "T"));
    TL_TEST(repl_test(&e, "(= 7)", "T"));
    TL_TEST(repl_test(&e, "(=)", "T"));
    TL_TEST(repl_test(&e, "(= (+ 2 2) 4 (- 10 6) (* 2 2))", "T"));
    TL_TEST(repl_test(&e, "(= '(1 2 3) '(1 2 3))", "T"));

    TL_TEST(repl_test(&e, "(eq '(1 2 3) (quote (1 2 3)) [1 2 3] (list 1 2 3) (range 1 3))", "T"));
    TL_TEST(repl_test(&e, "(eq 4 (+ 2 2))", "T"));

    TL_TEST(repl_test(&e, "(equal 4 (+ 2 2))", "NIL"));
    TL_TEST(repl_test(&e, "(equal (1 2 3) (1 2 3) (1 2 3))", "T"));
}

void
test_conditionals(void)
{
    yal::Environment e;
    e.load_core();

    TL_TEST(repl_test(&e, 
                       "(if (= 2 2) 4)",
                       "4"));

    TL_TEST(repl_test(&e, 
                       "(if (= 2 2) 4 0)",
                       "4"));

    TL_TEST(repl_test(&e, 
                       "(if t 2)",
                       "2"));

    TL_TEST(repl_test(&e, 
                       "(if '(1 2 3) t)",
                       "T"));


    TL_TEST(repl_test(&e, 
                       "(if nil 2 3)",
                       "3"));

    TL_TEST(repl_test(&e, 
                       "(if () t nil)",
                       "NIL"));

    TL_TEST(repl_test(&e, 
                       "(if (NIL) t nil)",
                       "NIL"));

    TL_TEST(repl_test(&e, 
                       "(if (= 2 3) t nil)",
                       "NIL"));

    TL_TEST(repl_test(&e, 
                       "(if (= 2 3) 1 0)",
                       "0"));

    TL_TEST(repl_test(&e, 
                      "(fn is10 (v) "
                      "  (if (= v 10) t nil))",
                      "is10"));

    TL_TEST(repl_test(&e, 
                       "(is10 7)",
                       "NIL"));

    TL_TEST(repl_test(&e, 
                       "(is10 10)",
                       "T"));
}

void
test_buildin_progn(void)
{
    yal::Environment e;
    e.load_core();

    TL_TEST(repl_test(&e, 
                       "(progn)",
                       "NIL"));

    TL_TEST(repl_test(&e, 
                       "(progn (+ 2 3) (- 5 2))",
                       "3"));

    TL_TEST(repl_test(&e, 
                       "(progn (+ 30 51) (* 10 3 2) (range 0 5) [1 2 3])",
                       "(1 2 3)"));

    TL_TEST(repl_test(&e, 
                       "(progn 5)",
                       "5"));
}

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

    TL(test_sizes());
    TL(test_types_creation());
    TL(test_len());
    TL(test_assoc());
    TL(test_globals());
    TL(print_core());
    TL(test_ipreverse());
    TL(test_simple_eval());
    TL(test_lex_types());
    TL(test_read());
    TL(test_buildin_range());
    TL(test_buildin_equality());
    TL(test_buildin_accessors());
    TL(test_buildin_list_creation());
    //TL(test_conditionals());
    //TL(test_buildin_progn());
    //TL(test_buildin_math());

    tl_summary();
}
