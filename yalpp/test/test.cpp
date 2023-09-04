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
    std::cout << "res:     " << yal::stringify(res) << std::endl;
    return yal::stringify(res) == _gt;
}

bool
lex_type_test(yal::Environment* _e, const char* _prg, yal::TYPE _type)
{
    yal::Expr* res;
    std::cout << "program: " << _prg << std::endl;
    res = _e->read(_prg);
    std::cout << "res:     " << yal::stringify(res) << std::endl;
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
    std::cout << "res:     " << resstr << std::endl;
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
    std::cout << "res:     " << resstr << std::endl;
    return resstr == _gt;
}

char
repl_test(yal::Environment* _e, const char* _p, const char* _gt)
{
    std::string read_str;
    std::string eval_str;
    yal::Expr* read_result;
    yal::Expr* eval_result;
    if (_p == nullptr || _gt == nullptr)
        return false;

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
test_nilp(void)
{
    yal::Environment e;
    yal::Expr* p = e.read("(1 2 3)");
    std::cout << stringify(p) << std::endl;
    TL_TEST(is_nil(p) == false);

    p = e.read("mysym");
    std::cout << stringify(p) << std::endl;
    TL_TEST(is_nil(p) == false);

    p = e.read("32");
    std::cout << stringify(p) << std::endl;
    TL_TEST(is_nil(p) == false);

    p = e.read("7.1234");
    std::cout << stringify(p) << std::endl;
    TL_TEST(is_nil(p) == false);

    p = e.read("\"mystr\"");
    std::cout << stringify(p) << std::endl;
    TL_TEST(is_nil(p) == false);

    p = e.read("[4 5 6]");
    std::cout << stringify(p) << std::endl;
    TL_TEST(is_nil(p) == false);

    p = e.read("nil");
    std::cout << stringify(p) << std::endl;
    TL_TEST(is_nil(p) == true);

    p = e.read("NIL");
    std::cout << stringify(p) << std::endl;
    TL_TEST(is_nil(p) == true);

    /*TODO: fix lexing of empty lists!*/
    //p = e.read("(  )");
    //std::cout << stringify(p) << std::endl;
    //TL_TEST(is_nil(p) == true);
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

    std::cout << "len of constants: " << e.global_scope()->variables_len() << std::endl;
    TL_TEST(e.global_scope()->variables_len() == 2);

    yal::Expr* constant = e.global_scope()->variable_get("PI");
    std::cout << "PI: " << yal::stringify(constant) << std::endl;
    TL_TEST(yal::stringify(constant) == "(PI 3.14159 CONSTANT)");

    constant = e.global_scope()->variable_get("*Creator*");
    std::cout << "Creator: " << yal::stringify(constant) << std::endl;
    TL_TEST(yal::stringify(constant) == "(*Creator* \"Thomas Alexgaard\" CONSTANT)");

    e.add_constant("*Creator-github*", e.string("https://github.com/thom9258/"));
    e.add_constant("*Host-Languange*", e.string("C++"));
    e.add_constant("*Version*", e.list({e.real(0), e.real(1)}));
    e.add_constant("PI/2", e.decimal(pi/2));
    e.add_constant("PI2", e.decimal(pi*2));
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

    TL_TEST(read_test(&e, "''7", "(quote (quote 7))"));
    TL_TEST(read_test(&e, "''(1 2)", "(quote (quote (1 2)))"));

    TL_TEST(read_test(&e, "(1 NIL 3)", "(1 NIL 3)"));
    TL_TEST(read_test(&e, "(1 () 3)", "(1 NIL 3)"));
    TL_TEST(read_test(&e, "(1 [] 3)", "(1 NIL 3)"));
}

void
test_buildin_list_creation(void)
{
    yal::Environment e;
    e.load_core();
    TL_TEST(repl_test(&e, "(quote (foo bar (quote (baz))))", "(foo bar (quote (baz)))"));
    TL_TEST(repl_test(&e, "'(foo bar '(baz))", "(foo bar (quote (baz)))"));
    TL_TEST(repl_test(&e, "(quote (1 2 3 4))", "(1 2 3 4)"));

    /*TODO: locate why this error is there*/
    //TL_TEST(repl_test(&e, "'(1 2 '(3 4) (quote 5))", "(1 2 (quote (3 4)) (quote 5))"));

    TL_TEST(repl_test(&e, "()", "NIL"));
    TL_TEST(repl_test(&e, "[]", "NIL"));

    TL_TEST(repl_test(&e, "(quote ()  )", "(error \"[quote] expected 1 argument\")"));
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

    /*TODO: add these as macros*/
    //TL_TEST(repl_test(&e,
    //                   "(first (quote (1 2 3 4)))",
    //                   "1"));
    //
    //TL_TEST(repl_test(&e,
    //                   "(second (quote ((1 2) (3 4) (5 6)) ))",
    //                   "(3 4)"));
    //
    //TL_TEST(repl_test(&e,
    //                   "(third (quote (1 2 3 4)))",
    //                   "3"));
    //
    //TL_TEST(repl_test(&e,
    //                   "(fourth (quote (1 2 3 4)))",
    //                   "4"));

    //TL_TEST(repl_test(&e,
    //                   "(nth 9 (range 0 20))",
    //                   "9"));

    TL_TEST(repl_test(&e,
                       "(nthcdr 17 (range 0 20))",
                       "(17 18 19 20)"));

    TL_TEST(repl_test(&e,
                       "(nthcdr 21 (range 0 20))",
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
    TL_TEST(repl_test(&e, "(/ 2 0)", "(error \"[/] divide by zero error\")"));
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


    TL_TEST(repl_test(&e, "(< 1 2)", "T"));
    TL_TEST(repl_test(&e, "(< 1 2 5 7 9 12)", "T"));
    TL_TEST(repl_test(&e, "(> 1 2 5 7 9 12)", "NIL"));
    TL_TEST(repl_test(&e, "(> 1 2 5 7 9 12)", "NIL"));
    TL_TEST(repl_test(&e, "(> 4 2 3)", "NIL"));
    TL_TEST(repl_test(&e, "(< 4 2 3)", "NIL"));
    TL_TEST(repl_test(&e, "(< 'sym1 'sym2 3)", "(error \"[<] can only compare values\")"));
    TL_TEST(repl_test(&e, "(> 'sym1 'sym2 3)",  "(error \"[>] can only compare values\")"));
}

void
test_conditionals(void)
{
    yal::Environment e;
    e.load_core();

    TL_TEST(repl_test(&e, "(if (= 2 2) 4)", "4"));
    TL_TEST(repl_test(&e, "(if (= 2 2) 4 0)", "4"));
    TL_TEST(repl_test(&e, "(if t 2)", "2"));
    TL_TEST(repl_test(&e, "(if '(1 2 3) t)", "T"));
    TL_TEST(repl_test(&e, "(if nil 2 3)", "3"));
    //TL_TEST(repl_test(&e, "(if () t nil)", "NIL"));
    TL_TEST(repl_test(&e, "(if (NIL) t nil)", "NIL"));
    TL_TEST(repl_test(&e, "(if (= 2 3) t nil)", "NIL"));
    TL_TEST(repl_test(&e, "(if (= 2 3) 1 0)", "0"));

//   TL_TEST(repl_test(&e, 
//                     "(fn is10 (v) "
//                     "  (if (= v 10) t nil))",
//                     "is10"));
//
//   TL_TEST(repl_test(&e, 
//                      "(is10 7)",
//                      "NIL"));
//
//   TL_TEST(repl_test(&e, 
//                      "(is10 10)",
//                      "T"));
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

void
test_variables(void)
{
    yal::Environment e;
    e.load_core();

    TL_TEST(repl_test(&e, "(local! player-name \"player1\")", "player-name"));
    TL_TEST(repl_test(&e, "(local! health (+ 2 2))", "health"));
    TL_TEST(repl_test(&e, "(local! weapons '(pistol lazergun shotgun))", "weapons"));

    TL_TEST(repl_test(&e, "player-name", "\"player1\""));
    TL_TEST(repl_test(&e, "health", "4"));
    TL_TEST(repl_test(&e, "weapons", "(pistol lazergun shotgun)"));

    TL_TEST(repl_test(&e, "(local! thislist [1 2 3])", "thislist"));
    TL_TEST(repl_test(&e, "(local! my/list [2 (+ 3 3)])", "my/list"));

    TL_TEST(repl_test(&e, "thislist", "(1 2 3)"));
    TL_TEST(repl_test(&e, "my/list", "(2 6)"));

    TL_TEST(repl_test(&e, "(local! my/var 10)", "my/var"));
    TL_TEST(repl_test(&e, "(* my/var 2)", "20"));
}

void
test_set_variables(void)
{
    yal::Environment e;
    e.load_core();

    TL_TEST(repl_test(&e, "(const! max-health 10)", "max-health"));
    TL_TEST(repl_test(&e, "(local! health (+ 2 2))", "health"));
    TL_TEST(repl_test(&e, "(set! health (- health 1))", "3"));
    TL_TEST(repl_test(&e, "health", "3"));
    TL_TEST(repl_test(&e, "(set! health max-health)", "10"));
    TL_TEST(repl_test(&e, "(set! health (- health 1))", "9"));
    TL_TEST(repl_test(&e, "(set! invalid-name 7)",
                          "(error \"[setvar!] variable to set does not exist\")"));
    TL_TEST(repl_test(&e, "(set! health)", "NIL"));
}

void
test_try_catch_throw(void)
{
    yal::Environment e;
    e.load_core();

    TL_TEST(repl_test(&e, "(throw \"some error!\")", "(error \"[throw] some error!\")"));

    TL_TEST(repl_test(&e, "(try (throw \"myerr!\") "
                          "    )",
                          "(error \"[throw] myerr!\")"));

    TL_TEST(repl_test(&e, "(try (throw \"myerr!\") "
                          "    err err)",
                          "(error \"[throw] myerr!\")"));

    TL_TEST(repl_test(&e, "(try (throw \"shit-hit-the-fan\") "
                          "    err ['caught-error err])",
                          "(caught-error (error \"[throw] shit-hit-the-fan\"))"));

    TL_TEST(repl_test(&e, "(try (throw \"shit-hit-the-fan\") "
                          "    err (+ 2 2)",
                          "4"));

    TL_TEST(repl_test(&e, "(try (set! myvar 10) "
                          "    err err)",
                          "(error \"[setvar!] variable to set does not exist\")"));

    TL_TEST(repl_test(&e, "(try (/ 2 0) "
                          "    err err)",
                          "(error \"[/] divide by zero error\")"));

    TL_TEST(repl_test(&e, "(try (/ 2 0) "
                          "    err (/ 0 2)",
                          "0"));
}

void
test_predicates(void)
{
    yal::Environment e;
    e.load_core();

    TL_TEST(repl_test(&e, "(nil? 2)", "NIL"));
    TL_TEST(repl_test(&e, "(nil? NIL)", "T"));

    TL_TEST(repl_test(&e, "(real? 2)", "T"));
    TL_TEST(repl_test(&e, "(real? mysym)", "NIL"));

    TL_TEST(repl_test(&e, "(decimal? 2)", "NIL"));
    TL_TEST(repl_test(&e, "(decimal? 2.43)", "T"));
    TL_TEST(repl_test(&e, "(decimal? \"mysym\")", "NIL"));

    TL_TEST(repl_test(&e, "(value? 2)", "T"));
    TL_TEST(repl_test(&e, "(value? 2.45)", "T"));
    TL_TEST(repl_test(&e, "(value? mysym)", "NIL"));

    TL_TEST(repl_test(&e, "(symbol? 2)", "NIL"));
    TL_TEST(repl_test(&e, "(symbol? mysym)", "T"));

    TL_TEST(repl_test(&e, "(string? 2.43)", "NIL"));
    TL_TEST(repl_test(&e, "(string? \"mysym\")", "T"));

    TL_TEST(repl_test(&e, "(list? 2.43)", "NIL"));
    TL_TEST(repl_test(&e, "(list? (1 2 3))", "T"));

    TL_TEST(repl_test(&e, "(var myvar '(1 2))", "myvar"));

    TL_TEST(repl_test(&e, "(const? PI)", "T"));
    TL_TEST(repl_test(&e, "(const? (1 2 3))", "NIL"));
    TL_TEST(repl_test(&e, "(const? myvar)", "NIL"));

    TL_TEST(repl_test(&e, "(var? PI)", "NIL"));
    TL_TEST(repl_test(&e, "(var? (1 2 3))", "NIL"));
    TL_TEST(repl_test(&e, "(var? myvar)", "T"));

    TL_TEST(repl_test(&e, "(global! newvar1 23)", "newvar1"));
    TL_TEST(repl_test(&e, "(local! newvar2 5)", "newvar2"));
    TL_TEST(repl_test(&e, "(var? newvar1)", "T"));
    TL_TEST(repl_test(&e, "(var? newvar2)", "T"));

    TL_TEST(repl_test(&e, "(const? newvar1)", "NIL"));
    TL_TEST(repl_test(&e, "(const? newvar2)", "NIL"));
    TL_TEST(repl_test(&e, "(const? PI2)", "T"));
    TL_TEST(repl_test(&e, "(const? E)", "T"));

    TL_TEST(repl_test(&e, "(fn? +)", "T"));
    TL_TEST(repl_test(&e, "(fn? PI)", "NIL"));

    TL_TEST(repl_test(&e,
                      "(fn! factorial (v)"
                      "  (if (= v 0) "
                      "    1"
                      "    (* v (factorial (- v 1)))"
                      "))",
                      "factorial"));
	
    TL_TEST(repl_test(&e, "(fn? factorial)", "T"));

    TL_TEST(repl_test(&e, "(macro? +)", "NIL"));
    TL_TEST(repl_test(&e, "(macro? PI)", "NIL"));

    TL_TEST(repl_test(&e,
                      "(macro! same (v)"
                      "v)",
                      "same"));
	
    TL_TEST(repl_test(&e, "(macro? same)", "T"));


}

void
test_apply(void)
{
    yal::Environment e;
    e.load_core();

    TL_TEST(repl_test(&e, "(apply '- '(5 2))", "3"));
    TL_TEST(repl_test(&e, "(apply '+ (range 1 10))", "55"));
}

void
test_lambda(void)
{
    yal::Environment e;
    e.load_core();

    TL_TEST(repl_test(&e, "(lambda (a) a)", "#<lambda>"));
    TL_TEST(repl_test(&e, "((lambda (a) a) 12)", "12"));
    TL_TEST(repl_test(&e, "((lambda () 5))", "5"));
    TL_TEST(repl_test(&e, "(lambda () 5)", "#<lambda>"));
    TL_TEST(repl_test(&e, "((lambda () '(1 1 1)))", "(1 1 1)"));
    TL_TEST(repl_test(&e, "((lambda (n) [n 2 3]) 1)", "(1 2 3)"));
    TL_TEST(repl_test(&e, "((lambda (n) [n 2 3]) 'hi!)", "(hi! 2 3)"));
    TL_TEST(repl_test(&e, "((lambda (a) (+ a 10)) 4)", "14"));
    TL_TEST(repl_test(&e, "((lambda (b c) (+ b c)) 4 6)", "10"));
    TL_TEST(repl_test(&e, "((lambda (a b) (+ a b)) 10 20)", "30"));
    TL_TEST(repl_test(&e, "((lambda () (* 5 3)))", "15"));
    TL_TEST(repl_test(&e, "((lambda (a b) (* a b)) 4 6)", "24"));
    TL_TEST(repl_test(&e,
                      "((lambda (a b c) "
                      "  (local! s (* a b))"
                      "  (+ s c))"
                      "2 4 3)",
                      "11"));

    TL_TEST(repl_test(&e,
                      "((lambda (a) "
                      "  (return '(early-return))"
                      "  (* a a))"
                      "2)",
                      "(early-return)"));

    TL_TEST(repl_test(&e,
                      "((lambda (a) "
                      "  (return)"
                      "  (* a a))"
                      "2)",
                      "NIL"));


}

void
test_fn(void)
{
    yal::Environment e;
    e.load_core();

    TL_TEST(repl_test(&e, 
                       "(fn! same (a) a)",
                       "same"));

    TL_TEST(repl_test(&e, 
                       "(same 12)",
                       "12"));

    TL_TEST(repl_test(&e, 
                       "(fn! same2 (&all) all)",
                       "same2"));

    TL_TEST(repl_test(&e, 
                       "(same2 'a 'b 'c 'd)",
                       "(a b c d)"));

    TL_TEST(repl_test(&e, 
                       "(fn! add2 (a b) (+ a b))",
                       "add2"));

    TL_TEST(repl_test(&e, 
                       "(add2 5 10)",
                       "15"));

    TL_TEST(repl_test(&e, 
                       "(add2 5 (+ 5 5))",
                       "15"));

    TL_TEST(repl_test(&e, 
                       "(fn! move (this scale offset) (add2 offset (* this scale)))",
                       "move"));

    TL_TEST(repl_test(&e, 
                       "(move 5 10 3)",
                       "53"));

    TL_TEST(repl_test(&e, 
                      "(fn! with-docstring (a b) "
                      "\"This is my optional documentation string for my function!\""
                      "(+ a b))",
                       "with-docstring"));

    TL_TEST(repl_test(&e, 
                       "(with-docstring 5 10)",
                       "15"));


    TL_TEST(repl_test(&e, 
                       "(fn! do-transform (a &transforms) (apply '+ (cons a transforms)))",
                       "do-transform"));

    TL_TEST(repl_test(&e, 
                       "(do-transform 2 5 3)",
                       "10"));
}

void
test_functions_and_recursion(void)
{
    yal::Environment e;
    e.load_core();
    /*
(defun factorial (n)
  (if (= n 0)
      1
      (* n (factorial (- n 1))) ) )
    */

    TL_TEST(repl_test(&e, 
                      "(fn! factorial (v)"
                      "  (if (= v 0) "
                      "    1"
                      "    (* v (factorial (- v 1)))"
                      "))",
                      "factorial"));

    TL_TEST(repl_test(&e, 
                       "(factorial 5)",
                       "120"));
    TL_TEST(repl_test(&e, 
                       "(factorial 3)",
                       "6"));
    TL_TEST(repl_test(&e, 
                       "(factorial 10)",
                       "3628800"));


    //TL_TEST(repl_test(&e, 
    //                  "(fn! list-len (list)"
    //                  "  (if list "
    //                  "    (+ 1 (list-len (cdr list)))"
    //                  "    -1"
    //                  "))",
    //                  "list-len"));

    TL_TEST(repl_test(&e, 
                       "(list-len '(1 2 3))",
                       "3"));
    TL_TEST(repl_test(&e, 
                       "(list-len [1 (+ 2 2) 3])",
                       "3"));
    TL_TEST(repl_test(&e, 
                       "(list-len '(1 ((((3))))))",
                       "2"));

    TL_TEST(repl_test(&e, 
                      "(fn! isin (v l)"
                      "  (if l"
                      "     (if (eq (car l) v)"
                      "      t (isin (cdr v) l))"
                      "nil))",
                      "isin"));

    /*
    TL_TEST(repl_test(&e, 
                       "(isin 3 '(1 2 3 4))",
                       "t"));

     TL_TEST(repl_test(&e, 
                       "(isin 5 '(1 2 3 4))",
                       "nil"));
    */
    /*https://www.omnicalculator.com/math/binomial-coefficient*/
    TL_TEST(repl_test(&e, 
                      "(fn! bin-coeff (n k)"
                      "(/ (factorial n) (factorial (- n k)) (factorial k)"
                      ")",
                      "bin-coeff"));

     TL_TEST(repl_test(&e, 
                       "(bin-coeff 5 2)",
                       "10"));

     TL_TEST(repl_test(&e, 
                       "(bin-coeff 5 4)",
                       "5"));
     /*
     TL_TEST(repl_test(&e, 
                       "(bin-coeff 22 13)",
                       "497.4200000"));
      */
}

void
test_slurp_read_eval(void)
{
    yal::Environment e;
    e.load_core();

    TL_TEST(repl_test(&e, "(slurp-file \"../test_pow.yal\")",
                          "\"(fn! pow (a) (* a a))\""));

    TL_TEST(repl_test(&e, "(read (slurp-file \"../test_pow.yal\"))",
                          "(fn! pow (a) (* a a))"));

    TL_TEST(repl_test(&e, "(eval (read (slurp-file \"../test_pow.yal\")))",
                          "pow"));

    TL_TEST(repl_test(&e, "(pow 2)", "4"));

    /*NOTE: this is now done in load_core*/
    //TL_TEST(repl_test(&e, "(fn! load-file (f) (eval (read (slurp-file f))))", "load-file"));

    TL_TEST(repl_test(&e, "(load-file \"../test_factorial.yal\")", "factorial"));

    TL_TEST(repl_test(&e, 
                       "(factorial 5)",
                       "120"));
    TL_TEST(repl_test(&e, 
                       "(factorial 3)",
                       "6"));
    TL_TEST(repl_test(&e, 
                       "(factorial 10)",
                       "3628800"));
}

void
test_libraries(void)
{
    yal::Environment e;
    e.load_core();

    TL_TEST(repl_test(&e, "(load-file \"../math.yal\")", "math"));

    TL_TEST(repl_test(&e, "(pow 2)", "4"));

    TL_TEST(repl_test(&e, 
                       "(factorial 5)",
                       "120"));

    TL_TEST(repl_test(&e, 
                       "(factorial 3)",
                       "6"));

    TL_TEST(repl_test(&e, 
                       "(factorial 10)",
                       "3628800"));

     TL_TEST(repl_test(&e, 
                       "(bin-coeff 5 2)",
                       "10"));

     TL_TEST(repl_test(&e, 
                       "(bin-coeff 5 4)",
                       "5"));

}

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

    TL(test_sizes());
    TL(test_types_creation());
    TL(test_read());
    TL(test_nilp());
    TL(test_len());
    TL(test_assoc());
    TL(test_globals());
    TL(test_ipreverse());
    TL(test_simple_eval());
    TL(test_lex_types());
    TL(test_buildin_range());
    TL(test_buildin_accessors());
    TL(test_buildin_list_creation());
    TL(test_buildin_math());
    TL(test_buildin_progn());
    TL(test_conditionals());
    TL(test_buildin_equality());
    TL(test_variables());
    TL(test_set_variables());
    TL(test_apply());
    TL(test_try_catch_throw());
    TL(test_predicates());
    TL(test_lambda());
    TL(test_fn());
    TL(test_functions_and_recursion());
    TL(test_slurp_read_eval());
    TL(test_libraries());

    tl_summary();
}
