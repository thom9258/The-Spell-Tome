#pragma once

#include <iostream>
#include <initializer_list>
#include <algorithm>
#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <sstream>
#include <functional>
#include <cassert>
#include <stdexcept>

#ifndef YALCPP_H
#define YALCPP_H


namespace yal {

#define ASSERT_UNREACHABLE(str) assert(str && "Unmanaged execution path observed!")  
#define UNUSED(v) (void)v

enum TYPE {
    TYPE_INVALID = 0,
    TYPE_CONS,
    TYPE_REAL,
    TYPE_DECIMAL,
    TYPE_SYMBOL,
    TYPE_STRING,
    TYPE_SMALLSYMBOL,
    TYPE_SMALLSTRING,

    TYPE_BUILDIN,
    TYPE_FUNCTION,
    TYPE_LAMBDA,
    TYPE_MACRO,

	TYPE_COUNT
};

/*Forward Declarations*/
struct Expr;
class VariableScope;
class Environment;
typedef Expr*(*buildin_fn)(Environment*, VariableScope*, Expr*);


const size_t smallXsz = (sizeof(Expr*) * 2) / sizeof(char);

struct Expr {
    union {
        int real;
        float decimal;
        char* string;
        char* symbol;
        /*small* allows us to store symbols and strings without
          creating a heap allocation*/
        char smallstring[smallXsz]; 
        char smallsymbol[smallXsz];
        buildin_fn buildin;
        struct {
            Expr* binds;
            Expr* body;
        }callable;

        struct {
            Expr* car;
            Expr* cdr;
        }cons;
    };
    char type;
};


bool is_nil(Expr* _e);
bool is_val(Expr* _e);
bool is_cons(Expr* _e);
bool is_dotted(Expr* _e);

int len(Expr* _e);
Expr* car(Expr* _e);
Expr* cdr(Expr* _e);
Expr* first(Expr* _e); 
Expr* second(Expr* _e);
Expr* third(Expr* _e);
Expr* fourth(Expr* _e);
Expr* nth(int _n, Expr* _e);

Expr* put(Environment* _env, Expr* _val, Expr* _list);
Expr* assoc(Expr* _key, Expr* _list);
Expr* ipreverse(Expr* _list);

char* to_cstr(std::string _s);
Expr* nil(void);
/*TODO: Make this one private somehow..*/
std::stringstream _stream_value(Expr* _e);
std::stringstream stream(Expr* _e);
std::string stringify(Expr* _e);


namespace core {

Expr* progn(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* apply(Environment* _env, VariableScope* _scope, Expr* _e);

Expr* quote(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* list(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* len(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* put(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* reverse_ip(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* range(Environment* _env, VariableScope* _scope, Expr* _e);

Expr* car(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* cdr(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* cons(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* nth(Environment* _env, VariableScope* _scope, Expr* _e);

Expr* plus(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* minus(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* multiply(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* divide(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* lessthan(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* greaterthan(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* mathequal(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* eq(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* equal(Environment* _env, VariableScope* _scope, Expr* _e);

Expr* defconst(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* defglobal(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* defvar(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* deflambda(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* deffunction(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* defmacro(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* setfield(Environment* _env, VariableScope* _scope, Expr* _e);

Expr* _try(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* _throw(Environment* _env, VariableScope* _scope, Expr* _e);

Expr* _if(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* _cond(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* _or(Environment* _env, VariableScope* _scope, Expr* _e);
Expr* _and(Environment* _env, VariableScope* _scope, Expr* _e);
};



class GarbageCollector {
    std::vector<Expr*> m_in_use = {};
public:
    Expr *new_variable(void);
    void destroy_variable(Expr* _e);
    ~GarbageCollector(void);
};

class VariableScope {
public:
    VariableScope* m_outer = nullptr;
    Expr* m_variables = nullptr;
};

class Environment {
public:
    Expr* globals(void);
    Expr* constants(void);

    Expr* read(const char* _program);
    Expr* eval(Expr* _e);
    Expr* scoped_eval(VariableScope* _scope, Expr* _e);
    Expr* list_eval(VariableScope* _scope, Expr* _list);

    bool add_global(const char* _name, Expr* _v);
	bool add_constant(const char* _name, Expr* _v);
	bool add_buildin(const char* _name, buildin_fn _fn);

    bool load_core(void);

	Expr *cons(Expr *_car, Expr *_cdr);
    Expr *real(int _v);
    Expr *decimal(float _v);
    Expr *symbol(std::string _v);
    Expr *symbol(const char* _v);
    Expr *string(std::string _v);
    Expr *string(const char* _v);
    Expr *buildin(buildin_fn _v);
    Expr *list(std::initializer_list<Expr *> _lst);

private:
    GarbageCollector m_gc;
    VariableScope m_global_scope;
    Expr* m_constants = nullptr; /*constant values and buildins*/
    bool add_variable(VariableScope* _scope, const char* _name, Expr* _v);
    Expr* lex_value(std::string& _token);
    Expr* lex(std::list<std::string>& _tokens);
};

#define YALCPP_IMPLEMENTATION
#ifdef YALCPP_IMPLEMENTATION

Expr *
GarbageCollector::new_variable()
{
    //Expr* out = (Expr*)malloc(sizeof(Expr));
    Expr* out = new Expr;
    m_in_use.push_back(out);
    return out;
}

void
GarbageCollector::destroy_variable(Expr* _e)
{
    if (_e == nullptr)
        return;
    switch(_e->type) {
    case TYPE_STRING:
        delete[] _e->string;
        break;
    case TYPE_SYMBOL:
        delete[] _e->symbol;
        break;
    default:
        break;
    };
    delete _e;
}

GarbageCollector::~GarbageCollector(void)
{
    for (auto v : m_in_use) 
        destroy_variable(v);
}


Expr*
Environment::globals(void)
{
    return m_global_scope.m_variables; 
}

Expr*
Environment::constants(void)
{
    return m_constants; 
}

        /*TODO:  Do error checking like matching+even parens*/
        /*TODO:  Remove garbage tokens*/
/*TODO:  if you want a tokenizer impl that is isolated, take this
        and add a tokenization recipe struct with:
        - garbate token list
        - literal token list
        - extra stuff like use newline tokens
        - etc..
*/

std::string
_try_get_special_token(const char* _start)    
{
    const std::string stokens[] = {
        //"'(",
        "(", ")", /*Lists*/
        //"'[",
        "[", "]", /*array*/
        "'", "`", ",", ",@", /*quoting*/
        };
    const size_t stokens_len = sizeof(stokens) / sizeof(stokens[0]);
    /*TODO: not a cpp'y solution..*/
    auto begins_with = [_start] (std::string stok) {
        for (size_t i = 0; i < stok.size(); i++) {
            if (*(_start+i) != stok[i])
                return false;
        }
        return true;

    };
    for (size_t i = 0; i < stokens_len; i++) {
        if (begins_with(stokens[i]))
            return stokens[i];
    }
    return "";
}


std::string
_get_next_token(const char* _source, int& _cursor)
{
    std::string token;
    int len = 0;
    auto is_whitespace = [] (char c) {
        return (c == ' ' || c == '\t' || c == '\n');
    };

    /*Handle special tokens*/
    token = _try_get_special_token(&_source[_cursor]);
    if (token != "") {
        _cursor += token.size();
        return token;
    }

    /*Handle strings*/
    if (_source[_cursor + len] == '\"') {
        len++;
        while (_source[_cursor + len] != '\"' && _source[_cursor + len] != '\0') 
            len++;
        len++;
        token = std::string(&_source[_cursor], len);
        _cursor += len;
        return token;
    }

    /*Handle normal tokens*/
    while (!is_whitespace(_source[_cursor + len]) &&
           _source[_cursor + len] != '\0') {
        len++;
        token = _try_get_special_token(&_source[_cursor + len]);
        if (token != "")
            break;
    }
    token = std::string(&_source[_cursor], len);
    _cursor += len;
    return token;
}

std::list<std::string>
tokenize(const char* _str) {
    int cursor = 0;
    std::list<std::string> tokens = {};
    std::string curr;

    auto is_whitespace = [] (char c) {
        return (c == ' ' || c == '\t' || c == '\n');
    };
    auto trim_whitespace = [is_whitespace, &cursor, _str] () {
        while (is_whitespace(_str[cursor]) && _str[cursor] != '\0')
            cursor++;
    };
    while (_str[cursor] != '\0') {
        trim_whitespace();
        curr = _get_next_token(_str, cursor);
        tokens.push_back(curr);
    }
    return tokens;
};

Expr*
Environment::lex_value(std::string& _token)
{

    /*TODO: We should do cool stuff like allowing _ in numbers and remove them before
            lexing so that 100_000_000 is a valid number. */
    auto looks_like_number = [] (std::string& _token) {
        if (_token.size() > 1 && (_token[0] == '-' || _token[0] == '+'))
            return (_token[1] >= '0' && _token[1] <= '9');
        return (_token[0] >= '0' && _token[0] <= '9');
    };
    auto looks_like_string = [] (std::string& _token) {
        if (_token.size() < 2)
            return false;
        return (_token[0] == '\"');
    };
    auto token_value_type = [] (std::string& _token) {
        if (_token.find(".") == std::string::npos)
            return TYPE_REAL;
        return TYPE_DECIMAL;
    };

    if (looks_like_string(_token)) {
        return string(_token.substr(1, _token.size()-2));
    }
    if (looks_like_number(_token)) {
        if (token_value_type(_token) == TYPE_REAL) {
            return real(std::stoi(_token));
        }
        else {
            return decimal(std::stof(_token));
        }
    }
    return symbol(_token);
}

Expr*
Environment::lex(std::list<std::string>& _tokens)
{
    /*TODO: Does not support special syntax for:
    dotted lists (a . 34)
    */
    std::string token;
    Expr* program = nullptr;
    Expr* curr = nullptr;

    if (_tokens.empty())
        return nil();

    token = _tokens.front();
    //std::cout << "lexing token: " << token << std::endl;

    if (token == "(") {
        //std::cout << "found ("  << std::endl;
        _tokens.pop_front();
        while (token != ")") {
            if (_tokens.empty())
                return ipreverse(program);
            curr = lex(_tokens);
            program = put(this, curr, program);
            token = _tokens.front();
        }
        _tokens.pop_front();
        return ipreverse(program);
    }
    if (token == "[") {
        //std::cout << "found ["  << std::endl;
        _tokens.pop_front();
        program = put(this, symbol("list"), program);
        while (token != "]" ) {
            if (_tokens.empty())
                return ipreverse(program);
            curr = lex(_tokens);
            program = put(this, curr, program);
            token = _tokens.front();
        }
        _tokens.pop_front();
        return ipreverse(program);
    }
    if (token == "'") {
        //std::cout << "found '"  << std::endl;
        _tokens.pop_front();
        program = list({symbol("quote"), lex(_tokens)});
        return program;
    }
    //std::cout << "found value"  << std::endl;
    program = lex_value(token);
    _tokens.pop_front();
    return program;
}

Expr *
Environment::read(const char* _program)
{
    std::list<std::string> tokens;
    Expr* lexed = nullptr;
    tokens = tokenize(_program);
    if (tokens.empty())
        return nil();
    lexed = lex(tokens);
    std::cout << "lexed:   " << stringify(lexed) << std::endl;
    return lexed;
}

Expr*
Environment::eval(Expr* _e)
{
    Expr* res = nullptr;
    try {
        res = scoped_eval(&m_global_scope, _e);
    }
    catch (std::exception& e) {
        //std::cout << "ERROR CAUGHT: " << e.what() << std::endl;
        return list({symbol("error"), string(e.what())});
    }
    return res;
}

Expr*
Environment::scoped_eval(VariableScope* _scope, Expr* _e)
{
    Expr* var = nullptr;
    Expr* fn = nullptr;
    Expr* callable = nullptr;
    Expr* args = nullptr;
    VariableScope* sc = _scope;
    if (is_nil(_e)) return nil();

    switch (_e->type) {
    case TYPE_REAL:
        /*Fallthrough*/
    case TYPE_DECIMAL:
        /*Fallthrough*/
    case TYPE_STRING:
        return _e;

    case TYPE_CONS:
        fn = car(_e);
        args = cdr(_e);
        callable = cdr(assoc(fn, m_constants));
        if (!is_nil(callable)) {
            if (callable->type == TYPE_BUILDIN)
                return callable->buildin(this, _scope, args);
        }
        return nil();

    case TYPE_SYMBOL:
        if (is_nil(_e))
            return symbol("NIL");
        if (_e->symbol == std::string("t") || _e->symbol == std::string("T"))
            return symbol("T");

        var = assoc(_e, m_constants);
        if (!is_nil(var))
            return cdr(var);
        while (sc != nullptr) {
            var = assoc(_e, _scope->m_variables);
            if (!is_nil(var))
                return cdr(var);
            sc = sc->m_outer;
        }
        return nil();

    default:
        ASSERT_UNREACHABLE("unknown type");
    };
    ASSERT_UNREACHABLE("unreachable");
    return nil();

}

Expr*
Environment::list_eval(VariableScope* _scope, Expr* _list)
{
    Expr* root = nullptr;
    Expr* curr = _list;
    if (!is_cons(_list)) {
        return nil();
    }
    while (!is_nil(curr)) {
        root = put(this, scoped_eval(_scope, car(curr)), root);
        curr = cdr(curr);
    }
    return ipreverse(root);
}

bool
Environment::add_variable(VariableScope* _scope, const char* _name, Expr* _v)
{
    Expr* entry = cons(symbol(_name), _v);
    _scope->m_variables = put(this, entry, _scope->m_variables);
    return true;
}

bool
Environment::add_global(const char* _name, Expr* _v)
{
    return add_variable(&m_global_scope, _name, _v);
}

bool
Environment::add_constant(const char* _name, Expr* _v)
{
    Expr* entry = cons(symbol(_name), _v);
    m_constants = put(this, entry, m_constants);
    return true;
}

bool
Environment::add_buildin(const char* _name, buildin_fn _fn)
{
    Expr* entry = cons(symbol(_name), buildin(_fn));
    m_constants = put(this, entry, m_constants);
    return true;
}

bool
Environment::load_core(void)
{
    const float pi = 3.141592; 

    add_constant("*Creator*", string("Thomas Alexgaard"));
    add_constant("*Creator-github*", string("https://github.com/thom9258/"));
    add_constant("*Host-Languange*", string("C++"));
    add_constant("*Version*", list({real(0), real(1)}));
    add_constant("PI", decimal(pi));
    add_constant("PI/2", decimal(pi/2));
    add_constant("PI2", decimal(pi*2));

    add_buildin("quote", core::quote);
    add_buildin("list", core::list);
    add_buildin("cons", core::cons);
    add_buildin("range", core::range);
    add_buildin("apply", core::apply);
    add_buildin("reverse!", core::reverse_ip);
    add_buildin("progn", core::progn);

    add_buildin("car", core::car);
    add_buildin("cdr", core::cdr);
    add_buildin("nth", core::nth);

    add_buildin("+", core::plus);
    add_buildin("-", core::minus);
    add_buildin("*", core::multiply);
    add_buildin("/", core::divide);
    add_buildin("=", core::mathequal);
    add_buildin("<", core::lessthan);
    add_buildin(">", core::greaterthan);
    add_buildin("eq", core::eq);
    add_buildin("equal", core::equal);

    add_buildin("if", core::_if);
    add_buildin("try", core::_try);
    add_buildin("throw", core::_throw);
    return true;
}

Expr *
Environment::cons(Expr *_car, Expr *_cdr) {
	Expr *out = m_gc.new_variable();
	if (out == nullptr)
		return nil();
	out->type = TYPE_CONS;
	out->cons.car = _car;
	out->cons.cdr = _cdr;
	return out;
}

Expr *
Environment::real(int _v) {
     Expr *out = m_gc.new_variable();
     if (out == nullptr)
       return nil();
     out->type = TYPE_REAL;
     out->real = _v;
     return out;
   }

Expr*
Environment::decimal(float _v) {
     Expr *out = m_gc.new_variable();
     if (out == nullptr)
       return nil();
     out->type = TYPE_DECIMAL;
     out->decimal = _v;
     return out;
   }

Expr*
Environment::symbol(std::string _v) {
    /*Test size of inp and store smallstr if possible*/
     Expr *out = m_gc.new_variable();
     if (out == nullptr)
       return nil();
     out->type = TYPE_SYMBOL;
     out->symbol = to_cstr(_v);
     return out;
}

Expr*
Environment::symbol(const char* _v) {
    return symbol(std::string(_v));
}

Expr *
Environment::string(std::string _v) {
    /*Test size of inp and store smallstr if possible*/
  Expr *out = m_gc.new_variable();
  if (out == nullptr)
    return nil();
  out->type = TYPE_STRING;
  out->string = to_cstr(_v);
  return out;
}

Expr *
Environment::string(const char* _v) {
    return string(std::string(_v));
}

Expr *Environment::buildin(buildin_fn _v) {
  Expr *out = m_gc.new_variable();
  if (out == nullptr)
    return nil();
  out->type = TYPE_BUILDIN;
  out->buildin = _v;
  return out;
}

Expr *Environment::list(std::initializer_list<Expr*> _lst) {
    Expr* lst = nil();
    /*Iterating in reverse order*/
    for (auto it = std::rbegin(_lst); it != std::rend(_lst); ++it)
        lst = put(this, *it, lst);
    return lst;
}

char*
to_cstr(std::string _s) 
{
    char* out = new char[_s.size()+1];
    out[_s.size()] = '\0';
    std::memcpy(out, _s.c_str(), _s.size());
    return out;
}

Expr*
nil(void)
{
    return nullptr;
}

bool
is_nil(Expr* _e)
{
    const std::string nil1 = "nil";
    const std::string nil2 = "NIL";
    if (_e == nullptr || _e->type == TYPE_INVALID)
        return true;
    if (_e->type == TYPE_CONS && is_nil(_e->cons.car) && is_nil(_e->cons.cdr))
        return true;
    if (_e->type == TYPE_SYMBOL && nil1 == _e->symbol)
        return true;
    if (_e->type == TYPE_SYMBOL && nil2 == _e->symbol)
        return true;
    return false;
}

bool
is_val(Expr* _e)
{
    if (_e == nullptr)
        return false;
    if (_e->type == TYPE_REAL || _e->type == TYPE_DECIMAL)
        return true;
    return false;
}

bool
is_cons(Expr* _e)
{
    if (!is_nil(_e) && _e->type == TYPE_CONS)
        return true;
    return false;
}

bool
is_dotted(Expr* _e)
{
    if (!is_nil(_e) && _e->type == TYPE_CONS  &&
        !is_nil(car(_e)) && !is_cons(car(_e)) &&
        !is_nil(cdr(_e)) && !is_cons(cdr(_e)))
        return true;
    return false;
}

int
len(Expr* _e)
{
    if (is_nil(_e)) return 0;
    if (!is_cons(_e)) return 1;
    int cnt = 0;
    Expr* tmp = _e;
    while (!is_nil(tmp)) {
        cnt++;
        tmp = cdr(tmp);
    }
    return cnt;
}

Expr*
put(Environment* _env, Expr* _val, Expr* _list)
{
    return _env->cons(_val, _list);
}

Expr*
assoc(Expr* _key, Expr* _list)
{
    Expr* tmp;
    std::string key;
    if (is_nil(_key) || is_nil(_list)) return nil();
    if (!is_cons(_list)) return nil();

    key = stringify(_key);
    tmp = _list;
    while (!is_nil(tmp)) {
        if (key == stringify(car(car(tmp))))
            return car(tmp);
        tmp = cdr(tmp);
    }
    return nil();
}

Expr*
ipreverse(Expr* _list)
{
    Expr* curr = _list;
    Expr* next = nullptr;
    Expr* prev = nullptr;
    while (!is_nil(curr)) {
        next = cdr(curr);
        curr->cons.cdr = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}

Expr*
car(Expr* _e)
{
    if (is_nil(_e) || !is_cons(_e))
        return nil();
    return _e->cons.car;
}

Expr*
cdr(Expr* _e)
{
    if (is_nil(_e) || !is_cons(_e))
        return nil();
    return _e->cons.cdr;
}

Expr* first(Expr* _e)
{ return car(_e); }
Expr* second(Expr* _e)
{ return car(cdr(_e)); }
Expr* third(Expr* _e)
{ return car(cdr(cdr(_e))); }
Expr* fourth(Expr* _e)
{ return car(cdr(cdr(cdr(_e)))); }

Expr*
nth(int _n, Expr* _e)
{
    int i;
    Expr* tmp = _e;
    if (_n < 0)
        return nil();
    for (i = 0; i < _n; i++)
        tmp = cdr(tmp);
    return car(tmp);
}

std::stringstream
_stream_value(Expr* _e)
{

    std::stringstream ss;
    if (is_nil(_e)) {
        ss << "NIL";
        return ss;
    }

    switch (_e->type) {
    case TYPE_CONS:
        ss << "(" << stream(_e).str() << ")";
        break;
    case TYPE_LAMBDA:
        ss << "#<lambda>";
        break;
    case TYPE_MACRO:
        ss << "#<macro>";
        break;
    case TYPE_FUNCTION:
        ss << "#<function>";
        break;
    case TYPE_BUILDIN:
        ss << "#<buildin>";
        break;
    case TYPE_REAL:
        ss << _e->real;
        break;
    case TYPE_DECIMAL:
        ss << _e->decimal;
        break;
    case TYPE_SMALLSYMBOL:
        /*TODO*/
    case TYPE_SYMBOL:
        ss << _e->symbol;
        break;
    case TYPE_SMALLSTRING:
        /*TODO*/
    case TYPE_STRING:
        ss << "\"" << _e->string << "\"";
        break;
    default:
        ASSERT_UNREACHABLE("stringify_value() Got invalid atom type!");
    };
    //std::cout << "returning " << ss.str() << std::endl;
    return ss;
}

std::stringstream
stream(Expr* _e)
{
    std::stringstream ss;
    Expr* curr = _e;
    if (is_nil(_e)) {
        ss << "NIL";
        return ss;
    }
    if (!is_cons(_e))
        return _stream_value(_e);

    if (is_dotted(_e)) {
        ss << "(" << stream(car(_e)).str() << " . "
           << stream(cdr(_e)).str() << ")";
        return ss;
    }
    ss << "(";
    ss << stream(car(curr)).str();
    curr = cdr(curr);
    while (!is_nil(curr)) {
        ss << " " << stream(car(curr)).str();
        curr = cdr(curr);
    }
    ss << ")";
    return ss;
}

std::string
stringify(Expr* _e)
{
    std::stringstream ss = stream(_e);
    if (ss.rdbuf()->in_avail() == 0)
        return "";
    return ss.str();
}


Expr*
core::quote(Environment* _env, VariableScope* _scope, Expr* _e)
{
    UNUSED(_env);
    UNUSED(_scope);
    if (len(_e) > 1)
        throw std::runtime_error("quote can only have 1 input argument.");
    //std::cout << "quote " << stringify(first(_e)) << std::endl;
    return first(_e);
}

Expr*
core::list(Environment* _env, VariableScope* _scope, Expr* _e)
{
    return _env->list_eval(_scope, _e);
}

Expr*
core::cons(Environment* _env, VariableScope* _scope, Expr* _e)
{
    Expr* args = _env->list_eval(_scope, _e);
    std::cout << "cons input: " << stringify(_e) << std::endl;
    std::cout << "cons eval: " << stringify(args) << std::endl;
    //std::cout << "cons result: " << stringify(_env->cons(first(args), second(args))) << std::endl;
    return _env->cons(first(args), second(args));
}

Expr*
core::car(Environment* _env, VariableScope* _scope, Expr* _e)
{
    Expr* args = _env->list_eval(_scope, _e);
    if (len(args) < 1)
        return nil();
    return car(first(args));
}

Expr*
core::cdr(Environment* _env, VariableScope* _scope, Expr* _e)
{
    Expr* args = _env->list_eval(_scope, _e);
    return cdr(first(args));
}

Expr*
core::reverse_ip(Environment* _env, VariableScope* _scope, Expr* _e)
{
    Expr* args = _env->list_eval(_scope, _e);
    if (len(args) != 1)
        return nil();
    return ipreverse(first(args));
}

Expr*
core::nth(Environment* _env, VariableScope* _scope, Expr* _e)
{
    Expr* args = _env->list_eval(_scope, _e);
    if (len(args) != 2)
        return nil();
    if (first(args)->type != TYPE_REAL || second(args)->type != TYPE_CONS)
        return nil();
    return nth(first(args)->real, second(args));
}

Expr*
core::len(Environment* _env, VariableScope* _scope, Expr* _e)
{
    Expr* args = _env->list_eval(_scope, _e);
    return _env->real(len(args));
}

Expr*
core::plus(Environment* _env, VariableScope* _scope, Expr* _e)
{
    Expr* args = _env->list_eval(_scope, _e);
    Expr* res = _env->real(0);
    Expr* curr = args;

    auto PLUS2 = [_env] (Expr* _a, Expr* _b) {
        if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
            return _env->decimal(_a->decimal + _b->decimal);
        else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
            return _env->decimal(_a->real + _b->decimal);
        else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
            return  _env->decimal(_a->decimal + _b->real);
        return _env->real(_a->real + _b->real);
    };

    while (!is_nil(curr)) {
        if (!is_val(car(curr)))
            break;
        res = PLUS2(res, car(curr));
        curr = cdr(curr);
    }
    return res;
}

Expr*
core::minus(Environment* _env, VariableScope* _scope, Expr* _e)
{
    Expr* args = _env->list_eval(_scope, _e);
    Expr* res = _env->real(0);
    Expr* curr = args;

    auto MINUS2 = [_env] (Expr* _a, Expr* _b) {
        if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
            return _env->decimal(_a->decimal - _b->decimal);
        else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
            return _env->decimal(_a->real - _b->decimal);
        else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
            return  _env->decimal(_a->decimal - _b->real);
        return _env->real(_a->real - _b->real);
    };

    if (len(args) == 0) {
        return _env->real(0);
    }

    res = car(curr);
    curr = cdr(curr);
    while (!is_nil(curr)) {
        if (!is_val(car(curr)))
            break;
        res = MINUS2(res, car(curr));
        curr = cdr(curr);
    }
    return res;
}

Expr*
core::multiply(Environment* _env, VariableScope* _scope, Expr* _e)
{
    Expr* args = _env->list_eval(_scope, _e);
    Expr* res = _env->real(1);
    Expr* curr = args;

    auto MULT2 = [_env] (Expr* _a, Expr* _b) {
        if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
            return _env->decimal(_a->decimal * _b->decimal);
        else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
            return _env->decimal(_a->real * _b->decimal);
        else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
            return  _env->decimal(_a->decimal * _b->real);
        return _env->real(_a->real * _b->real);
    };

    if (len(args) == 0)
        return _env->real(1);
    if (len(args) == 1)
        return first(args);
    while (!is_nil(curr)) {
        if (!is_val(car(curr)))
            break;
        res = MULT2(res, car(curr));
        curr = cdr(curr);
    }
    return res;
}

Expr*
core::divide(Environment* _env, VariableScope* _scope, Expr* _e)
{
    Expr* args = _env->list_eval(_scope, _e);
    Expr* res = nullptr;
    Expr* curr = args;

    auto DIV2 = [_env] (Expr* _a, Expr* _b) {
        std::cout << "a = " << stringify(_a) << " , "
                  << "b = " << stringify(_b) << std::endl;
        if ((_a->type == TYPE_DECIMAL && _a->decimal == 0) ||
            (_a->type == TYPE_REAL && _a->real == 0)        )
            return _env->decimal(0);

        if ((_b->type == TYPE_DECIMAL && _b->decimal == 0) ||
            (_b->type == TYPE_REAL && _b->real == 0)        )
            throw std::runtime_error("[/] divide by zero error");

        if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
            return _env->decimal(_a->decimal / _b->decimal);
        else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
            return _env->decimal(_a->real / _b->decimal);
        else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
            return  _env->decimal(_a->decimal / _b->real);
        return _env->decimal(_a->real / _b->real);
    };

    if (len(args) == 0)
        return _env->real(1);
    if (len(args) == 1)
        return first(args);

    res = car(curr);
    curr = cdr(curr);
    while (!is_nil(curr)) {
        if (!is_val(car(curr)))
            break;
        res = DIV2(res, car(curr));
        curr = cdr(curr);
    }
    return res;
}

Expr*
core::mathequal(Environment* _env, VariableScope* _scope, Expr* _e)
{
    Expr* args = _env->list_eval(_scope, _e);
    Expr* curr = cdr(args);
    Expr* prev = args;

    auto EQ2 = [] (Expr* a, Expr* b) {
        if (a->type == TYPE_DECIMAL && b->type == TYPE_DECIMAL)
            return a->decimal == b->decimal;
        else if (a->type == TYPE_REAL && b->type == TYPE_DECIMAL)
            return a->real == b->decimal;
        else if (a->type == TYPE_DECIMAL && b->type == TYPE_REAL)
            return  a->decimal == b->real;
        else if (a->type == TYPE_REAL && b->type == TYPE_REAL)
            return a->real == b->real;
        else if (a->type == TYPE_SYMBOL && b->type == TYPE_SYMBOL)
            return std::string(a->symbol) ==b->symbol;
        else if (a->type == TYPE_STRING && b->type == TYPE_STRING)
            return std::string(a->string) == b->string;
        else if (a->type == TYPE_CONS && b->type == TYPE_CONS)
            return stringify(a) == stringify(b);
        return false;
    };
    if (len(args) < 2)
        return _env->symbol("T");
    while (!is_nil(curr)) {
        if (!is_val(prev) || !is_val(curr))
            return nil();
        if (!EQ2(car(prev), car(curr)))
            return nil();
        prev = curr;
        curr = cdr(curr);
    }
    return _env->symbol("T");
}

Expr*
core::lessthan(Environment* _env, VariableScope* _scope, Expr* _e)
{
    Expr* args = _env->list_eval(_scope, _e);
    Expr* curr = cdr(args);
    Expr* prev = args;

    auto LT2 = [_env] (Expr* _a, Expr* _b) {
        if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
            return _a->decimal < _b->decimal;
        else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
            return _a->real < _b->decimal;
        else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
            return  _a->decimal < _b->real;
        return _a->real < _b->real;
    };

    if (len(args) < 2)
        return _env->symbol("T");
    while (!is_nil(curr)) {
        if (!is_val(prev) || !is_val(curr))
            return nil();
        if (!LT2(car(prev), car(curr)))
            return nil();
        prev = curr;
        curr = cdr(curr);
    }
    return _env->symbol("T");
}

Expr*
core::greaterthan(Environment* _env, VariableScope* _scope, Expr* _e)
{
    Expr* args = _env->list_eval(_scope, _e);
    Expr* curr = cdr(args);
    Expr* prev = args;

    auto GT2 = [_env] (Expr* _a, Expr* _b) {
        if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
            return _a->decimal > _b->decimal;
        else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
            return _a->real > _b->decimal;
        else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
            return  _a->decimal > _b->real;
        return _a->real > _b->real;
    };

    if (len(args) < 2)
        return _env->symbol("T");
    while (!is_nil(curr)) {
        if (!is_val(prev) || !is_val(curr))
            return nil();
        if (!GT2(car(prev), car(curr)))
            return nil();
        prev = curr;
        curr = cdr(curr);
    }
    return _env->symbol("T");
}

Expr*
core::equal(Environment* _env, VariableScope* _scope, Expr* _e)
{
    UNUSED(_scope);
    Expr* args = _e;
    Expr* curr = cdr(args);
    Expr* prev = args;

    auto EQ2 = [_env] (Expr* _a, Expr* _b) {
        return stringify(_a) == stringify(_b);
    };

    if (len(args) < 2) {
        return _env->symbol("T");
    }
    while (!is_nil(curr)) {
        if (!EQ2(car(prev), car(curr)))
            return nil();
        prev = curr;
        curr = cdr(curr);
    }
    return _env->symbol("T");
}

Expr*
core::eq(Environment* _env, VariableScope* _scope, Expr* _e)
{
    Expr* args = _env->list_eval(_scope, _e);
    Expr* curr = cdr(args);
    Expr* prev = args;

    auto EQ2 = [_env] (Expr* _a, Expr* _b) {
        return stringify(_a) == stringify(_b);
    };

    if (len(args) < 2) {
        return _env->symbol("T");
    }
    while (!is_nil(curr)) {
        if (!EQ2(car(prev), car(curr)))
            return nil();
        prev = curr;
        curr = cdr(curr);
    }
    return _env->symbol("T");
}

Expr*
core::apply(Environment* _env, VariableScope* _scope, Expr* _e)
{
    /*TODO: fix apply*/
    Expr* call;
    std::cout << "apply inp " << stringify(_e) << std::endl;
    Expr* args = _env->list_eval(_scope, _e);
    std::cout << "apply args " << stringify(args) << std::endl;
    if (len(args) != 2)
        throw std::runtime_error("[apply] Expected 2 arguments");
    call = _env->cons(first(args), second(args));
    return _env->scoped_eval(_scope, call);
}

Expr*
core::progn(Environment* _env, VariableScope* _scope, Expr* _e)
{
    Expr* last = nil();
    while (!is_nil(_e)) {
        last = _env->scoped_eval(_scope, car(_e));
        _e = cdr(_e);
    }
    return last;
}

Expr*
core::range(Environment* _env, VariableScope* _scope, Expr* _e)
{
    bool reverse = false;
    int low;
    int high;
    Expr* args = _env->list_eval(_scope, _e);
    Expr* out = nullptr;

    if (first(args)->real < second(args)->real) {
        low = first(args)->real;
        high = second(args)->real;
    } else {
        low = second(args)->real;
        high = first(args)->real;
        reverse = true;
    }
    for (int i = low; i <= high; i++)
        out = put(_env, _env->real(i), out);
    if (reverse)
        return out;
    return ipreverse(out);
}

Expr*
core::_if(Environment* _env, VariableScope* _scope, Expr* _e)
{
    Expr* cond = nullptr;    
    //std::cout << "if input " << stringify(_e) << std::endl; 
    if (is_nil(_e))
        throw std::runtime_error("[if] expected required condition and true body");
    if (!(len(_e) == 2 || len(_e) == 3))
        throw std::runtime_error("[if] expected required condition and true body");
    cond = _env->scoped_eval(_scope, first(_e));
    std::cout << "condition res " << stringify(cond) << std::endl; 
    if (!is_nil(cond))
        return _env->scoped_eval(_scope, second(_e));
    return _env->scoped_eval(_scope, third(_e));
}

Expr*
core::_try(Environment* _env, VariableScope* _scope, Expr* _e)
{
    UNUSED(_env);
    UNUSED(_scope);
    UNUSED(_e);
    throw std::runtime_error("[try] not implemented!");
#if 0
    Expr* res;
    try {
        res = scoped_eval(&m_global_scope, _e);
    }
    catch (std::exception& e) {
        return list({symbol("error"), string(e.what())});
    }
#endif
}


Expr*
core::_throw(Environment* _env, VariableScope* _scope, Expr* _e)
{
    Expr* args = _env->list_eval(_scope, _e);
    if (first(args)->type != TYPE_STRING)
        throw std::runtime_error("[throw] expects string input");
    throw std::runtime_error(first(args)->string);
    return nil();
}

/* Reverse list without append
(define (reverse l)
    (define (aux orig result)
        (if (null? orig) result
                (aux (cdr orig) (cons (car orig) result))))
    (aux l '()))
*/

#endif /*YALPP_IMPLEMENTATION*/
#endif /*YALCPP_H*/

}; /*namespace yal*/
