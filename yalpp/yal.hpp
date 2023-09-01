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

/*TODO:
  Make environment a const refrence
*/

namespace yal {

#define ASSERT_UNREACHABLE(str) assert(str && "Unmanaged execution path observed!")  
#define UNUSED(v) ((void)v)

const auto constvar_id = "CONSTANT";

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
typedef Expr*(*BuildinFn)(VariableScope*, Expr*);

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
        BuildinFn buildin;
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

std::runtime_error UserError(const char *_fn, const char *_msg, Expr *_specification);
std::runtime_error ImplementationError(const char *_msg);
std::runtime_error NotImplemented(const char *_fn);

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
Expr* nthcdr(int _n, Expr* _e);

Expr* assoc(Expr* _key, Expr* _list);
Expr* ipreverse(Expr* _list);

char* to_cstr(const std::string& _s);
Expr* nil(void);
/*TODO: Make this one private somehow..*/
std::stringstream _stream_value(Expr* _e);
std::stringstream stream(Expr* _e);
std::string stringify(Expr* _e);


namespace core {

Expr* progn(VariableScope* _scope, Expr* _e);
Expr* apply(VariableScope* _scope, Expr* _e);

Expr* quote(VariableScope* _scope, Expr* _e);
Expr* list(VariableScope* _scope, Expr* _e);
Expr* len(VariableScope* _scope, Expr* _e);
Expr* put(VariableScope* _scope, Expr* _e);
Expr* reverse_ip(VariableScope* _scope, Expr* _e);
Expr* range(VariableScope* _scope, Expr* _e);

Expr* car(VariableScope* _scope, Expr* _e);
Expr* cdr(VariableScope* _scope, Expr* _e);
Expr* cons(VariableScope* _scope, Expr* _e);
Expr* nthcdr(VariableScope* _scope, Expr* _e);

Expr* plus(VariableScope* _scope, Expr* _e);
Expr* minus(VariableScope* _scope, Expr* _e);
Expr* multiply(VariableScope* _scope, Expr* _e);
Expr* divide(VariableScope* _scope, Expr* _e);
Expr* lessthan(VariableScope* _scope, Expr* _e);
Expr* greaterthan(VariableScope* _scope, Expr* _e);
Expr* mathequal(VariableScope* _scope, Expr* _e);
Expr* eq(VariableScope* _scope, Expr* _e);
Expr* equal(VariableScope* _scope, Expr* _e);

Expr* defconst(VariableScope* _scope, Expr* _e);
Expr* defglobal(VariableScope* _scope, Expr* _e);
Expr* deflocal(VariableScope* _scope, Expr* _e);
Expr* deflambda(VariableScope* _scope, Expr* _e);
Expr* deffunction(VariableScope* _scope, Expr* _e);
Expr* defmacro(VariableScope* _scope, Expr* _e);
Expr* setvar(VariableScope* _scope, Expr* _e);

Expr* _try(VariableScope* _scope, Expr* _e);
Expr* _throw(VariableScope* _scope, Expr* _e);

Expr* _if(VariableScope* _scope, Expr* _e);
Expr* _cond(VariableScope* _scope, Expr* _e);
Expr* _or(VariableScope* _scope, Expr* _e);
Expr* _and(VariableScope* _scope, Expr* _e);
};

class GarbageCollector {
    std::vector<Expr*> m_in_use = {};
public:
    Expr* new_variable(void);
    void destroy_variable(Expr* _e);
    ~GarbageCollector(void);
};

//class VariableScope {
//public:
//    VariableScope(VariableScope* _outer);
//    VariableScope* m_outer;
//    Expr* m_variables = nullptr;
//};

class VariableScope {
public:
    VariableScope(Environment* _env, VariableScope* _outer);
    Environment* env(void);
    VariableScope* global_scope(void);
    VariableScope create_internal(void);
    bool is_var_const(Expr* _var);
    Expr* variable_get(const std::string& _name);
    int variables_len(void);
	bool add_constant(const char* _name, Expr* _v);
    bool add_global(const char* _name, Expr* _v);
	bool add_buildin(const char* _name, const BuildinFn _fn);
    bool add_variable(const char* _name, Expr* _v);
    void bind(Expr* _binds, Expr* _values);

private:
    VariableScope* m_outer = nullptr;
    Environment* m_env;
    Expr* m_variables = nullptr;
};

class Environment {
public:
    Environment(void);
    VariableScope* global_scope(void);

    Expr* read(const char* _program);
    Expr* eval(Expr* _e);
    Expr* list_eval(VariableScope* _scope, Expr* _list);
    Expr* scoped_eval(VariableScope* _scope, Expr* _e);

	Expr* blank(void);
	Expr* cons(Expr *_car, Expr *_cdr);
    Expr* real(int _v);
    Expr* decimal(float _v);
    Expr* symbol(const std::string& _v);
    Expr* string(const std::string& _v);
    Expr* buildin(const BuildinFn _v);
    Expr* list(const std::initializer_list<Expr *>& _lst);
    Expr* put_in_list(Expr* _val, Expr* _list);

    bool load_core(void);
	bool add_constant(const char* _name, Expr* _v);
    bool add_global(const char* _name, Expr* _v);
	bool add_buildin(const char* _name, const BuildinFn _fn);
    bool add_variable(const char* _name, Expr* _v);

private:
    GarbageCollector m_gc;
    VariableScope m_global_scope;
    Expr* lex_value(std::string& _token);
    Expr* lex(std::list<std::string>& _tokens);
};

class NormalReturn {
    Expr* m_r;
public:
    NormalReturn(Expr* _r) : m_r(_r) {};
    Expr* expr(void);
};

#define YALCPP_IMPLEMENTATION
#ifdef YALCPP_IMPLEMENTATION

std::runtime_error
UserError(const char *_fn, const char *_msg, Expr *_specification = nullptr) {
    std::stringstream ss;
    ss << "[" << _fn << "] " << _msg;
    if (_specification != nullptr)
        ss << stringify(_specification);
    return std::runtime_error(ss.str());
}

std::runtime_error
ImplementationError(const char *_msg)
{
    return std::runtime_error(_msg);
}

std::runtime_error
NotImplemented(const char *_fn)
{
    return UserError(_fn, "Not Implemented!");
}


Expr*
NormalReturn::expr(void)
{
    return m_r;
}

Expr*
GarbageCollector::new_variable()
{
    //Expr* out = (Expr*)malloc(sizeof(Expr));
    Expr* out = new Expr;
    if (out == nullptr)
      throw UserError("INTERNAL",
                      "Expr limit reached, could not create more Expr's.");
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

VariableScope::VariableScope(Environment* _env, VariableScope* _outer) : m_outer(_outer), m_env(_env) {};

VariableScope*
VariableScope::global_scope(void)
{
    VariableScope* g = this;
    while (g->m_outer != nullptr)
        g = g->m_outer;
    return g;
}

VariableScope
VariableScope::create_internal(void)
{
    return VariableScope(env(), this);
}

Environment*
VariableScope::env(void)
{
    return m_env;
}

int
VariableScope::variables_len(void)
{
    return len(m_variables);
}

Expr*
VariableScope::variable_get(const std::string& _s)
{
    Expr* v = nullptr;
    VariableScope* scope = this;
    while (scope != nullptr) {
        std::cout << "looking for " << _s << " in scope " << scope << std::endl;
        v = assoc(env()->symbol(_s), m_variables);
        if (!is_nil(v))
            return v;
        scope = scope->m_outer;
    }
    std::cout << "could not find " << _s << std::endl;
    return nullptr;
}

bool
VariableScope::add_variable(const char* _name, Expr* _v)
{
    Expr* entry = env()->list({env()->symbol(_name), _v});
    if (!is_nil(variable_get(_name)))
        throw UserError("INTERNAL add_variable", "could not add variable, because it already exists");
    m_variables = env()->put_in_list(entry, m_variables);
    return true;
}

bool
VariableScope::add_global(const char* _name, Expr* _v)
{
    return global_scope()->add_variable(_name, _v);
}

bool
VariableScope::add_constant(const char* _name, Expr* _v)
{
    Expr* entry;
    if (!is_nil(variable_get(_name)))
        throw UserError("INTERNAL add_constant", "could not add variable, because it already exists");
    entry = env()->list({env()->symbol(_name), _v, env()->symbol(constvar_id)});
    global_scope()->m_variables = env()->put_in_list(entry, global_scope()->m_variables);
    return true;
}

bool
VariableScope::add_buildin(const char* _name, const BuildinFn _fn)
{
    return add_constant(_name, env()->buildin(_fn));
}

void
VariableScope::bind(Expr* _binds, Expr* _values)
{
  /*TODO: We just blindly pair binds and values,
          In the future, add support for &rest and keyword binds aswell*/
    std::string bindstr;
    Expr* bind = _binds; 
    Expr* val = _values; 
    std::cout << "binding " << stringify(_binds) << " to " << stringify(_values) << std::endl;
    while (!is_nil(bind) && !is_nil(val)) {
        if (bind->type != TYPE_SYMBOL)
            throw UserError("internal bind", "expected binds to be symbols");
        //bindstr = bind->symbol;
        //if (bindstr.size() > 1 && bindstr.find_first_of('&') == 0) {
        //    bind = env()->symbol(bindstr.c_str()+1);
        //    add_variable(car(bind)->symbol, car(val));
        //    
        //}
        add_variable(car(bind)->symbol, car(val));
        bind = cdr(bind);
        val = cdr(val);
    }
}

bool
VariableScope::is_var_const(Expr* _var)
{
    if (!is_nil(third(_var)) &&
        third(_var)->type == TYPE_SYMBOL &&
        third(_var)->symbol == std::string(constvar_id))
        return true;
    return false;
}


VariableScope*
Environment::global_scope(void)
{
    return &m_global_scope;
}

bool
Environment::add_variable(const char* _name, Expr* _v)
{
    return m_global_scope.add_variable(_name, _v);
}

bool
Environment::add_global(const char* _name, Expr* _v)
{
    return m_global_scope.add_global(_name, _v);
}

bool
Environment::add_constant(const char* _name, Expr* _v)
{
    return m_global_scope.add_constant(_name, _v);
}

bool
Environment::add_buildin(const char* _name, const BuildinFn _fn)
{
    return m_global_scope.add_global(_name, buildin(_fn));
}

Environment::Environment(void) : m_global_scope(this, nullptr) {}

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
        "(", ")", /*Lists*/
        "[", "]", /*array*/
        "'", "`", ",", ",@", /*quoting & quasi-quoting*/
        //".", /*dotted-lists*/
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
        _tokens.pop_front();
        if (_tokens.front() == ")")
            return nil();
        while (token != ")") {
            if (_tokens.empty())
                return ipreverse(program);
            curr = lex(_tokens);
            program = put_in_list(curr, program);
            token = _tokens.front();
        }
        _tokens.pop_front();
        return ipreverse(program);
    }
    if (token == "[") {
        _tokens.pop_front();
        if (_tokens.front() == "]")
            return nil();
        program = put_in_list(symbol("list"), program);
        while (token != "]" ) {
            if (_tokens.empty())
                return ipreverse(program);
            curr = lex(_tokens);
            program = put_in_list(curr, program);
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

Expr*
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
        return list({symbol("error"), string(e.what())});
    }
    catch (NormalReturn& e) {
        return e.expr();
    }
    return res;
}

Expr*
Environment::scoped_eval(VariableScope* _scope, Expr* _e)
{
    if (is_nil(_e)) return nil();

    if (_e->type == TYPE_REAL    ||
        _e->type == TYPE_DECIMAL ||
        _e->type == TYPE_STRING  )
        return _e;

    if (_e->type ==  TYPE_CONS) {
        Expr* fn = nullptr;
        Expr* args = nullptr;
        std::cout << "got cons " << std::endl;
        fn = car(_e);
        args = cdr(_e);

        /*Pre-evaluation if function is represented as a list*/
        if (fn->type == TYPE_CONS)
            fn = scoped_eval(_scope, fn);

        std::cout << "got cons with fn   " << stringify(fn) << std::endl;
        std::cout << "got cons with args " << stringify(args) << std::endl;

        /*Evaluate as lambda*/
        if (fn->type == TYPE_LAMBDA) {
            VariableScope internal = _scope->create_internal();
            std::cout << "evaluating lambda " << stringify(fn->callable.binds)
                      << " " << stringify(fn->callable.body) << std::endl;
            args = list_eval(_scope, args);
            internal.bind(fn->callable.binds, args);
            return core::progn(&internal, fn->callable.body);
        }

        /*Evaluate as buildin*/
        Expr* callable = nullptr;
        std::cout << "finding callable " << stringify(fn) << std::endl;
        callable = _scope->variable_get(fn->symbol);
        std::cout << "got fn " << stringify(callable) << std::endl;
        callable = second(callable);

        if (!is_nil(callable)) {
            if (callable->type == TYPE_BUILDIN)
                return callable->buildin(_scope, args);
        }
        throw UserError("eval", "could not find function called ", fn);
    }

    if (_e->type == TYPE_SYMBOL) {
        Expr* var = nullptr;
        std::cout << "got symbol " << std::endl;
        if (is_nil(_e))
            return symbol("NIL");
        if (_e->symbol == std::string("t") || _e->symbol == std::string("T"))
            return symbol("T");

        var = _scope->variable_get(_e->symbol);
        if (!is_nil(var))
            return second(var);
        throw UserError("eval", "could not find existing symbol called ", _e);
    }
    throw UserError("eval", "Got something that could not be evaluated", _e);
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
        root = put_in_list(scoped_eval(_scope, car(curr)), root);
        curr = cdr(curr);
    }
    return ipreverse(root);
}

bool
Environment::load_core(void)
{
    const float pi = 3.141592; 

    add_constant("*Creator*", string("Thomas Alexgaard"));
    add_constant("*Creator-github*", string("https://github.com/thom9258/"));
    add_constant("*Host-Languange*", string("C++"));
    add_constant("*Version*", list({real(0), real(0), real(1)}));
    add_constant("PI", decimal(pi));
    add_constant("PI/2", decimal(pi/2));
    add_constant("PI2", decimal(pi*2));
    add_constant("E", decimal(2.71828));
    add_constant("E-constant", decimal(0.57721));

    add_buildin("const!", core::defconst);
    add_buildin("global!", core::defglobal);
    add_buildin("local!", core::deflocal);
    add_buildin("set!", core::setvar);

    add_buildin("fn!", core::deffunction);
    add_buildin("macro!", core::defmacro);
    add_buildin("lambda", core::deflambda);

    add_buildin("quote", core::quote);
    add_buildin("list", core::list);
    add_buildin("cons", core::cons);
    add_buildin("range", core::range);
    add_buildin("apply", core::apply);
    add_buildin("reverse!", core::reverse_ip);
    add_buildin("progn", core::progn);

    add_buildin("car", core::car);
    add_buildin("cdr", core::cdr);
    add_buildin("nthcdr", core::nthcdr);

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
Environment::blank(void)
{
	Expr *out = m_gc.new_variable();
	if (out == nullptr)
		return nil();
	return out;
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
Environment::symbol(const std::string& _v) {
    /*Test size of inp and store smallstr if possible*/
     Expr *out = m_gc.new_variable();
     if (out == nullptr)
       return nil();
     out->type = TYPE_SYMBOL;
     out->symbol = to_cstr(_v);
     return out;
}

Expr *
Environment::string(const std::string& _v) {
    /*Test size of inp and store smallstr if possible*/
  Expr *out = m_gc.new_variable();
  if (out == nullptr)
    return nil();
  out->type = TYPE_STRING;
  out->string = to_cstr(_v);
  return out;
}

Expr *
Environment::buildin(BuildinFn _v) {
  Expr *out = m_gc.new_variable();
  if (out == nullptr)
    return nil();
  out->type = TYPE_BUILDIN;
  out->buildin = _v;
  return out;
}

Expr *Environment::list(const std::initializer_list<Expr*>& _lst) {
    Expr* lst = nil();
    /*Iterating in reverse order*/
    for (auto it = std::rbegin(_lst); it != std::rend(_lst); ++it)
        lst = put_in_list(*it, lst);
    return lst;
}

Expr*
Environment::put_in_list(Expr* _val, Expr* _list)
{
    return cons(_val, _list);
}

char*
to_cstr(const std::string& _s) 
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
nthcdr(int _n, Expr* _e)
{
    int i;
    Expr* tmp = _e;
    if (_n < 0)
        return nil();
    for (i = 0; i < _n; i++)
        tmp = cdr(tmp);
    return tmp;
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
core::quote(VariableScope* _s, Expr* _e)
{
    UNUSED(_s);
    if (len(_e) != 1)
      throw UserError("quote", "expected 1 argument");
    return first(_e);
}

Expr*
core::list(VariableScope* _s, Expr* _e)
{
    return _s->env()->list_eval(_s, _e);
}

Expr*
core::cons(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(_e) != 2)
      throw UserError("cons", "expected 2 arguments");
    return _s->env()->cons(first(args), second(args));
}

Expr*
core::car(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(_e) != 1)
      throw UserError("car", "expected 1 argument");
    return car(first(args));
}

Expr*
core::cdr(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(_e) != 1)
      throw UserError("cdr", "expected 1 argument");
    return cdr(first(args));
}

Expr*
core::reverse_ip(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1)
        return nil();
    return ipreverse(first(args));
}

Expr*
core::nthcdr(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(_e) != 2)
      throw UserError("nthcdr", "expected 2 arguments");
    if (len(args) != 2)
        return nil();
    if (first(args)->type != TYPE_REAL || second(args)->type != TYPE_CONS)
        return nil();
    return nthcdr(first(args)->real, second(args));
}

Expr*
core::len(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    return _s->env()->real(len(args));
}

Expr*
core::plus(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* res = _s->env()->real(0);
    Expr* curr = args;

    auto PLUS2 = [_s] (Expr* _a, Expr* _b) {
        if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
            return _s->env()->decimal(_a->decimal + _b->decimal);
        else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
            return _s->env()->decimal(_a->real + _b->decimal);
        else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
            return  _s->env()->decimal(_a->decimal + _b->real);
        return _s->env()->real(_a->real + _b->real);
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
core::minus(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* res = _s->env()->real(0);
    Expr* curr = args;

    auto MINUS2 = [_s] (Expr* _a, Expr* _b) {
        if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
            return _s->env()->decimal(_a->decimal - _b->decimal);
        else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
            return _s->env()->decimal(_a->real - _b->decimal);
        else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
            return  _s->env()->decimal(_a->decimal - _b->real);
        return _s->env()->real(_a->real - _b->real);
    };

    if (len(args) == 0) {
        return _s->env()->real(0);
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
core::multiply(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* res = _s->env()->real(1);
    Expr* curr = args;

    auto MULT2 = [_s] (Expr* _a, Expr* _b) {
        if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
            return _s->env()->decimal(_a->decimal * _b->decimal);
        else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
            return _s->env()->decimal(_a->real * _b->decimal);
        else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
            return  _s->env()->decimal(_a->decimal * _b->real);
        return _s->env()->real(_a->real * _b->real);
    };

    if (len(args) == 0)
        return _s->env()->real(1);
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
core::divide(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* res = nullptr;
    Expr* curr = args;

    auto DIV2 = [_s] (Expr* _a, Expr* _b) {
        //std::cout << "a = " << stringify(_a) << " , "
        //          << "b = " << stringify(_b) << std::endl;
        if ((_a->type == TYPE_DECIMAL && _a->decimal == 0) ||
            (_a->type == TYPE_REAL && _a->real == 0)        )
            return _s->env()->decimal(0);

        if ((_b->type == TYPE_DECIMAL && _b->decimal == 0) ||
            (_b->type == TYPE_REAL && _b->real == 0)        )
          throw UserError("/", "divide by zero error");

        if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
            return _s->env()->decimal(_a->decimal / _b->decimal);
        else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
            return _s->env()->decimal(_a->real / _b->decimal);
        else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
            return  _s->env()->decimal(_a->decimal / _b->real);
        return _s->env()->decimal(_a->real / _b->real);
    };

    if (len(args) == 0)
        return _s->env()->real(1);
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
core::mathequal(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
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
        return _s->env()->symbol("T");
    while (!is_nil(curr)) {
        if (!EQ2(car(prev), car(curr)))
            return nil();
        prev = curr;
        curr = cdr(curr);
    }
    return _s->env()->symbol("T");
}

Expr*
core::lessthan(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* curr = cdr(args);
    Expr* prev = args;

    auto LT2 = [_s] (Expr* _a, Expr* _b) {
        if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
            return _a->decimal < _b->decimal;
        else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
            return _a->real < _b->decimal;
        else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
            return  _a->decimal < _b->real;
        return _a->real < _b->real;
    };

    if (len(args) < 2)
        return _s->env()->symbol("T");
    while (!is_nil(curr)) {
        if (!is_val(car(prev)) || !is_val(car(curr)))
          throw UserError("<", "can only compare values");
        if (!LT2(car(prev), car(curr)))
            return nil();
        prev = curr;
        curr = cdr(curr);
    }
    return _s->env()->symbol("T");
}

Expr*
core::greaterthan(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* curr = cdr(args);
    Expr* prev = args;

    auto GT2 = [_s] (Expr* _a, Expr* _b) {
        if (_a->type == TYPE_DECIMAL && _b->type == TYPE_DECIMAL)
            return _a->decimal > _b->decimal;
        else if (_a->type == TYPE_REAL && _b->type == TYPE_DECIMAL)
            return _a->real > _b->decimal;
        else if (_a->type == TYPE_DECIMAL && _b->type == TYPE_REAL)
            return  _a->decimal > _b->real;
        return _a->real > _b->real;
    };

    if (len(args) < 2)
        return _s->env()->symbol("T");
    while (!is_nil(curr)) {
        if (!is_val(car(prev)) || !is_val(car(curr)))
          throw UserError(">", "can only compare values");
        if (!GT2(car(prev), car(curr)))
            return nil();
        prev = curr;
        curr = cdr(curr);
    }
    return _s->env()->symbol("T");
}

Expr*
core::equal(VariableScope* _s, Expr* _e)
{
    UNUSED(_s);
    Expr* args = _e;
    Expr* curr = cdr(args);
    Expr* prev = args;

    auto EQ2 = [_s] (Expr* _a, Expr* _b) {
        return stringify(_a) == stringify(_b);
    };

    if (len(args) < 2) {
        return _s->env()->symbol("T");
    }
    while (!is_nil(curr)) {
        if (!EQ2(car(prev), car(curr)))
            return nil();
        prev = curr;
        curr = cdr(curr);
    }
    return _s->env()->symbol("T");
}

Expr*
core::eq(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* curr = cdr(args);
    Expr* prev = args;

    auto EQ2 = [_s] (Expr* _a, Expr* _b) {
        return stringify(_a) == stringify(_b);
    };

    if (len(args) < 2) {
        return _s->env()->symbol("T");
    }
    while (!is_nil(curr)) {
        if (!EQ2(car(prev), car(curr)))
            return nil();
        prev = curr;
        curr = cdr(curr);
    }
    return _s->env()->symbol("T");
}

Expr*
core::apply(VariableScope* _s, Expr* _e)
{
    /*TODO: fix apply*/
    Expr* call;
    std::cout << "apply inp " << stringify(_e) << std::endl;
    Expr* args = _s->env()->list_eval(_s, _e);
    std::cout << "apply args " << stringify(args) << std::endl;
    if (len(args) != 2)
      throw UserError("apply", "Expected 2 arguments");
    call = _s->env()->cons(first(args), second(args));
    return _s->env()->scoped_eval(_s, call);
}

Expr*
core::progn(VariableScope* _s, Expr* _e)
{
    Expr* last = nil();
    while (!is_nil(_e)) {
        last = _s->env()->scoped_eval(_s, car(_e));
        _e = cdr(_e);
    }
    return last;
}

Expr*
core::range(VariableScope* _s, Expr* _e)
{
    bool reverse = false;
    int low;
    int high;
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* out = nullptr;

    if (first(args)->real < second(args)->real) {
        low = first(args)->real;
        high = second(args)->real;
    } else {
        low = second(args)->real;
        high = first(args)->real;
        reverse = true;
    }
    for (int i = low; i <= high; i++) {
        out = _s->env()->put_in_list(_s->env()->real(i), out);
    }
    if (reverse)
        return out;
    return ipreverse(out);
}

Expr*
core::_if(VariableScope* _s, Expr* _e)
{
    Expr* cond = nullptr;    
    //std::cout << "if input " << stringify(_e) << std::endl; 
    if (is_nil(_e))
      throw UserError("if", "expected required condition and true body");
    if (!(len(_e) == 2 || len(_e) == 3))
      throw UserError("if", "expected required condition and true body");
    cond = _s->env()->scoped_eval(_s, first(_e));
    std::cout << "condition res " << stringify(cond) << std::endl; 
    if (!is_nil(cond))
        return _s->env()->scoped_eval(_s, second(_e));
    return _s->env()->scoped_eval(_s, third(_e));
}

Expr*
core::_try(VariableScope* _s, Expr* _e)
{
    Expr* res = nullptr;
    Expr* err = nullptr;
    try {
        //std::cout << "trying " << stringify(first(_e)) << std::endl; 
        res = _s->env()->scoped_eval(_s, first(_e));
    }
    //catch (Error& e) {
    catch (std::exception& e) {
        err = _s->env()->list({_s->env()->symbol("error"), _s->env()->string(e.what())});
        //std::cout << "caught err " << stringify(err) << std::endl; 
    }
    if (is_nil(err))
        return res;
    /*Check all requirements for catching is in place*/
    if (len(_e) < 2)
        return err;
    if (is_nil(second(_e)) || second(_e)->type != TYPE_SYMBOL)
      throw UserError("try", "arg 2 expected symbol for error");
    if (is_nil(third(_e)) || !is_cons(_e))
      throw UserError("try", "arg 3 expected expression for error evaluation");

    VariableScope internal = _s->create_internal();
    internal.add_variable(second(_e)->symbol, err);
    res = _s->env()->scoped_eval(&internal, third(_e));
    return res;
}


Expr*
core::_throw(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (first(args)->type != TYPE_STRING)
      throw UserError("throw", "expects string input");
    throw UserError("throw", first(args)->string);
    return nil();
}

Expr*
core::defconst(VariableScope* _s, Expr* _e)
{
    Expr* val = _s->env()->scoped_eval(_s, second(_e));
    if (len(_e) != 2)
      throw UserError("const!", "expects symbol and value.");
    if (first(_e)->type != TYPE_SYMBOL)
      throw UserError("const!", "expects name to be type symbol");
    _s->add_constant(first(_e)->symbol, val);
    return first(_e);
}

Expr*
core::defglobal(VariableScope* _s, Expr* _e)
{
    Expr* val = _s->env()->scoped_eval(_s, second(_e));
    if (len(_e) != 2)
      throw UserError("global!", "expects symbol and value.");
    if (first(_e)->type != TYPE_SYMBOL)
      throw UserError("global!", "expects name to be type symbol");
    _s->add_global(first(_e)->symbol, val);
    return first(_e);
}

Expr*
core::deflocal(VariableScope* _s, Expr* _e)
{
    Expr* val = _s->env()->scoped_eval(_s, second(_e));
    if (len(_e) != 2)
      throw UserError("local!", "expects symbol and value.");
    if (first(_e)->type != TYPE_SYMBOL)
      throw UserError("local!", "expects name to be type symbol");
    _s->add_variable(first(_e)->symbol, val);
    return first(_e);
}

Expr*
core::setvar(VariableScope* _s, Expr* _e)
{
    std::cout << "setting " << stringify(first(_e)) << " to "
              << stringify(second(_e)) << std::endl;
    Expr* var = nullptr;
    if (first(_e)->type != TYPE_SYMBOL)
        throw UserError("set!", "Expected symbol name to set");

    var = _s->variable_get(first(_e)->symbol);
    if (is_nil(var))
        throw UserError("setvar!", "variable to set does not exist");

    if (_s->is_var_const(var))
        throw UserError("set!", "Cannot set constant symbol");
    cdr(var)->cons.car = _s->env()->scoped_eval(_s, second(_e));
    return second(var);
}

Expr*
core::deflambda(VariableScope* _s, Expr* _e)
{
    UNUSED(_s);
    Expr* l = _s->env()->blank();
    if (len(_e) != 2)
        throw UserError("lambda", "expected arguments to be binds and body");
    l->type = TYPE_LAMBDA;
    l->callable.binds = car(_e);
    l->callable.body = cdr(_e);
    return l;
}

Expr*
core::deffunction(VariableScope* _s, Expr* _e)
{
    UNUSED(_s);
    UNUSED(_e);
    throw NotImplemented("fn!");
    return nil();
}

Expr*
core::defmacro(VariableScope* _s, Expr* _e)
{
    UNUSED(_s);
    UNUSED(_e);
    throw NotImplemented("macro!");
    return nil();
}


#endif /*YALPP_IMPLEMENTATION*/
#endif /*YALCPP_H*/

}; /*namespace yal*/
