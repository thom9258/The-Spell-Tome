#include <iostream>
#include <variant>

#define YALCPP_IMPLEMENTATION
#include "../yal.hpp"

#include "testlib.h"

const char* std_path = "../../std.yal";

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

struct cppExpr;

struct Callable {
    cppExpr* binds;
    cppExpr* body;
};

struct Cons {
    cppExpr* car;
    cppExpr* cdr;
};

struct cppExpr {
    std::uint8_t flags;
    std::variant<int,
                 float,
                 yal::BuildinFn,
                 Callable,
                 Cons>value;
};

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

    std::cout << "sizeof iostream = " << sizeof(std::iostream) << std::endl; 
    std::cout << "sizeof cppExpr = " << sizeof(cppExpr) << std::endl; 
    std::cout << "sizeof yal::Expr = " << sizeof(yal::Expr) << std::endl; 
    
    TL_TEST(sizeof(cppExpr) > sizeof(yal::Expr));
}

void
test_types_creation(void)
{
    yal::Environment e;
    yal::Expr* real = e.real(4);
    TL_TEST(real->type == yal::TYPE_REAL);
    TL_TEST(real->real == 4);
    std::string realstr = yal::stringify(real);
    std::cout << realstr << std::endl;
    TL_TEST(realstr == "4");

    yal::Expr* deci = e.decimal(4.3f);
    TL_TEST(deci->type == yal::TYPE_DECIMAL);
    TL_TEST(deci->decimal == 4.3f);
    std::string decistr = yal::stringify(deci);
    std::cout << decistr << std::endl;
    TL_TEST(decistr == "4.3");

    yal::Expr* cns = e.cons(real, deci);
    TL_TEST(cns->type == yal::TYPE_CONS);
    TL_TEST(cns->cons.car == real);
    TL_TEST(cns->cons.cdr == deci);
    std::string cnsstr = yal::stringify(cns);
    std::cout << cnsstr << std::endl;
    TL_TEST(cnsstr == "(4 . 4.3)");

    yal::Expr* str = e.string("mystr");
    TL_TEST(str->type == yal::TYPE_STRING);
    TL_TEST(str->string == std::string("mystr"));
    std::string strstr = yal::stringify(str);
    std::cout << strstr << std::endl;
    TL_TEST(strstr == "\"mystr\"");

    yal::Expr* sym = e.symbol("mysym");
    TL_TEST(sym->type == yal::TYPE_SYMBOL);
    TL_TEST(sym->symbol == std::string("mysym"));
    std::string symstr = yal::stringify(sym);
    std::cout << symstr << std::endl;
    TL_TEST(symstr == "mysym");

    yal::Expr* lst = e.list({e.symbol("*"), e.real(2), e.decimal(3.14)});
    TL_TEST(lst->type == yal::TYPE_CONS);
    std::string lststr = yal::stringify(lst);
    std::cout << lststr << std::endl;
    TL_TEST(lststr == std::string("(* 2 3.14)"));

    yal::Expr* lst2 = e.list({e.symbol("+"), e.real(5), e.list({e.symbol("*"), e.real(4), e.decimal(2.5)})});
    std::string lst2str = yal::stringify(lst2);
    std::cout << lst2str << std::endl;
    TL_TEST(lst2str == std::string("(+ 5 (* 4 2.5))"));

    std::cout << e.gc_info() << std::endl;
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

    std::cout << e.gc_info() << std::endl;
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

    std::cout << e.gc_info() << std::endl;
}

void
test_globals(void)
{
    yal::Environment e;
    const float pi = 3.141592; 

    e.add_global("*Creator*", e.string("Thomas Alexgaard"));
    e.add_global("PI", e.decimal(pi));

    yal::Expr* constant = e.global_scope()->variable_get("PI");
    std::cout << "PI: " << yal::stringify(constant) << std::endl;
    TL_TEST(yal::stringify(constant) == "(3.14159)");

    constant = e.global_scope()->variable_get("*Creator*");
    std::cout << "Creator: " << yal::stringify(constant) << std::endl;
    TL_TEST(yal::stringify(constant) == "(\"Thomas Alexgaard\")");

    e.add_global("*Creator-github*", e.string("https://github.com/thom9258/"));
    e.add_global("*Host-Languange*", e.string("C++"));
    e.add_global("*Version*", e.list({e.real(0), e.real(1)}));
    e.add_global("PI/2", e.decimal(pi/2));
    e.add_global("PI2", e.decimal(pi*2));

    std::cout << e.gc_info() << std::endl;
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

    std::cout << e.gc_info() << std::endl;
}

void
test_simple_eval(void)
{
    yal::Expr* program;
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

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

    std::cout << e.gc_info() << std::endl;
}

void
test_lex_types(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

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

    std::cout << e.gc_info() << std::endl;
}

void
test_read(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);
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
    
    TL_TEST(read_test(&e, "; mymath\n(+ 2 2)", "(+ 2 2)"));
    

    TL_TEST(read_test(&e, "; mymath\n"
                          "(- 1 8) ; is dahm hard!\n",
                      "(- 1 8)"));

    std::cout << e.gc_info() << std::endl;
}

void
test_buildin_list_creation(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);
    TL_TEST(repl_test(&e, "(quote (foo bar (quote (baz))))", "(foo bar (quote (baz)))"));
    TL_TEST(repl_test(&e, "'(foo bar '(baz))", "(foo bar (quote (baz)))"));
    TL_TEST(repl_test(&e, "(quote (1 2 3 4))", "(1 2 3 4)"));

    /*TODO: locate why this error is there*/
    //TL_TEST(repl_test(&e, "'(1 2 '(3 4) (quote 5))", "(1 2 (quote (3 4)) (quote 5))"));

    TL_TEST(repl_test(&e, "()", "NIL"));
    TL_TEST(repl_test(&e, "[]", "NIL"));

    TL_TEST(repl_test(&e, "(quote ()  )", "NIL"));
    TL_TEST(repl_test(&e, "(quote a b)", "(error \"[quote] expected 1 argument, not\" (a b))"));
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

    std::cout << e.gc_info() << std::endl;
}


void
test_buildin_list_management(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);
    TL_TEST(repl_test(&e,
                      "(put 0 (list 1 2 3))",
                      "(0 1 2 3)"));
    TL_TEST(repl_test(&e,
                      "(put nil '(t nil t))",
                      "(NIL T NIL T)"));

    TL_TEST(repl_test(&e,
                      "(put \"myname\" '(23 m))",
                      "(\"myname\" 23 m)"));

    std::cout << e.gc_info() << std::endl;
}

void
test_buildin_range(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);
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

    std::cout << e.gc_info() << std::endl;
}

void
test_buildin_accessors(void)
{

    yal::Environment e;
    e.load_core();
    e.load_file(std_path);
    TL_TEST(repl_test(&e,
                       "(car '(1 2 3 4))",
                       "1"));

    TL_TEST(repl_test(&e,
                       "(cdr '(1 2 3 4))",
                       "(2 3 4)"));

    std::cout << e.gc_info() << std::endl;
}

void
test_buildin_math(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

    TL_TEST(repl_test(&e, "(+)", "0"));
    TL_TEST(repl_test(&e, "(-)", "0"));
    TL_TEST(repl_test(&e, "(*)", "1"));
    TL_TEST(repl_test(&e, "(/)", "1"));
    TL_TEST(repl_test(&e, "(+ 1 2)", "3"));
    TL_TEST(repl_test(&e, "(- 7 3)", "4"));
    TL_TEST(repl_test(&e, "(* 4 3)", "12"));
    TL_TEST(repl_test(&e, "(/ 8 2)", "4"));
    TL_TEST(repl_test(&e, "(/ 0 2)", "0"));
    TL_TEST(repl_test(&e, "(/ 2 0)", "(error \"[/] divide by zero\" (2 0))"));
    TL_TEST(repl_test(&e, "(+ 2 (- 5 6) 1)", "2"));
    TL_TEST(repl_test(&e, "(+ 4 (* 2 6) (- 10 5))", "21"));
    TL_TEST(repl_test(&e, "(- 5 2)", "3"));
    TL_TEST(repl_test(&e, "(/ (- (+ 515 (* -87 311)) 296) 27)", "-994"));

    std::cout << e.gc_info() << std::endl;
}

void
test_buildin_equality(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

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
    TL_TEST(repl_test(&e, "(< 'sym1 'sym2 3)", "(error \"[<] can only compare values, not\" sym1)"));
    TL_TEST(repl_test(&e, "(> 'sym1 'sym2 3)",  "(error \"[>] can only compare values, not\" sym1)"));

    std::cout << e.gc_info() << std::endl;
}

void
test_conditionals(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);


    TL_TEST(repl_test(&e, "(if (= 2 2) 4)", "4"));
    TL_TEST(repl_test(&e, "(if (= 2 2) 4 0)", "4"));
    TL_TEST(repl_test(&e, "(if t 2)", "2"));
    TL_TEST(repl_test(&e, "(if '(1 2 3) t)", "T"));
    TL_TEST(repl_test(&e, "(if nil 2 3)", "3"));
    TL_TEST(repl_test(&e, "(if () t nil)", "NIL"));
    TL_TEST(repl_test(&e, "(if NIL t nil)", "NIL"));
    TL_TEST(repl_test(&e, "(if (= 2 3) t nil)", "NIL"));
    TL_TEST(repl_test(&e, "(if (= 2 3) 1 0)", "0"));

    TL_TEST(repl_test(&e, 
                      "(fn! is10 (v) "
                      "  (if (= v 10) t nil))",
                      "is10"));
    
    TL_TEST(repl_test(&e, 
                      "(is10 7)",
                      "NIL"));
    
    TL_TEST(repl_test(&e, 
                      "(is10 10)",
                      "T"));

    TL_TEST(repl_test(&e, "(_OR2 t nil)", "T"));
    TL_TEST(repl_test(&e, "(_OR2 NIL T)", "T"));
    TL_TEST(repl_test(&e, "(_OR2 t T)", "T"));
    TL_TEST(repl_test(&e, "(_OR2 nil NIL)", "NIL"));

    TL_TEST(repl_test(&e, "(or t nil)", "T"));
    TL_TEST(repl_test(&e, "(or NIL T)", "T"));
    TL_TEST(repl_test(&e, "(or t T)", "T"));
    TL_TEST(repl_test(&e, "(or nil NIL)", "NIL"));

    TL_TEST(repl_test(&e, "(_AND2 t nil)", "NIL"));
    TL_TEST(repl_test(&e, "(_AND2 NIL T)", "NIL"));
    TL_TEST(repl_test(&e, "(_AND2 nil NIL)", "NIL"));
    TL_TEST(repl_test(&e, "(_AND2 t T)", "T"));

    TL_TEST(repl_test(&e, "(and t nil)", "NIL"));
    TL_TEST(repl_test(&e, "(and NIL T)", "NIL"));
    TL_TEST(repl_test(&e, "(and nil NIL)", "NIL"));
    TL_TEST(repl_test(&e, "(and t T)", "T"));

    std::cout << e.gc_info() << std::endl;
}

void
test_variables(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

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

    std::cout << e.gc_info() << std::endl;
}

void
test_set_variables(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

    TL_TEST(repl_test(&e, "(global! mycons (cons 'a 'b))", "mycons"));
    TL_TEST(repl_test(&e, "(setcar! mycons 'c)", "c"));
    TL_TEST(repl_test(&e, "mycons", "(c . b)"));
    TL_TEST(repl_test(&e, "(setcdr! mycons 'c)", "c"));
    TL_TEST(repl_test(&e, "mycons", "(c . c)"));
    TL_TEST(repl_test(&e, "(setcar! mycons 'car)", "car"));
    TL_TEST(repl_test(&e, "(setcdr! mycons 'cdr)", "cdr"));
    TL_TEST(repl_test(&e, "mycons", "(car . cdr)"));
    TL_TEST(repl_test(&e, "(setcar! 'notcons 'a)",
                          "(error \"[setcar!] Expected cons to modify, got\" notcons)"));
    TL_TEST(repl_test(&e, "(setcdr! 'notcons 'a)",
                          "(error \"[setcdr!] Expected cons to modify, got\" notcons)"));

    TL_TEST(repl_test(&e, "(global! max-health 10)", "max-health"));
    TL_TEST(repl_test(&e, "(variable-definition 'max-health)", "(10)"));
    TL_TEST(repl_test(&e, "(local! health (+ 2 2))", "health"));
    TL_TEST(repl_test(&e, "(set! 'health (- health 1))", "3"));
    TL_TEST(repl_test(&e, "health", "3"));
    TL_TEST(repl_test(&e, "(set! 'health max-health)", "10"));
    TL_TEST(repl_test(&e, "(set! 'health (- health 1))", "9"));
    TL_TEST(repl_test(&e, "(set! 'invalid-name 7)",
                          "(error \"set! expected symbol variable, not\" invalid-name)"));
    TL_TEST(repl_test(&e, "(set! 'PI 7)", "7"));
    /*TODO: how do we allow setting something to nil but only when explicitly supplied as input?*/
    TL_TEST(repl_test(&e, "(set! 'health)", "NIL"));
    TL_TEST(repl_test(&e, "health", "9"));

    std::cout << e.gc_info() << std::endl;
}

void
test_try_catch_throw(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

    TL_TEST(repl_test(&e, "(throw \"some error!\")", "(error \"some error!\")"));

    TL_TEST(repl_test(&e, "(try (throw \"another error!\") "
                          "    err err)",
                          "(error \"another error!\")"));

    TL_TEST(repl_test(&e, "(try (throw \"myerr!\") "
                          "    err err)",
                          "(error \"myerr!\")"));

    TL_TEST(repl_test(&e, "(try (throw \"shit-hit-the-fan\") "
                          "    err ['caught-error err])",
                          "(caught-error (error \"shit-hit-the-fan\"))"));

    TL_TEST(repl_test(&e, "(try (throw \"shit-hit-the-fan\") "
                          "    err (+ 2 2)",
                          "4"));

    //TL_TEST(repl_test(&e, "(try (set! myvar 10) "
    //                      "    err err)",
    //                      "(error \"[set!] variable to set does not exist\")"));

    TL_TEST(repl_test(&e, "(try (/ 2 0)"
                          "    err err)",
                          "(error \"[/] divide by zero\" (2 0))"));

    TL_TEST(repl_test(&e, "(try (/ 2 0) "
                          "    err (/ 0 2)",
                          "0"));

    std::cout << e.gc_info() << std::endl;
}

void
test_predicates(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

    TL_TEST(repl_test(&e, "(typeof 2)", "real"));
    TL_TEST(repl_test(&e, "(typeof 2.34)", "decimal"));
    TL_TEST(repl_test(&e, "(typeof \"mystr\")", "string"));
    TL_TEST(repl_test(&e, "(typeof 'mysym)", "symbol"));
    TL_TEST(repl_test(&e, "(typeof '(1 2 3))", "cons"));

    TL_TEST(repl_test(&e, "(nil? 2)", "NIL"));
    TL_TEST(repl_test(&e, "(nil? 'NIL)", "T"));
    TL_TEST(repl_test(&e, "(nil? NIL)", "T"));
    TL_TEST(repl_test(&e, "(nil? ())", "T"));
    TL_TEST(repl_test(&e, "(nil?)", "T"));

    TL_TEST(repl_test(&e, "(real? 2)", "T"));
    TL_TEST(repl_test(&e, "(real? 'mysym)", "NIL"));

    TL_TEST(repl_test(&e, "(decimal? 2)", "NIL"));
    TL_TEST(repl_test(&e, "(decimal? 2.43)", "T"));
    TL_TEST(repl_test(&e, "(decimal? \"mysym\")", "NIL"));

    TL_TEST(repl_test(&e, "(value? 2)", "T"));
    TL_TEST(repl_test(&e, "(value? 2.45)", "T"));
    TL_TEST(repl_test(&e, "(value? 'mysym)", "NIL"));

    TL_TEST(repl_test(&e, "(symbol? 2)", "NIL"));
    TL_TEST(repl_test(&e, "(symbol? 'mysym)", "T"));

    TL_TEST(repl_test(&e, "(string? 2.43)", "NIL"));
    TL_TEST(repl_test(&e, "(string? \"mysym\")", "T"));

    TL_TEST(repl_test(&e, "(list? 2.43)", "NIL"));
    TL_TEST(repl_test(&e, "(list? '(1 2 3))", "T"));
    //TL_TEST(repl_test(&e, "(list? '())", "T"));
    TL_TEST(repl_test(&e, "(list? 'NIL)", "T"));
    //TL_TEST(repl_test(&e, "(cons? '())", "NIL"));
    //TL_TEST(repl_test(&e, "(cons? 'NIL)", "NIL"));

    TL_TEST(repl_test(&e, "(atom? 2.43)", "T"));
    TL_TEST(repl_test(&e, "(atom? '(1 2 3))", "NIL"));
    TL_TEST(repl_test(&e, "(atom? 'NIL)", "T"));


    TL_TEST(repl_test(&e, "(global! myvar '(1 2))", "myvar"));

    TL_TEST(repl_test(&e, "(var? 'PI)", "T"));
    TL_TEST(repl_test(&e, "(var? 'PI2)", "T"));
    TL_TEST(repl_test(&e, "(var? 'PI/2)", "T"));
    TL_TEST(repl_test(&e, "(var? 'PI/3)", "T"));
    TL_TEST(repl_test(&e, "(var? 'PI/4)", "T"));
    TL_TEST(repl_test(&e, "(var? 'PI/5)", "NIL"));
    TL_TEST(repl_test(&e,
                      "(var? '(1 2 3))",
                      "(error \"[variable-definition] expects symbol variable, but got\" (1 2 3))"));
    TL_TEST(repl_test(&e, "(var? 'myvar)", "T"));
    TL_TEST(repl_test(&e, "(global! newvar1 23)", "newvar1"));
    TL_TEST(repl_test(&e, "(local! newvar2 5)", "newvar2"));
    TL_TEST(repl_test(&e, "(var? 'newvar1)", "T"));
    TL_TEST(repl_test(&e, "(var? 'newvar2)", "T"));

    //TL_TEST(repl_test(&e, "(const? 'PI)", "T"));
    //TL_TEST(repl_test(&e, "(const? '(1 2 3))", "NIL"));
    //TL_TEST(repl_test(&e, "(const? 'myvar)", "NIL"));

    

    //TL_TEST(repl_test(&e, "(const? 'newvar1)", "NIL"));
    //TL_TEST(repl_test(&e, "(const? 'newvar2)", "NIL"));
    //TL_TEST(repl_test(&e, "(const? 'PI2)", "T"));
    //TL_TEST(repl_test(&e, "(const? 'E)", "T"));
    
    //TL_TEST(repl_test(&e, "(fn? +)", "T"));
    //TL_TEST(repl_test(&e, "(fn? PI)", "NIL"));
    //
    //TL_TEST(repl_test(&e,
    //                  "(fn! factorial (v)"
    //                  "  (if (= v 0) "
    //                  "    1"
    //                  "    (* v (factorial (- v 1)))"
    //                  "))",
    //                  "factorial"));
    //
    //TL_TEST(repl_test(&e, "(fn? factorial)", "T"));
    //
    //TL_TEST(repl_test(&e, "(macro? +)", "NIL"));
    //TL_TEST(repl_test(&e, "(macro? PI)", "NIL"));
    //
    //TL_TEST(repl_test(&e,
    //                  "(macro! same (v)"
    //                  "v)",
    //                  "same"));
    //
    //TL_TEST(repl_test(&e, "(macro? same)", "T"));

    std::cout << e.gc_info() << std::endl;
}

void
test_lambda(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

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

    std::cout << e.gc_info() << std::endl;
}

void
test_fn(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

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

    std::cout << e.gc_info() << std::endl;
}

void
test_functions_and_recursion(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);
    
    TL_TEST(repl_test(&e, 
                       "(len '(1 2 3))",
                       "3"));
    TL_TEST(repl_test(&e, 
                       "(len [1 (+ 2 2) 3])",
                       "3"));
    TL_TEST(repl_test(&e, 
                       "(len '(1 ((((3))))))",
                       "2"));

    TL_TEST(repl_test(&e, 
                      "(fn! isin (v l)"
                      "  (if (nil? l)"
                      "     (if (eq (car l) v)"
                      "       (isin (cdr v) l))"
                      "  nil))",
                      "isin"));

    TL_TEST(repl_test(&e, 
                       "(contains 3 '(1 2 3 4))",
                       "T"));

     TL_TEST(repl_test(&e, 
                       "(contains 5 '(1 2 3 4))",
                       "NIL"));

    std::cout << e.gc_info() << std::endl;
}

void
test_full_circle(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

    /*NOTE: I added a \n at the end here but it should not be there*/
    TL_TEST(repl_test(&e, "(slurp-file \"../test_pow.yal\")",
                          "\"(fn! pow (a) (* a a))\n\""));

    TL_TEST(repl_test(&e, "(read (slurp-file \"../test_pow.yal\"))",
                          "(fn! pow (a) (* a a))"));

    TL_TEST(repl_test(&e, "(eval (read (slurp-file \"../math.yal\")))",
                          "math"));

    TL_TEST(repl_test(&e, "(stringify '(+ 2 2))", "\"(+ 2 2)\""));
    TL_TEST(repl_test(&e, "(read (stringify '(+ 2 2)))", "(+ 2 2)"));
    TL_TEST(repl_test(&e, "(eval (read (stringify '(+ 2 2))))", "4"));

    std::cout << e.gc_info() << std::endl;
}

void
test_load_libraries(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

    TL_TEST(repl_test(&e, "(load-file \"../math.yal\")", "math"));

    TL_TEST(repl_test(&e, "(square 2)", "4"));
    TL_TEST(repl_test(&e, "(cube 2)", "8"));
    TL_TEST(repl_test(&e, "(pow 2 2)", "4"));
    TL_TEST(repl_test(&e, "(pow 2 8)", "256"));

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

    std::cout << e.gc_info() << std::endl;
}


void
test_std(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

    TL_TEST(repl_test(&e, "(not t)", "NIL"));
    TL_TEST(repl_test(&e, "(not nil)", "T"));

    TL_TEST(repl_test(&e, "(apply '- '(5 2))", "3"));
    TL_TEST(repl_test(&e, "(apply '+ (range 1 10))", "55"));
    TL_TEST(repl_test(&e, "(apply (lambda (a b) (+ a (* a b))) '(2 3))", "8"));

    TL_TEST(repl_test(&e, "(put 1 [2 3 4])", "(1 2 3 4)"));
    TL_TEST(repl_test(&e, "(put '(1 2) '((2 3) (3 4)))", "((1 2) (2 3) (3 4))"));
    TL_TEST(repl_test(&e, "(put NIL '(T NIL T)", "(NIL T NIL T)"));

    TL_TEST(repl_test(&e, "(contains '(1 2) '((1 2) (2 3) (3 4))", "T"));
    TL_TEST(repl_test(&e, "(contains 3 '(1 2 3 4))", "T"));
    TL_TEST(repl_test(&e, "(contains 5 '(1 2 3 4))", "NIL"));

    TL_TEST(repl_test(&e, "(head [1 2 3 4])", "1"));
    TL_TEST(repl_test(&e, "(tail [1 2 3 4])", "(2 3 4)"));
    TL_TEST(repl_test(&e, "(car (cdr [1 2 3 4]))", "2"));
    TL_TEST(repl_test(&e, "(car (cdr (tail  [1 2 3 4])))", "3"));

    TL_TEST(repl_test(&e, "(second [1 2 3 4])", "2"));
    TL_TEST(repl_test(&e, "(global! to10 (range 1 10))", "to10"));
    TL_TEST(repl_test(&e, "to10", "(1 2 3 4 5 6 7 8 9 10)"));

    TL_TEST(repl_test(&e, "(first to10)", "1"));
    TL_TEST(repl_test(&e, "(second to10)", "2"));
    TL_TEST(repl_test(&e, "(third to10)", "3"));
    TL_TEST(repl_test(&e, "(fourth to10)", "4"));
    TL_TEST(repl_test(&e, "(fifth to10)", "5"));
    TL_TEST(repl_test(&e, "(sixth to10)", "6"));
    TL_TEST(repl_test(&e, "(seventh to10)", "7"));
    TL_TEST(repl_test(&e, "(eighth to10)", "8"));
    TL_TEST(repl_test(&e, "(ninth to10)", "9"));
    TL_TEST(repl_test(&e, "(tenth to10)", "10"));

    TL_TEST(repl_test(&e, "(nthcdr 5 to10)", "(6 7 8 9 10)"));
    TL_TEST(repl_test(&e, "(nthcdr 8 to10)", "(9 10)"));
    TL_TEST(repl_test(&e, "(nth 7 to10)", "8"));
    TL_TEST(repl_test(&e, "(nth 5 to10)", "6"));
    /*NOTE: negative indexes returns first element*/
    TL_TEST(repl_test(&e, "(nth -7 to10)", "1"));
    TL_TEST(repl_test(&e, "(nth 0 to10)", "1"));
    TL_TEST(repl_test(&e, "(nth 23 to10)", "NIL"));

    TL_TEST(repl_test(&e, "(global! to100 (range 1 100))", "to100"));
    TL_TEST(repl_test(&e, "(nth 77 to100)", "78"));
    TL_TEST(repl_test(&e, "(nth 52 to100)", "53"));

    TL_TEST(repl_test(&e, "(last to10)", "10"));
    TL_TEST(repl_test(&e, "(last to100)", "100"));
    TL_TEST(repl_test(&e, "(last '((1 2) (3 4) (5 6)))", "(5 6)"));

    TL_TEST(repl_test(&e, "(assoc 'a '((a 1) (b 2) (c 3))", "(a 1)"));
    TL_TEST(repl_test(&e, "(assoc 'b '((a 1) (b 2) (c 3))", "(b 2)"));
    TL_TEST(repl_test(&e, "(assoc 'c '((a 1) (b 2) (c 3))", "(c 3)"));
    TL_TEST(repl_test(&e, "(assoc 'd '((a 1) (b 2) (c 3))", "NIL"));

    TL_TEST(repl_test(&e, "(progn (+ 2 2) (* 2 2) 'END)", "END"));
    TL_TEST(repl_test(&e, "(progn (- 8 6) (* 5 5))", "25"));
    TL_TEST(repl_test(&e, "(progn NIL)", "NIL"));
    /*TODO: this is a binding error,
      it does not know what to set arg var to because none is supplied*/
    //TL_TEST(repl_test(&e, "(my-progn)", "NIL"));
    TL_TEST(repl_test(&e, "(progn (+ 30 51) (* 10 3 2) (range 0 5) [1 2 3])", "(1 2 3)"));

    TL_TEST(repl_test(&e, "(abs 2.345)", "2.345"));
    TL_TEST(repl_test(&e, "(abs -72.5)", "72.5"));
    TL_TEST(repl_test(&e, "(abs (* -5 2))", "10"));

    std::cout << e.gc_info() << std::endl;
}

void
test_tco(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

    TL_TEST(repl_test(&e, "(recurse (write 1000000))", "did-we-blow-up?"));
    std::cout << e.gc_info() << std::endl;
}

void
test_setnth(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

    TL_TEST(repl_test(&e, "(global! to10 (range 0 10))", "to10"));
    TL_TEST(repl_test(&e, "(setnth! 5 'fifth to10)", "fifth"));
    TL_TEST(repl_test(&e, "(nth 5 to10)", "fifth"));
    TL_TEST(repl_test(&e, "to10", "(0 1 2 3 4 fifth 6 7 8 9 10)"));

    TL_TEST(repl_test(&e, "(setnth! 77 'seven2 to10)", "(error \"invalid index to set, got\" 77)"));
    TL_TEST(repl_test(&e, "to10", "(0 1 2 3 4 fifth 6 7 8 9 10)"));


    std::cout << e.gc_info() << std::endl;
}

void
test_get(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

    TL_TEST(repl_test(&e, "(local! mymat '((1 2 3)"
                          "                (4 5 6)"
                          "                (7 8 9)))", "mymat"));

    TL_TEST(repl_test(&e, "(get '(2 2) mymat)", "9"));
    TL_TEST(repl_test(&e, "(get '(0 1) mymat)", "2"));
    TL_TEST(repl_test(&e, "(get '(1) mymat)", "(4 5 6)"));
    std::cout << e.gc_info() << std::endl;
}

void
test_variable_definition(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

    TL_TEST(repl_test(&e, "(variable-definition 'PI)", "(3.14159)"));
    TL_TEST(repl_test(&e, "(variable-definition 'NOTPI)", "NIL"));
    TL_TEST(repl_test(&e, "(variable-value 'PI)", "3.14159"));

    std::cout << e.gc_info() << std::endl;
}

void
test_scope_management(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

    TL_TEST(repl_test(&e, "(local! a 10)", "a"));
    TL_TEST(repl_test(&e, "(scope '(progn (local! a 7) (write a) a)) ", "7"));
    TL_TEST(repl_test(&e, "a", "10"));

    TL_TEST(repl_test(&e, "(let1 '(b 7) '(progn (write b) (+ b 2)))", "9"));
    TL_TEST(repl_test(&e, "(var? 'b)", "NIL"));
    TL_TEST(repl_test(&e, "b", "NIL"));

    TL_TEST(repl_test(&e, "((lambda () (local! c 20) c))", "20"));
    TL_TEST(repl_test(&e, "c", "NIL"));


    std::cout << e.gc_info() << std::endl;
}

void
test_std_list_stuff(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

    TL_TEST(repl_test(&e, "(before-n 3 (range 1 10))", "(1 2 3)"));
    TL_TEST(repl_test(&e, "(after-n 7 (range 1 10))", "(8 9 10)"));
    TL_TEST(repl_test(&e, "(split 5 (range 1 10))", "((1 2 3 4 5) (6 7 8 9 10))"));

    TL_TEST(repl_test(&e, "(join (range 1 3) (range 4 6))", "(1 2 3 4 5 6)"));

    TL_TEST(repl_test(&e, "(transform - (range 1 5))", "(-1 -2 -3 -4 -5)"));
    TL_TEST(repl_test(&e, "(transform (lambda (x) (+ 2 x)) (range 1 5))", "(3 4 5 6 7)"));
    TL_TEST(repl_test(&e, "(transform real? [-1 2.3 4 3.2 9])", "(T NIL T NIL T)"));


    /*TODO: map not working yet*/
    //TL_TEST(repl_test(&e, "(map '+ [1 2 3] [3 4 5])", "(4 6 8)"));

    //TL_TEST(repl_test(&e, "(filter 'real? [-1 2.3 4 3.2 9])", "(-1 4 9)"));
    //TL_TEST(repl_test(&e, "(filter 'symbol? [1 'two 3 'four 5])", "(two four)"));

    std::cout << e.gc_info() << std::endl;
}

void
test_funcall(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

    TL_TEST(repl_test(&e, 
                      " (macro! funcall (fn &lst)"
                      "   \"apply function to list\""
                      //"   ['put ['variable-value fn] lst])"
                      "   [fn lst])"
                      //"   (put fn lst))"
                      , "funcall"));

    TL_TEST(repl_test(&e, "(macro-expand '(funcall + 1 2 3))", "6"));

    TL_TEST(repl_test(&e, "(funcall + 1 2 3)", "6"));
    TL_TEST(repl_test(&e, "(funcall first '(1 2 3))", "1"));

    std::cout << e.gc_info() << std::endl;
}

void
test_reducers(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

    TL_TEST(repl_test(&e, "(reduce + 0 '(1 2 3))", "6"));
    TL_TEST(repl_test(&e, "(reduce + 4 '(1 2 3))", "10"));
    TL_TEST(repl_test(&e, "(reduce * 1 '(3 8))", "24"));

    TL_TEST(repl_test(&e, "(reverse '(1 2 3))", "(3 2 1)"));

    std::cout << e.gc_info() << std::endl;
}

void
test_macros(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

    TL_TEST(repl_test(&e, "(global! v1 2)", "v1"));
    TL_TEST(repl_test(&e, "(macro! setq! (v x)"
                      "['set! 'v x])",
                      "setq!"));

    TL_TEST(repl_test(&e, "(setq! v1 5)", "5"));
    TL_TEST(repl_test(&e, "v1", "5"));
    TL_TEST(repl_test(&e, "(setq! v1 6)", "6"));
    TL_TEST(repl_test(&e, "v1", "6"));
    TL_TEST(repl_test(&e, "(macro-expand '(setq! v1 7))", "(set! (quote v1) 7)"));
    TL_TEST(repl_test(&e, "(eval (macro-expand '(setq! v1 7)))", "7"));
    TL_TEST(repl_test(&e, "v1", "7"));
    
    TL_TEST(repl_test(&e, "(fn! gfn (a b) (+ a b))", "gfn"));
    TL_TEST(repl_test(&e, "(global! lfn (lambda (a b) (+ a b)))", "lfn"));
    TL_TEST(repl_test(&e, "(lfn 2 3)", "5"));
    TL_TEST(repl_test(&e, "(gfn 2 3)", "5"));

    TL_TEST(repl_test(&e, "(macro! global-fn! (name binds &body)"
                      "['global! 'name 'binds ])",
                      "setq!"));

    TL_TEST(repl_test(&e, "(global-fn! gfn2 (a b) (+ a b))", "gfn2"));
    TL_TEST(repl_test(&e, "(gfn2 2 3)", "5"));
    std::cout << e.gc_info() << std::endl;
}

void
test_quasiquote(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

    TL_TEST(repl_test(&e, "`(+ 1 2)", "(+ 1 2)"));

    TL_TEST(repl_test(&e, "'`(+ 1 ,(+ 2 3))", "(quasiquote (+ 1 (unquote (+ 2 3))))"));

    TL_TEST(repl_test(&e, "`(+ 1 ,(+ 2 3))", "(+ 1 5)"));

    TL_TEST(repl_test(&e, "(macro-expand '`(+ 1 ,(+ 2 3)))", "(+ 1 5)"));
    TL_TEST(repl_test(&e, "[(quasiquote (+ 1 (unquote (+ 2 3))))]", "(+ 1 5)"));

    TL_TEST(repl_test(&e, "(global! L [3 4 5])", "(3 4 5)"));
    TL_TEST(repl_test(&e, "`(1 2 ,@L)", "(1 2 3 4 5)"));

    std::cout << e.gc_info() << std::endl;
}

void
test_scope(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);
    TL_TEST(repl_test(&e,
                      "(macro! scope (&body)"
                      "  ['lambda '() ['progn body]])",
                      "scope"));
    
    TL_TEST(repl_test(&e, "(eval (scope (+ 2 2)))", "4"));
    TL_TEST(repl_test(&e, "(macro-expand '(scope (+ 2 2)))", "(lambda NIL (progn ((+ 2 2))))"));
    TL_TEST(repl_test(&e, "(eval (macro-expand '(scope (+ 2 2))))", "4"));

    TL_TEST(repl_test(&e, "((lambda (b) (progn b)) '(+ 2 2))", "4"));

    //TL_TEST(repl_test(&e,
    //                  "(macro! set2! (a b val)"
    //                  "['progn"
    //                  "  ['set! 'a 'val]"
    //                  "  ['set! 'b 'val]])",
    //                  "set2!"));
    //
    //
    //TL_TEST(repl_test(&e, "(global! x 8)", "x"));
    //TL_TEST(repl_test(&e, "(global! y 22)", "y"));
    //TL_TEST(repl_test(&e, "(set2! 'x 'y 3)", "3"));
    //TL_TEST(repl_test(&e, "x", "3"));
    //TL_TEST(repl_test(&e, "y", "3"));

    //TL_TEST(repl_test(&e,
    //                  "(macro! when (test &body)"
    //                  "['if test '(progn body)))",
    //                  "when"));
 

    std::cout << e.gc_info() << std::endl;
}

void
test_extended_math(void)
{
    yal::Environment e;
    e.load_core();
    e.load_file(std_path);

    TL_TEST(repl_test(&e, "(cos 1)", "0.540302"));
    TL_TEST(repl_test(&e, "(sin 1)", "0.841471"));
    TL_TEST(repl_test(&e, "(tan 1)", "1.55741"));

    TL_TEST(repl_test(&e, "(cos 0.3)", "0.955337"));
    TL_TEST(repl_test(&e, "(sin 0.3)", "0.29552"));
    TL_TEST(repl_test(&e, "(tan 0.3)", "0.309336"));

    TL_TEST(repl_test(&e, "(sqrt 5)", "2.23607"));

    TL_TEST(repl_test(&e, "(floor 7)", "7"));
    TL_TEST(repl_test(&e, "(ceil 7)", "7"));

    TL_TEST(repl_test(&e, "(floor 7.2)", "7"));
    TL_TEST(repl_test(&e, "(ceil 7.2)", "8"));

    TL_TEST(repl_test(&e, "(floor 7.8)", "7"));
    TL_TEST(repl_test(&e, "(ceil 7.8)", "8"));
    
    TL_TEST(repl_test(&e, "(sum-of-squares 2 0.3 11 4)", "141.09"));

    TL_TEST(repl_test(&e, "(min 2 4.3)", "2"));
    TL_TEST(repl_test(&e, "(max 2 4.3)", "4.3"));
    TL_TEST(repl_test(&e, "(min 7.2 1)", "1"));
    TL_TEST(repl_test(&e, "(max 7.2 1)", "7.2"));

    TL_TEST(repl_test(&e, "(clamp 4 -2 2)", "2"));
    TL_TEST(repl_test(&e, "(clamp 1 -2 2)", "1"));
    TL_TEST(repl_test(&e, "(clamp -6 -2 2)", "-2"));
}

int
main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

    TL(test_sizes());
    //TL(test_types_creation());
    //TL(test_read());
    //TL(test_nilp());
    //TL(test_len());
    //TL(test_globals());
    //TL(test_ipreverse());
    //TL(test_simple_eval());
    //TL(test_lex_types());
    //TL(test_buildin_range());
    //TL(test_buildin_equality());
    //TL(test_buildin_accessors());
    //TL(test_buildin_list_creation());
    //TL(test_buildin_math());
    //TL(test_variables());
    //TL(test_lambda());
    //TL(test_fn());
    //TL(test_try_catch_throw());
    //TL(test_full_circle());
    //TL(test_load_libraries());
    //TL(test_functions_and_recursion());
    //TL(test_predicates());
    //TL(test_std());
    //TL(test_conditionals());
    //TL(test_setnth());
    //TL(test_variable_definition());
    //TL(test_set_variables());
    //TL(test_reducers());
    //TL(test_std_list_stuff());
    //TL(test_extended_math());
    //TL(test_get());
    //TL(test_macros());

    //TL(test_quasiquote());
    //TL(test_funcall());

    /*NOT WORKING*/
    //TL(test_scope_management());
    //TL(test_tco());

    tl_summary();
}
