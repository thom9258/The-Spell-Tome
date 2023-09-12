#include <iostream>
#include <fstream>
#include <sstream>
#include <initializer_list>
#include <algorithm>
#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <functional>
#include <cassert>
//#include <stdexcept>
#include <math.h>

#include "yalstd.hpp"

#ifndef YALCPP_H
#define YALCPP_H

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

enum THROWABLE_TYPES {
    THROWABLE_ERROR,
    THROWABLE_RETURNED,
    THROWABLE_THROWN,
};

class Throwable {
public:
    char m_type = THROWABLE_ERROR;
    Expr* m_returned_expr = nullptr;
    std::string m_error_msg = "";
    Expr* m_error_expr = nullptr;
    Expr* m_thrown_expr = nullptr;

    bool is_error(void);
    const std::string& error_msg(void);
    Expr* error_expr(void);

    bool is_returned(void);
    Expr* returned(void);

    bool is_thrown(void);
    Expr* thrown(void);
};

Throwable NotImplemented(const char *_fn);
Throwable InternalError(const char *_msg);
Throwable ProgramError(const char *_fn, const char *_msg, Expr *_specification);
Throwable UserThrow(Expr* _thrown);
Throwable UserReturn(Expr* _returned);

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

Expr* assoc(Expr* _key, Expr* _list);
Expr* ipreverse(Expr* _list);

char* to_cstr(const std::string& _s);
std::stringstream stream(Expr* _e);
std::string stringify(Expr* _e);


namespace core {

Expr* quote(VariableScope* _s, Expr* _e);
Expr* list(VariableScope* _s, Expr* _e);
Expr* len(VariableScope* _s, Expr* _e);
Expr* reverse_ip(VariableScope* _s, Expr* _e);
Expr* range(VariableScope* _s, Expr* _e);

Expr* car(VariableScope* _s, Expr* _e);
Expr* cdr(VariableScope* _s, Expr* _e);
Expr* cons(VariableScope* _s, Expr* _e);

Expr* plus(VariableScope* _s, Expr* _e);
Expr* minus(VariableScope* _s, Expr* _e);
Expr* multiply(VariableScope* _s, Expr* _e);
Expr* divide(VariableScope* _s, Expr* _e);
Expr* lessthan(VariableScope* _s, Expr* _e);
Expr* greaterthan(VariableScope* _s, Expr* _e);
Expr* mathequal(VariableScope* _s, Expr* _e);
Expr* eq(VariableScope* _s, Expr* _e);
Expr* equal(VariableScope* _s, Expr* _e);
Expr* is_nil(VariableScope* _s, Expr* _e);
Expr* _or(VariableScope* _s, Expr* _e);
Expr* _and(VariableScope* _s, Expr* _e);

// more math! https://www.w3schools.com/cpp/cpp_math.asp
//Expr* cos(VariableScope* _s, Expr* _e);
//Expr* sin(VariableScope* _s, Expr* _e);
//Expr* sqrt(VariableScope* _s, Expr* _e);

Expr* slurp_file(VariableScope* _s, Expr* _e);
Expr* read(VariableScope* _s, Expr* _e);
Expr* eval(VariableScope* _s, Expr* _e);
Expr* write(VariableScope* _s, Expr* _e);
Expr* stringify(VariableScope* _s, Expr* _e);

Expr* defconst(VariableScope* _s, Expr* _e);
Expr* defglobal(VariableScope* _s, Expr* _e);
Expr* deflocal(VariableScope* _s, Expr* _e);
Expr* deflambda(VariableScope* _s, Expr* _e);
Expr* deffunction(VariableScope* _s, Expr* _e);
Expr* defmacro(VariableScope* _s, Expr* _e);
//Expr* setvar(VariableScope* _s, Expr* _e);
Expr* setcar(VariableScope* _s, Expr* _e);
Expr* setcdr(VariableScope* _s, Expr* _e);
Expr* variable_definition(VariableScope* _s, Expr* _e);

Expr* _try(VariableScope* _s, Expr* _e);
Expr* _throw(VariableScope* _s, Expr* _e);
Expr* _return(VariableScope* _s, Expr* _e);

Expr* _if(VariableScope* _s, Expr* _e);
Expr* _cond(VariableScope* _s, Expr* _e);
Expr* _do(VariableScope* _s, Expr* _e);
Expr* _typeof(VariableScope* _s, Expr* _e);
};

class GarbageCollector {
  /*TODO:
    Currently just a dummy gc
    Implement a mark-and-sweep algorithm as the primary gc
  */
    std::vector<Expr*> m_in_use = {};
public:
    size_t exprs_in_use(void);
    Expr* new_variable(void);
    void destroy_variable(Expr* _e);
    ~GarbageCollector(void);
};

class VariableScope {
public:
/*TODO:
  Make environment a const refrence */
    VariableScope(Environment* _env, VariableScope* _outer);
    Environment* env(void);
    VariableScope* global_scope(void);
    VariableScope create_internal(void);
    bool is_var_const(Expr* _var);
    Expr* variable_get(const std::string& _name);
    Expr* variable_get_this_scope(const std::string& _name);
    int variables_len(void);
	bool add_constant(const char* _name, Expr* _v);
    bool add_global(const char* _name, Expr* _v);
	bool add_buildin(const char* _name, const BuildinFn _fn);
    bool add_variable(const char* _name, Expr* _v);
    void bind(const char* _fnname, Expr* _binds, Expr* _values);

private:
    VariableScope* m_outer = nullptr;
    Environment* m_env;
    Expr* m_variables = nullptr;
};

class Environment {
public:
    Environment(void);
    VariableScope* global_scope(void);

    /*Info*/
    size_t exprs_in_use(void);
    const std::string gc_info(void);

    /*Evaluation*/
    Expr* read(const char* _program);
    Expr* eval(Expr* _e);
    Expr* list_eval(VariableScope* _scope, Expr* _list);
    Expr* scoped_eval(VariableScope* _scope, Expr* _e);

    /*Expr creation*/
	Expr* blank(void);
	Expr* t(void);
	Expr* nil(void);
	Expr* cons(Expr *_car, Expr *_cdr);
    Expr* real(int _v);
    Expr* decimal(float _v);
    Expr* symbol(const std::string& _v);
    Expr* string(const std::string& _v);
    Expr* buildin(const BuildinFn _v);
    Expr* list(const std::initializer_list<Expr *>& _lst);
    Expr* put_in_list(Expr* _val, Expr* _list);

    /*Library creation*/
    bool load_core(void);
    bool load_std(void);
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

#define YALCPP_IMPLEMENTATION
#ifdef YALCPP_IMPLEMENTATION

bool
Throwable::is_error(void)
{
    return m_type == THROWABLE_ERROR;
}

const std::string&
Throwable::error_msg(void)
{
  return m_error_msg;
}

Expr*
Throwable::error_expr(void)
{
    return m_error_expr; 
}

bool
Throwable::is_returned(void)
{
    return m_type == THROWABLE_RETURNED;
}

Expr*
Throwable::returned(void)
{
    return m_returned_expr;
}

bool
Throwable::is_thrown(void)
{
    return m_type == THROWABLE_THROWN;
}

Expr*
Throwable::thrown(void)
{
    return m_thrown_expr;
}

Throwable
InternalError(const char *_msg)
{
    std::stringstream ss;
    Throwable tw;
    tw.m_type = THROWABLE_ERROR;
    ss << "[INTERNAL] " << _msg;
    tw.m_error_msg = ss.str();
    return tw;
}

Throwable
NotImplemented(const char *_fn)
{
    std::stringstream ss;
    Throwable tw;
    tw.m_type = THROWABLE_ERROR;
    ss << "[" << _fn << "] is not implemented" ;
    tw.m_error_msg = ss.str();
    return tw;
}

Throwable
ProgramError(const char *_fn, const char *_msg, Expr *_specification) {
    std::stringstream ss;
    Throwable tw;
    tw.m_type = THROWABLE_ERROR;
    ss << "[" << _fn << "] " << _msg;
    tw.m_error_msg = ss.str();
    tw.m_error_expr = _specification;
    return tw;
}

Throwable
UserReturn(Expr* _returned)
{
    Throwable tw;
    tw.m_type = THROWABLE_RETURNED;
    tw.m_returned_expr = _returned;
    return tw;
}

Throwable
UserThrow(Expr* _thrown)
{
    Throwable tw;
    tw.m_type = THROWABLE_THROWN;
    tw.m_thrown_expr = _thrown;
    return tw;
}

Expr*
GarbageCollector::new_variable()
{
    //Expr* out = (Expr*)malloc(sizeof(Expr));
    Expr* out = new Expr;
    if (out == nullptr)
      throw InternalError("Expr limit reached, could not create more Expr's.");
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


size_t
GarbageCollector::exprs_in_use(void)
{
    return m_in_use.size(); 
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
        v = assoc(env()->symbol(_s), scope->m_variables);
        if (!is_nil(v))
            return v;
        scope = scope->m_outer;
    }
    return nullptr;
}

Expr*
VariableScope::variable_get_this_scope(const std::string& _s)
{
    return assoc(env()->symbol(_s), m_variables);
}

bool
VariableScope::add_variable(const char* _name, Expr* _v)
{
    Expr* entry;
    if (!is_nil(variable_get_this_scope(_name)))
        return false;
    entry = env()->list({env()->symbol(_name), _v});
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
        return false;
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
VariableScope::bind(const char* _fnname, Expr* _binds, Expr* _values)
{
  /*TODO: add support for keyword binds aswell*/
    UNUSED(_fnname);
    std::string bindstr;
    Expr* rest = nullptr;
    //while (!is_nil(_binds) && !is_nil(_values)) {
    while (!is_nil(_binds)) {
        if (car(_binds)->type != TYPE_SYMBOL)
            throw ProgramError("bind", "expected binds to be symbols, not", car(_binds));
        bindstr = std::string(car(_binds)->symbol);
        if (bindstr.size() > 1 && bindstr.find_first_of('&') == 0) {
            while (!is_nil(_values)) {
                rest = env()->put_in_list(car(_values), rest);
                _values = cdr(_values);
            }
            rest = ipreverse(rest);
            add_variable(bindstr.c_str()+1, rest);
            return;
        }
        add_variable(car(_binds)->symbol, car(_values));
        _binds = cdr(_binds);
        _values = cdr(_values);
    }
    if (len(_binds) != 0)
        throw ProgramError(_fnname, "invalid amount of inputs given! could not bind", _binds);
    if (len(_values) != 0)
        throw ProgramError(_fnname, "too many inputs given! could not bind", _values);

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

size_t
Environment::exprs_in_use(void)
{
    return m_gc.exprs_in_use(); 
}

const std::string
Environment::gc_info(void)
{
    std::stringstream ss;
    ss << "Expr size:   bytes " << sizeof(yal::Expr) << std::endl
       << "used Expr's:       " << exprs_in_use() << std::endl
       << "used memory: bytes " << exprs_in_use() * sizeof(yal::Expr) << std::endl
       << "             MB    " << float(exprs_in_use() * sizeof(yal::Expr)) / 1000000;
    return ss.str();
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
    return add_constant(_name, buildin(_fn));
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
        return nullptr;

    token = _tokens.front();
    //std::cout << "lexing token: " << token << std::endl;

    if (token == "(") {
        _tokens.pop_front();
        if (_tokens.front() == ")") {
            _tokens.pop_front();
            return nullptr;
        }
        while (token != ")") {
            if (_tokens.empty())
                return ipreverse(program);
            curr = lex(_tokens);
            program = put_in_list(curr, program);
            if (!_tokens.empty())
                token = _tokens.front();
        }
        _tokens.pop_front();
        return ipreverse(program);
    }
    if (token == "[") {
        _tokens.pop_front();
        if (_tokens.front() == "]") {
            _tokens.pop_front();
            return nullptr;
        }
        program = put_in_list(symbol("list"), program);
        while (token != "]" ) {
            if (_tokens.empty())
                return ipreverse(program);
            curr = lex(_tokens);
            program = put_in_list(curr, program);
            if (!_tokens.empty())
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
        return nullptr;
    lexed = lex(tokens);
    //std::cout << "lexed:   " << stringify(lexed) << std::endl;
    return lexed;
}

Expr*
Environment::eval(Expr* _e)
{
    Expr* res = nullptr;
    try {
        res = scoped_eval(&m_global_scope, _e);
    } catch (Throwable& e) {
        if (e.is_error())
            return list({symbol("error"), string(e.error_msg()), e.error_expr()});
        if (e.is_thrown())
            return e.thrown();
        if (e.is_returned())
            return e.returned();
    }
    return res;
}

Expr*
Environment::scoped_eval(VariableScope* _scope, Expr* _e)
{
    auto eval_fn = [this, _scope] (const char* name, Expr* fn, Expr* args) {
        auto progn = [this] (VariableScope* s, Expr* e) {
            Expr* last = nullptr;
                while (!is_nil(e)) {
                    try {
                        last = s->env()->scoped_eval(s, car(e));
                    } catch (Throwable& e) {
                        if (e.is_error())
                            return list({symbol("error"), string(e.error_msg()), e.error_expr()});
                        if (e.is_thrown())
                            return e.thrown();
                        if (e.is_returned())
                            return e.returned();
                    }
                    e = cdr(e);
            }
            return last;
        };
        VariableScope internal = _scope->create_internal();
        args = list_eval(_scope, args);
        internal.bind(name, fn->callable.binds, args);
        return progn(&internal, fn->callable.body);
    };
    
    if (is_nil(_e)) return nil();

    if (_e->type == TYPE_REAL       ||
        _e->type == TYPE_DECIMAL    ||
        _e->type == TYPE_STRING     ||
        _e->type == TYPE_SMALLSTRING)
        return _e;

    if (_e->type ==  TYPE_CONS) {
        Expr* fn = nullptr;
        Expr* args = nullptr;
        fn = car(_e);
        args = cdr(_e);

        /*Pre-evaluation if function is represented as a list*/
        if (fn->type == TYPE_CONS)
            fn = scoped_eval(_scope, fn);

        /*Evaluate as lambda*/
        if (fn->type == TYPE_LAMBDA) {
            return eval_fn("lambda", fn, args);
        }

        /*Evaluate as buildin*/
        if (fn->type == TYPE_SYMBOL) {
            Expr* callable = nullptr;
            callable = second(_scope->variable_get(fn->symbol));
            if (!is_nil(callable)) {
                if (callable->type == TYPE_BUILDIN) {
                    return callable->buildin(_scope, args);
                }
                if (callable->type == TYPE_FUNCTION) {
                    return eval_fn(fn->symbol, callable, args);
                }
                if (callable->type == TYPE_MACRO) {
                    //https://github.com/kanaka/mal/blob/master/process/guide.md#step-8-macros
                    return eval_fn(fn->symbol, callable, args);
                }

            }
        }
        throw ProgramError("eval", "could not find function called ", fn);
    }

    if (_e->type == TYPE_SYMBOL) {
        Expr* var = nullptr;
        if (is_nil(_e))
            return nil();
        if (_e->symbol == std::string("t") || _e->symbol == std::string("T"))
            return t();

        var = _scope->variable_get(_e->symbol);
        if (!is_nil(var))
            return second(var);
        throw ProgramError("eval", "could not find existing symbol called ", _e);
    }
    throw ProgramError("eval", "Got something that could not be evaluated", _e);
    return nullptr;

}

Expr*
Environment::list_eval(VariableScope* _scope, Expr* _list)
{
    Expr* root = nullptr;
    Expr* curr = _list;
    if (!is_cons(_list)) {
        return nullptr;
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
    try {
        add_buildin("const!", core::defconst);
        add_buildin("global!", core::defglobal);
        add_buildin("local!", core::deflocal);
        add_buildin("variable-definition", core::variable_definition);
        /*TODO: set! can be replaced by interpreted fn now*/
        //add_buildin("set!", core::setvar);
        add_buildin("setcar!", core::setcar);
        add_buildin("setcdr!", core::setcdr);
        add_buildin("fn!", core::deffunction);
        add_buildin("macro!", core::defmacro);
        add_buildin("lambda", core::deflambda);
        add_buildin("quote", core::quote);
        add_buildin("list", core::list);
        add_buildin("cons", core::cons);
        add_buildin("range", core::range);
        add_buildin("reverse!", core::reverse_ip);
        add_buildin("car", core::car);
        add_buildin("cdr", core::cdr);
        add_buildin("+", core::plus);
        add_buildin("-", core::minus);
        add_buildin("*", core::multiply);
        add_buildin("/", core::divide);
        add_buildin("=", core::mathequal);
        add_buildin("<", core::lessthan);
        add_buildin(">", core::greaterthan);
        add_buildin("eq", core::eq);
        add_buildin("equal", core::equal);
        add_buildin("nil?", core::is_nil);
        add_buildin("and", core::_and);
        add_buildin("or", core::_or);
        add_buildin("if", core::_if);
        add_buildin("try", core::_try);
        add_buildin("throw", core::_throw);
        add_buildin("return", core::_return);
        add_buildin("typeof", core::_typeof);
        add_buildin("slurp-file", core::slurp_file);
        add_buildin("read", core::read);
        add_buildin("eval", core::eval);
        add_buildin("write", core::write);
        add_buildin("stringify", core::stringify);
    }
    catch (std::exception& e) {
        std::cout << "something wrong with std library!" << std::endl
                  << e.what() << std::endl;
        return false;
    }
    return true;
}

bool
Environment::load_std(void)
{
    //std::cout << "std: " << stringify(eval(read(std_lib))) << std::endl;
    load_core();
    eval(read(" (fn! progn (&body)"
              "   \"evaluate body and return last result\""
              "   (if (nil? body)"
              "     NIL"
              "     (last body)))"));
    eval(read(std_lib));
    return true;
}

Expr *
Environment::t(void)
{
	return symbol("T");
}

Expr *
Environment::nil(void)
{
	return symbol("NIL");
}

Expr *
Environment::blank(void)
{
	Expr *out = m_gc.new_variable();
	if (out == nullptr)
		return nullptr;
	return out;
}

Expr *
Environment::cons(Expr *_car, Expr *_cdr) {
	Expr *out = m_gc.new_variable();
	if (out == nullptr)
		return nullptr;
	out->type = TYPE_CONS;
	out->cons.car = _car;
	out->cons.cdr = _cdr;
	return out;
}

Expr *
Environment::real(int _v) {
     Expr *out = m_gc.new_variable();
     if (out == nullptr)
       return nullptr;
     out->type = TYPE_REAL;
     out->real = _v;
     return out;
   }

Expr*
Environment::decimal(float _v) {
     Expr *out = m_gc.new_variable();
     if (out == nullptr)
       return nullptr;
     out->type = TYPE_DECIMAL;
     out->decimal = _v;
     return out;
   }

Expr*
Environment::symbol(const std::string& _v) {
    /*Test size of inp and store smallstr if possible*/
     Expr *out = m_gc.new_variable();
     if (out == nullptr)
       return nullptr;
     out->type = TYPE_SYMBOL;
     out->symbol = to_cstr(_v);
     return out;
}

Expr *
Environment::string(const std::string& _v) {
    /*Test size of inp and store smallstr if possible*/
  Expr *out = m_gc.new_variable();
  if (out == nullptr)
    return nullptr;
  out->type = TYPE_STRING;
  out->string = to_cstr(_v);
  return out;
}

Expr *
Environment::buildin(BuildinFn _v) {
  Expr *out = m_gc.new_variable();
  if (out == nullptr)
    return nullptr;
  out->type = TYPE_BUILDIN;
  out->buildin = _v;
  return out;
}

Expr *Environment::list(const std::initializer_list<Expr*>& _lst) {
    Expr* lst = nullptr;
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


bool
is_nil(Expr* _e)
{
    const std::string nil1 = "nil";
    const std::string nil2 = "NIL";
    if (_e == nullptr || _e->type == TYPE_INVALID)
        return true;
    //if (_e->type == TYPE_CONS && is_nil(_e->cons.car) && is_nil(_e->cons.cdr))
    if (_e->type == TYPE_CONS && _e->cons.car == nullptr && _e->cons.cdr == nullptr)
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
    if (is_nil(_key) || is_nil(_list)) return nullptr;
    if (!is_cons(_list)) return nullptr;

    key = stringify(_key);
    tmp = _list;
    while (!is_nil(tmp)) {
        if (key == stringify(car(car(tmp))))
            return car(tmp);
        tmp = cdr(tmp);
    }
    return nullptr;
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
        return nullptr;
    return _e->cons.car;
}

Expr*
cdr(Expr* _e)
{
    if (is_nil(_e) || !is_cons(_e))
        return nullptr;
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

std::stringstream
stream(Expr* _e)
{
    auto stream_value = [] (Expr* e) {
        std::stringstream ss;
        if (is_nil(e)) {
            ss << "NIL";
            return ss;
        }
        switch (e->type) {
        case TYPE_CONS:        ss << "(" << stream(e).str() << ")"; break;
        case TYPE_LAMBDA:      ss << "#<lambda>";                   break;
        case TYPE_MACRO:       ss << "#<macro>";                    break;
        case TYPE_FUNCTION:    ss << "#<function>";                 break;
        case TYPE_BUILDIN:     ss << "#<buildin>";                  break;
        case TYPE_REAL:        ss << e->real;                       break;
        case TYPE_DECIMAL:     ss << e->decimal;                    break;
        case TYPE_SMALLSYMBOL: ss << e->smallsymbol;                break;
        case TYPE_SYMBOL:      ss << e->symbol;                     break;
        case TYPE_SMALLSTRING: ss << e->smallstring;                break;
        case TYPE_STRING:      ss << "\"" << e->string << "\"";     break;
        default:
            ASSERT_UNREACHABLE("stringify_value() Got invalid atom type!");
        };
        return ss;
    };
    std::stringstream ss;
    Expr* curr = _e;
    if (is_nil(_e)) {
        ss << "NIL";
        return ss;
    }
    if (!is_cons(_e))
        return stream_value(_e);

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
    //if (len(_e) != 1)
    //throw ProgramError("quote", "expected 1 argument, not ", _e);
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
        throw ProgramError("cons", "expected 2 arguments, got", args);
    return _s->env()->cons(first(args), second(args));
}

Expr*
core::car(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* out;
    if (len(_e) != 1)
        throw ProgramError("car", "expected 1 argument, got", args);
    out = car(first(args));
    //std::cout << "car returns " << stringify(out) << std::endl;
    if (is_nil(out))
        return _s->env()->nil();
    return car(first(args));
}

Expr*
core::cdr(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* out;
    if (len(_e) != 1)
        throw ProgramError("cdr", "expected 1 argument, got", args);
    out = cdr(first(args));
    if (is_nil(out))
        return _s->env()->nil();
    return cdr(first(args));
}

Expr*
core::reverse_ip(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1)
        return nullptr;
    return ipreverse(first(args));
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
        if (car(curr)->type != TYPE_REAL && car(curr)->type != TYPE_DECIMAL)
            throw ProgramError("-", "cannot do math on non-value ", car(curr));
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
    if (len(args) == 1) {
        if (first(args)->type == TYPE_REAL)
            return _s->env()->real(first(args)->real * -1);
        if (first(args)->type == TYPE_DECIMAL)
            return _s->env()->real(first(args)->decimal * -1);
    }

    res = car(curr);
    curr = cdr(curr);
    while (!is_nil(curr)) {
        if (!is_val(car(curr)))
            break;
        if (car(curr)->type != TYPE_REAL && car(curr)->type != TYPE_DECIMAL)
            throw ProgramError("-", "cannot do math on non-value ", car(curr));
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
        if (car(curr)->type != TYPE_REAL && car(curr)->type != TYPE_DECIMAL)
            throw ProgramError("-", "cannot do math on non-value ", car(curr));
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
            throw ProgramError("/", "divide by zero error created by", _s->env()->cons(_a, _b));

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
        if (car(curr)->type != TYPE_REAL && car(curr)->type != TYPE_DECIMAL)
            throw ProgramError("-", "cannot do math on non-value ", car(curr));
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
        if (is_nil(a) && is_nil(b))
            return true;
        if (a->type == TYPE_DECIMAL && b->type == TYPE_DECIMAL)
            return a->decimal == b->decimal;
        if (a->type == TYPE_DECIMAL && b->type == TYPE_DECIMAL)
            return a->decimal == b->decimal;
        else if (a->type == TYPE_REAL && b->type == TYPE_DECIMAL)
            return a->real == b->decimal;
        else if (a->type == TYPE_DECIMAL && b->type == TYPE_REAL)
            return  a->decimal == b->real;
        else if (a->type == TYPE_REAL && b->type == TYPE_REAL)
            return a->real == b->real;
        else if (a->type == TYPE_SYMBOL && b->type == TYPE_SYMBOL)
            return std::string(a->symbol) == b->symbol;
        else if (a->type == TYPE_STRING && b->type == TYPE_STRING)
            return std::string(a->string) == b->string;
        else if (a->type == TYPE_CONS && b->type == TYPE_CONS)
            return stringify(a) == stringify(b);
        return false;
    };
    if (len(args) < 2)
        return _s->env()->t();
    while (!is_nil(curr)) {
        if (!EQ2(car(prev), car(curr)))
            return _s->env()->nil();
        prev = curr;
        curr = cdr(curr);
    }
    return _s->env()->t();
}

#if 0
Expr*
core::mathequal(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 2)
        throw ProgramError("=", "Expects 2 arguments, not", args);

    auto EQ2 = [] (Expr* a, Expr* b) {
        if (is_nil(a) && is_nil(b))
            return true;
        if (a->type == TYPE_DECIMAL && b->type == TYPE_DECIMAL)
            return a->decimal == b->decimal;
        if (a->type == TYPE_DECIMAL && b->type == TYPE_DECIMAL)
            return a->decimal == b->decimal;
        else if (a->type == TYPE_REAL && b->type == TYPE_DECIMAL)
            return a->real == b->decimal;
        else if (a->type == TYPE_DECIMAL && b->type == TYPE_REAL)
            return  a->decimal == b->real;
        else if (a->type == TYPE_REAL && b->type == TYPE_REAL)
            return a->real == b->real;
        else if (a->type == TYPE_SYMBOL && b->type == TYPE_SYMBOL)
            return std::string(a->symbol) == b->symbol;
        else if (a->type == TYPE_STRING && b->type == TYPE_STRING)
            return std::string(a->string) == b->string;
        else if (a->type == TYPE_CONS && b->type == TYPE_CONS)
            return stringify(a) == stringify(b);
        return false;
    };
    if (EQ2(first(args), second(args)))
        return _s->env()->t();
    return _s->env()->nil();
}
#endif

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
        return _s->env()->t();
    while (!is_nil(curr)) {
        if (!is_val(car(prev)) || !is_val(car(curr)))
            throw ProgramError("<", "can only compare values, not", car(prev));
        if (!LT2(car(prev), car(curr)))
            return nullptr;
        prev = curr;
        curr = cdr(curr);
    }
    return _s->env()->t();
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
        return _s->env()->t();
    while (!is_nil(curr)) {
        if (!is_val(car(prev)) || !is_val(car(curr)))
            throw ProgramError(">", "can only compare values, not", car(prev));
        if (!GT2(car(prev), car(curr)))
            return nullptr;
        prev = curr;
        curr = cdr(curr);
    }
    return _s->env()->t();
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
        return _s->env()->t();
    }
    while (!is_nil(curr)) {
        if (!EQ2(car(prev), car(curr)))
            return nullptr;
        prev = curr;
        curr = cdr(curr);
    }
    return _s->env()->t();
}

Expr*
core::is_nil(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (is_nil(first(args)))
        return _s->env()->t();
    return _s->env()->nil();
}

Expr*
core::_and(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) > 2)
        throw ProgramError("and", "expected 2 or less arguments, got", args);

    if (!is_nil(first(args)) && !is_nil(second(args)))
        return _s->env()->t();
    return _s->env()->nil();
}

Expr*
core::_or(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) > 2)
        throw ProgramError("or", "expected 2 or less arguments, got", args);

    if (!is_nil(first(args)))
        return _s->env()->t();
    if (!is_nil(second(args)))
        return _s->env()->t();
    return _s->env()->nil();

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
        return _s->env()->t();
    }
    while (!is_nil(curr)) {
        if (!EQ2(car(prev), car(curr)))
            return nullptr;
        prev = curr;
        curr = cdr(curr);
    }
    return _s->env()->t();
}

Expr*
core::slurp_file(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    std::string content;
    std::ifstream ifs;
    std::stringstream ss;
    if (len(args) != 1)
        throw ProgramError("slurp-file", "expects only 1 argument, got", args);
    if (first(args)->type != TYPE_STRING)
        throw ProgramError("slurp-file", "expects string as argument, got", first(args));
    ifs.open(first(args)->string);
    if (!ifs)
        throw ProgramError("slurp-file", "could not open file", first(args));

    while (std::getline(ifs, content))
        ss << content << std::endl;
    ifs.close(); 
    return _s->env()->string(ss.str());
}

Expr*
core::read(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) == 0)
        throw ProgramError("read", "expects 1 argument, but did not get anything", _s->env()->nil());
    if (len(args) != 1)
        throw ProgramError("read", "expects only 1 argument, got", args);
    if (first(args)->type != TYPE_STRING)
        throw ProgramError("read", "expects string as argument, not ", first(args));
    return _s->env()->read(first(args)->symbol);
}

Expr*
core::eval(VariableScope* _s, Expr* _e)
{
     Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1)
        throw ProgramError("eval", "expects only 1 argument, got", args);
    return _s->env()->eval(first(args));
}

Expr*
core::write(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1)
        std::cout << "NIL" << std::endl;
    /*TODO: Replace cout with a astringstream in environment, and explicitly
            cout the env stream to cout.
    */
    std::cout << stringify(first(args)) << std::endl;
    return first(args);
}

Expr*
core::stringify(VariableScope* _s, Expr* _e)
{
    std::stringstream ss;
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1)
        throw ProgramError("stringify", "expects only 1 argument, got", args);
    ss << stringify(first(args));
    return _s->env()->string(ss.str());
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
    if (!(len(_e) == 2 || len(_e) == 3))
        throw ProgramError("if", "expected required condition and true body, got", _e);
    cond = _s->env()->scoped_eval(_s, first(_e));
    if (!is_nil(cond))
        return _s->env()->scoped_eval(_s, second(_e));
    return _s->env()->scoped_eval(_s, third(_e));
}

Expr*
core::_typeof(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) > 1)
        throw ProgramError("typeof", "expected 1 argument, got", args);
    if (is_nil(first(args)))
        return nullptr;
    switch (first(args)->type) {
    case TYPE_CONS:        return _s->env()->symbol("cons");
    case TYPE_LAMBDA:      return _s->env()->symbol("lambda");
    case TYPE_MACRO:       return _s->env()->symbol("macro");
    case TYPE_FUNCTION:    return _s->env()->symbol("function");
    case TYPE_BUILDIN:     return _s->env()->symbol("buildin");
    case TYPE_REAL:        return _s->env()->symbol("real");
    case TYPE_DECIMAL:     return _s->env()->symbol("decimal");
    case TYPE_SMALLSYMBOL: return _s->env()->symbol("symbol");
    case TYPE_SYMBOL:      return _s->env()->symbol("symbol");
    case TYPE_SMALLSTRING: return _s->env()->symbol("string");
    case TYPE_STRING:      return _s->env()->symbol("string");
    default:
        throw ProgramError("typeof", "Could not determine type of input from", first(args));
    };
}

Expr*
core::_try(VariableScope* _s, Expr* _e)
{
    Expr* res = nullptr;
    Expr* err = nullptr;
    try {
        res = _s->env()->scoped_eval(_s, first(_e));
    } catch (Throwable& e) {
        if (e.is_error())
            err = _s->env()->list({
                    _s->env()->symbol("error"),
                    _s->env()->string(e.error_msg()),
                    e.error_expr()
                });
        if (e.is_thrown())
            err = e.thrown();
        if (e.is_returned())
            throw e;
    }
    if (is_nil(err))
        return res;
    /*Check all requirements for catching is in place*/
    if (len(_e) < 2)
        return err;
    if (is_nil(second(_e)) || second(_e)->type != TYPE_SYMBOL)
        throw ProgramError("try", "arg 2 expected symbol for error, not", second(_e));
    if (is_nil(third(_e)) || !is_cons(_e))
        throw ProgramError("try", "arg 3 expected expression for error evaluation, not", third(_e));

    VariableScope internal = _s->create_internal();
    internal.add_variable(second(_e)->symbol, err);
    res = _s->env()->scoped_eval(&internal, third(_e));
    return res;
}


Expr*
core::_throw(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    throw UserThrow(_s->env()->put_in_list(_s->env()->symbol("error"), args));
    return nullptr;
}

Expr*
core::_return(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) <= 1)
        throw UserReturn(first(args));
    throw ProgramError("return", "Return can only have 0-1 argument, got", args);
    return nullptr;
}

Expr*
core::variable_definition(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1)
        throw ProgramError("variable-definition", "expects single arg, but got", args);
    if (first(args)->type != TYPE_SYMBOL)
        throw ProgramError("variable-definition", "expects symbol variable, but got", first(args));
    return _s->variable_get(first(args)->symbol);
}

Expr*
core::defconst(VariableScope* _s, Expr* _e)
{
    Expr* val = _s->env()->scoped_eval(_s, second(_e));
    if (len(_e) != 2)
        throw ProgramError("const!", "expects symbol and value, got", _e);
    if (first(_e)->type != TYPE_SYMBOL)
        throw ProgramError("const!", "expects name to be type symbol, got", first(_e));
    _s->add_constant(first(_e)->symbol, val);
    return first(_e);
}

Expr*
core::defglobal(VariableScope* _s, Expr* _e)
{
    Expr* val = _s->env()->scoped_eval(_s, second(_e));
    if (len(_e) != 2)
        throw ProgramError("global!", "expects symbol and value, got", _e);
    if (first(_e)->type != TYPE_SYMBOL)
        throw ProgramError("global!", "expects name to be type symbol, got", first(_e));
    _s->add_global(first(_e)->symbol, val);
    return first(_e);
}

Expr*
core::deflocal(VariableScope* _s, Expr* _e)
{
    Expr* val = _s->env()->scoped_eval(_s, second(_e));
    if (len(_e) != 2)
        throw ProgramError("local!", "expects symbol and value, got", _e);
    if (first(_e)->type != TYPE_SYMBOL)
        throw ProgramError("local!", "expects name to be type symbol, got", first(_e));
    _s->add_variable(first(_e)->symbol, val);
    return first(_e);
}

//Expr*
//core::setvar(VariableScope* _s, Expr* _e)
//{
//    Expr* var = nullptr;
//    if (first(_e)->type != TYPE_SYMBOL)
//        throw ProgramError("set!", "Expected symbol name to set");
//    if (len(_e) != 2)
//        throw ProgramError("set!", "Expected value to set");
//    var = _s->variable_get(first(_e)->symbol);
//    if (is_nil(var))
//        throw ProgramError("set!", "variable to set does not exist");
//    if (_s->is_var_const(var))
//        throw ProgramError("set!", "Cannot set constant symbol");
//    cdr(var)->cons.car = _s->env()->scoped_eval(_s, second(_e));
//    return second(var);
//}

Expr*
core::setcar(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (first(args)->type != TYPE_CONS)
        throw ProgramError("setcar!", "Expected cons to modify, got", first(args));
    first(args)->cons.car = second(args);
    return second(args);
}

Expr*
core::setcdr(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (first(args)->type != TYPE_CONS)
        throw ProgramError("setcdr!", "Expected cons to modify, got", first(args));
    first(args)->cons.cdr = second(args);
    return second(args);
}

Expr*
core::deflambda(VariableScope* _s, Expr* _e)
{
    UNUSED(_s);
    Expr* l = _s->env()->blank();
    if (len(_e) < 1)
        throw ProgramError("lambda", "expected arguments to be binds and body, got", _e);
    l->type = TYPE_LAMBDA;
    l->callable.binds = car(_e);
    l->callable.body = cdr(_e);
    return l;
}

Expr*
core::deffunction(VariableScope* _s, Expr* _e)
{
    Expr* fn = _s->env()->blank();
    if (len(_e) < 2)
        throw ProgramError("fn!", "expected arguments to be name, binds and body, got", _e);
    fn->type = TYPE_FUNCTION;
    fn->callable.binds = second(_e);
    fn->callable.body = cdr(cdr(_e));
    _s->add_constant(first(_e)->symbol, fn);
    return first(_e);
}

Expr*
core::defmacro(VariableScope* _s, Expr* _e)
{
    Expr* macro = _s->env()->blank();
    if (len(_e) < 2)
        throw ProgramError("macro!", "expected arguments to be name, binds and body, got", _e);
    macro->type = TYPE_MACRO;
    macro->callable.binds = second(_e);
    macro->callable.body = cdr(cdr(_e));
    _s->add_constant(first(_e)->symbol, macro);
    return first(_e);
}

#endif /*YALPP_IMPLEMENTATION*/
#endif /*YALCPP_H*/

}; /*namespace yal*/
