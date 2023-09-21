#include <cmath>
#include <iostream>
#include <fstream>
#include <limits>
#include <sstream>
#include <initializer_list>
#include <algorithm>
#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <functional>
#include <cassert>
#include <math.h>
#include <chrono>

#include <unordered_map>
//https://www.educative.io/answers/what-is-a-hash-map-in-cpp

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

    TYPE_BUILDIN,
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

Throwable NotImplemented(const std::string& _fn);
Throwable InternalError(const std::string& _msg);
Throwable ProgramError(const std::string& _fn, const std::string& _msg, Expr *_specification);
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
std::stringstream stream(Expr* _e, bool _wrap_quotes);
std::string stringify(Expr* _e, bool _wrap_quotes=true);


namespace core {

Expr* quote(VariableScope* _s, Expr* _e);
Expr* reverse_ip(VariableScope* _s, Expr* _e);
Expr* range(VariableScope* _s, Expr* _e);

Expr* car(VariableScope* _s, Expr* _e);
Expr* cdr(VariableScope* _s, Expr* _e);
Expr* cons(VariableScope* _s, Expr* _e);

Expr* plus2(VariableScope* _s, Expr* _e);
Expr* minus2(VariableScope* _s, Expr* _e);
Expr* multiply2(VariableScope* _s, Expr* _e);
Expr* divide2(VariableScope* _s, Expr* _e);

Expr* plus(VariableScope* _s, Expr* _e);
Expr* minus(VariableScope* _s, Expr* _e);
Expr* multiply(VariableScope* _s, Expr* _e);
Expr* divide(VariableScope* _s, Expr* _e);
Expr* lessthan(VariableScope* _s, Expr* _e);
Expr* greaterthan(VariableScope* _s, Expr* _e);
Expr* mathequal(VariableScope* _s, Expr* _e);
/*TODO: make eq a macro called similar and equal a function*/
Expr* eq(VariableScope* _s, Expr* _e);
Expr* equal(VariableScope* _s, Expr* _e);
Expr* is_nil(VariableScope* _s, Expr* _e);
Expr* _or(VariableScope* _s, Expr* _e);
Expr* _and(VariableScope* _s, Expr* _e);

Expr* slurp_file(VariableScope* _s, Expr* _e);
Expr* read(VariableScope* _s, Expr* _e);
Expr* eval(VariableScope* _s, Expr* _e);
Expr* write(VariableScope* _s, Expr* _e);
Expr* newline(VariableScope* _s, Expr* _e);
Expr* stringify(VariableScope* _s, Expr* _e);
Expr* concat2(VariableScope* _s, Expr* _e);

Expr* defconst(VariableScope* _s, Expr* _e);
Expr* defglobal(VariableScope* _s, Expr* _e);
Expr* deflocal(VariableScope* _s, Expr* _e);
Expr* deflambda(VariableScope* _s, Expr* _e);
/*TODO: make deffunction into a macro that creates a lambda*/
Expr* deffunction(VariableScope* _s, Expr* _e);
Expr* defmacro(VariableScope* _s, Expr* _e);
Expr* macro_expand(VariableScope* _s, Expr* _e);
Expr* setcar(VariableScope* _s, Expr* _e);
Expr* setcdr(VariableScope* _s, Expr* _e);
Expr* variable_definition(VariableScope* _s, Expr* _e);

Expr* _try(VariableScope* _s, Expr* _e);
Expr* _throw(VariableScope* _s, Expr* _e);
Expr* _return(VariableScope* _s, Expr* _e);

Expr* _if(VariableScope* _s, Expr* _e);
Expr* _typeof(VariableScope* _s, Expr* _e);

/*NOTE: A lot of these can be calculated using the above buildins, but are
        pulled from c++ stl for convenience.
*/
Expr* cos(VariableScope* _s, Expr* _e);
Expr* sin(VariableScope* _s, Expr* _e);
Expr* tan(VariableScope* _s, Expr* _e);
Expr* acos(VariableScope* _s, Expr* _e);
Expr* asin(VariableScope* _s, Expr* _e);
Expr* atan(VariableScope* _s, Expr* _e);
Expr* cosh(VariableScope* _s, Expr* _e);
Expr* sinh(VariableScope* _s, Expr* _e);
Expr* tanh(VariableScope* _s, Expr* _e);

Expr* floor(VariableScope* _s, Expr* _e);
Expr* ceil(VariableScope* _s, Expr* _e);

Expr* pow(VariableScope* _s, Expr* _e);
Expr* sqrt(VariableScope* _s, Expr* _e);
Expr* log(VariableScope* _s, Expr* _e);
Expr* log10(VariableScope* _s, Expr* _e);
Expr* exp(VariableScope* _s, Expr* _e);

Expr* get_time(VariableScope* _s, Expr* _e);

}; /*namespace core*/

const char* std_lib(void);

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
    friend class Environment;
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
    Expr* variables(void);
	bool add_constant(const std::string& _name, Expr* _v);
    bool add_global(const std::string& _name, Expr* _v);
	bool add_buildin(const std::string& _name, const BuildinFn _fn);
    bool add_local(const std::string& _name, Expr* _v);
    void bind(const std::string& _fnname, Expr* _binds, Expr* _values);

private:
    VariableScope* m_outer = nullptr;
    Environment* m_env;
    //Expr* m_variables = nullptr;
    std::unordered_map<std::string, Expr*> m_variables;
};

class Environment {
    friend class VariableScope;
public:
    Environment(void);
    VariableScope* global_scope(void);

    /*Info*/
    size_t exprs_in_use(void);
    const std::string gc_info(void);

    /*Evaluation*/
    Expr* read(const std::string& _program);
    Expr* eval(Expr* _e);
    /*TODO: put these into private and access as friend*/
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
	bool add_constant(const std::string& _name, Expr* _v);
    bool add_global(const std::string& _name, Expr* _v);
	bool add_buildin(const std::string& _name, const BuildinFn _fn);
    bool add_local(const std::string& _name, Expr* _v);

    const std::stringstream& outbuffer_put(const std::string& _put);
    std::string outbuffer_getreset();

private:
    std::stringstream m_outbuffer;
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
InternalError(const std::string& _msg)
{
    std::stringstream ss;
    Throwable tw;
    tw.m_type = THROWABLE_ERROR;
    ss << "[INTERNAL] " << _msg;
    tw.m_error_msg = ss.str();
    return tw;
}

Throwable
NotImplemented(const std::string& _fn)
{
    std::stringstream ss;
    Throwable tw;
    tw.m_type = THROWABLE_ERROR;
    ss << "[" << _fn << "] is not implemented" ;
    tw.m_error_msg = ss.str();
    return tw;
}

Throwable
ProgramError(const std::string& _fn, const std::string& _msg, Expr *_specification) {
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

Expr*
VariableScope::variable_get(const std::string& _s)
{
    Expr* v = nullptr;
    VariableScope* scope = this;
    while (scope != nullptr) {
        v = scope->variable_get_this_scope(_s);
        if (!is_nil(v))
            return v;
        scope = scope->m_outer;
    }
    return nullptr;
}

Expr*
VariableScope::variable_get_this_scope(const std::string& _s)
{
    if (m_variables.find(_s) == m_variables.end())
        return nullptr;
    return m_variables[_s];
}

bool
VariableScope::add_local(const std::string& _name, Expr* _v)
{
    Expr* entry;
    if (!is_nil(variable_get_this_scope(_name)))
        return false;
    entry = env()->list({_v});
    m_variables.insert({_name, entry});
    return true;
}

bool
VariableScope::add_constant(const std::string& _name, Expr* _v)
{
    Expr* entry;
    if (!is_nil(variable_get(_name)))
        return false;
    entry = env()->list({_v, env()->symbol(constvar_id)});
    global_scope()->m_variables.insert({_name, entry});
    return true;
}

bool
VariableScope::add_global(const std::string& _name, Expr* _v)
{
    return global_scope()->add_local(_name, _v);
}

bool
VariableScope::add_buildin(const std::string& _name, const BuildinFn _fn)
{
    return add_constant(_name, env()->buildin(_fn));
}

void
VariableScope::bind(const std::string& _fnname, Expr* _binds, Expr* _values)
{
    /*TODO: add support for keyword binds aswell*/
    //std::cout << "binding for " << _fnname << std::endl;
    //std::cout << "    binding    " << stringify(_binds) << std::endl;
    //std::cout << "    binding to " << stringify(_values) << std::endl;

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
            add_local(bindstr.c_str()+1, rest);
            //std::cout << "&bound " << bindstr.c_str()+1 << " to " << stringify(rest) << std::endl;
            _binds = cdr(_binds);
            _values = nullptr;
            break;
        }
        add_local(car(_binds)->symbol, car(_values));
        //std::cout << "bound " << stringify(car(_binds)) << " to " << stringify(car(_values)) << std::endl;
        _binds = cdr(_binds);
        _values = cdr(_values);
    }
    //if (len(_binds) != 0)
    //throw ProgramError(_fnname, "leftover arg field, could not bind", _binds);
    if (len(_values) > 0)
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
    ss << "Expr size:   " << sizeof(yal::Expr) << " bytes" << std::endl
       << "used Expr's: " << exprs_in_use() << std::endl
       << "used memory: " << float(exprs_in_use() * sizeof(yal::Expr)) / 1000000
       << " MB";
    return ss.str();
}


bool
Environment::add_local(const std::string& _name, Expr* _v)
{
    return m_global_scope.add_local(_name, _v);
}

bool
Environment::add_global(const std::string& _name, Expr* _v)
{
    return m_global_scope.add_global(_name, _v);
}

bool
Environment::add_constant(const std::string& _name, Expr* _v)
{
    return m_global_scope.add_constant(_name, _v);
}

bool
Environment::add_buildin(const std::string& _name, const BuildinFn _fn)
{
    return add_constant(_name, buildin(_fn));
}

Environment::Environment(void) : m_global_scope(this, nullptr) {}

std::string
_try_get_special_token(const char* _start)    
{
    const std::string stokens[] = {
        "(", ")", /*Lists*/
        "[", "]", /*array*/
        "'", "`", ",@", ",",  /*quoting, quasi-quoting, unquoting & unquote-splicing*/
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
_get_next_token(const std::string& _source, int& _cursor)
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
tokenize(const std::string& _str) {
    /*TODO:  Do error checking like matching+even parens*/
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
        _tokens.pop_front();
        program = list({symbol("quote"), lex(_tokens)});
        return program;
    }
    if (token == "`") {
        _tokens.pop_front();
        program = list({symbol("quasiquote"), lex(_tokens)});
        return program;
    }
    if (token == ",") {
        _tokens.pop_front();
        program = list({symbol("unquote"), lex(_tokens)});
        return program;
    }
    if (token == ",@") {
        _tokens.pop_front();
        program = list({symbol("unquote-splicing"), lex(_tokens)});
        return program;
    }
    program = lex_value(token);
    _tokens.pop_front();
    return program;
}

Expr*
Environment::read(const std::string& _program)
{
    std::list<std::string> tokens;
    Expr* lexed = nullptr;
    tokens = tokenize(_program);
    if (tokens.empty())
        return nullptr;
    lexed = lex(tokens);
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
_macro_expander(VariableScope* _s, const std::string& _macroname, Expr* _macrofn, Expr* _macroargs) {
    auto macro_expand = [] (auto macro_expand, VariableScope* scope, Expr* body) {
        if (is_nil(body))
            return body;
        if (body->type == TYPE_SYMBOL) {
            Expr* sym = first(scope->variable_get_this_scope(body->symbol));
            if (is_nil(sym)) {
                return body;
            }
            sym = scope->env()->list({scope->env()->symbol("quote"), sym});
            return sym;
        }
        if (body->type == TYPE_CONS) {
            body->cons.car = macro_expand(macro_expand, scope, car(body));
            body->cons.cdr = macro_expand(macro_expand, scope, cdr(body));
            return body;
        }
        return body;
    };
    auto progn = [] (VariableScope* s, Expr* e) {
        Expr* last = nullptr;
            while (!is_nil(e)) {
                try {
                    last = s->env()->scoped_eval(s, car(e));
                } catch (Throwable& e) {
                    if (e.is_error())
                        return s->env()->list({s->env()->symbol("error"), s->env()->string(e.error_msg()), e.error_expr()});
                    if (e.is_thrown())
                        return e.thrown();
                    if (e.is_returned())
                        return e.returned();
                }
                e = cdr(e);
        }
        return last;
    };

    VariableScope internal = _s->create_internal();
    internal.bind(_macroname, _macrofn->callable.binds, _macroargs);
    Expr* body_copy = _s->env()->read(stringify(_macrofn->callable.body));
    Expr* expanded = macro_expand(macro_expand, &internal, body_copy);
    expanded = progn(_s, expanded);
    return expanded;
}

Expr*
Environment::scoped_eval(VariableScope* _scope, Expr* _e)
{
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
    auto eval_fn = [progn, this, _scope] (const std::string& name, Expr* fn, Expr* args) {
        VariableScope internal = _scope->create_internal();
        args = list_eval(&internal, args);
        internal.bind(name, fn->callable.binds, args);
        return progn(&internal, fn->callable.body);
    };

    if (is_nil(_e)) return nil();

    /*Handle non-evaluating types*/
    if (_e->type == TYPE_REAL       ||
        _e->type == TYPE_DECIMAL    ||
        _e->type == TYPE_STRING
        )
        return _e;

    /*Handle symbols*/
    if (_e->type == TYPE_SYMBOL) {
        Expr* var = nullptr;
        if (is_nil(_e))
            return nil();
        if (_e->symbol == std::string("t") || _e->symbol == std::string("T"))
            return t();
        var = _scope->variable_get(_e->symbol);
        if (!is_nil(var))
            return first(var);
        throw ProgramError("eval", "could not find existing symbol called", _e);
        //return nil();
    }

    /*Handle function calls*/
    if (_e->type == TYPE_CONS) {
        std::string name;
        Expr* fn = nullptr;
        Expr* args = nullptr;
        fn = car(_e);
        args = cdr(_e);
        if (fn->type == TYPE_CONS)
            fn = scoped_eval(_scope, fn);
        if (fn->type == TYPE_SYMBOL) {
            name = fn->symbol;
            fn = first(_scope->variable_get(fn->symbol));
        }
        else {
            name = "lambda";
        }
        if (is_nil(fn))
            throw ProgramError("eval", "could not evaluate unknown function", first(_e));

        if (fn->type == TYPE_BUILDIN) {
            return fn->buildin(_scope, args);
        }
        if (fn->type == TYPE_LAMBDA) {
            return eval_fn(name, fn, args);
        }
        if (fn->type == TYPE_MACRO) {
            return scoped_eval(_scope, _macro_expander(_scope, name, fn, args));
        }
        throw ProgramError("eval", "could not find function called", fn);
    }
    throw ProgramError("eval", "Got something that could not be evaluated", _e);
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
        add_buildin("setcar!", core::setcar);
        add_buildin("setcdr!", core::setcdr);
        add_buildin("fn!", core::deffunction);
        add_buildin("macro!", core::defmacro);
        add_buildin("macro-expand", core::macro_expand);
        add_buildin("lambda", core::deflambda);
        add_buildin("quote", core::quote);
        //add_buildin("error", core::quote);
        add_buildin("cons", core::cons);
        add_buildin("range", core::range);
        add_buildin("reverse!", core::reverse_ip);
        add_buildin("car", core::car);
        add_buildin("cdr", core::cdr);
        //add_buildin("_PLUS2", core::plus2);
        //add_buildin("_MINUS2", core::minus2);
        //add_buildin("_MULTIPLY2", core::multiply2);
        //add_buildin("_DIVIDE2", core::divide2);
        add_buildin("+", core::plus);
        add_buildin("-", core::minus);
        add_buildin("*", core::multiply);
        add_buildin("/", core::divide);
        add_buildin("=", core::mathequal);
        add_buildin("<", core::lessthan);
        /*TODO: greaterthan is just the opposite of lessthan*/
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
        add_buildin("newline", core::newline);
        add_buildin("stringify", core::stringify);
        add_buildin("_concat2", core::concat2);

        add_constant("real-max", real(std::numeric_limits<int>::max()));
        add_constant("real-min", real(std::numeric_limits<int>::min()));
        add_constant("decimal-max", decimal(std::numeric_limits<float>::max()));
        add_constant("decimal-min", decimal(std::numeric_limits<float>::min()));

        add_buildin("cos", core::cos);
        add_buildin("cosh", core::cosh);
        add_buildin("acos", core::acos);
        add_buildin("sin", core::sin);
        add_buildin("sinh", core::sinh);
        add_buildin("asin", core::asin);
        add_buildin("tan", core::tan);
        add_buildin("tanh", core::tanh);
        add_buildin("atan", core::atan);

        add_buildin("floor", core::floor);
        add_buildin("ceil", core::ceil);
        add_buildin("pow", core::pow);
        add_buildin("sqrt", core::sqrt);

        add_buildin("log", core::log);
        add_buildin("log10", core::log10);
        add_buildin("exp", core::exp);

        add_buildin("time", core::get_time);
    }
    catch (std::exception& e) {
        std::cout << "something wrong with buildins!" << std::endl
                  << e.what() << std::endl;
        return false;
    }
    return true;
}

bool
Environment::load_std(void)
{
    load_core();
    eval(read(" (fn! progn (&body)"
              "   \"evaluate body and return last result\""
              "   (if (nil? body)"
              "     NIL"
              "     (last body)))"));
    eval(read(std_lib()));
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
     Expr *out = m_gc.new_variable();
     if (out == nullptr)
       return nullptr;
     out->type = TYPE_SYMBOL;
     out->symbol = to_cstr(_v);
     return out;
}

Expr *
Environment::string(const std::string& _v) {
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


const std::stringstream&
Environment::outbuffer_put(const std::string& _put)
{
    m_outbuffer << _put;
    return m_outbuffer;
}

std::string
Environment::outbuffer_getreset()
{
    std::string out = m_outbuffer.str();
    m_outbuffer.str("");
    m_outbuffer.clear();
    return out;
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
stream(Expr* _e, bool _wrap_quotes)
{
    auto stream_value = [_wrap_quotes] (Expr* e) {
        std::stringstream ss;
        if (is_nil(e)) {
            ss << "NIL";
            return ss;
        }
        switch (e->type) {
        case TYPE_CONS:        ss << "(" << stream(e, _wrap_quotes).str() << ")"; break;
        case TYPE_LAMBDA:      ss << "#<lambda>";                   break;
        case TYPE_MACRO:       ss << "#<macro>";                    break;
            //case TYPE_FUNCTION:    ss << "#<function>";                 break;
        case TYPE_BUILDIN:     ss << "#<buildin>";                  break;
        case TYPE_REAL:        ss << e->real;                       break;
        case TYPE_DECIMAL:     ss << e->decimal;                    break;
        case TYPE_SYMBOL:      ss << e->symbol;                     break;
        case TYPE_STRING:      
            if (_wrap_quotes)
                ss << "\"" << e->string << "\"";
            else
              ss << e->string;
            break;
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
        ss << "(" << stream(car(_e), _wrap_quotes).str() << " . "
           << stream(cdr(_e), _wrap_quotes).str() << ")";
        return ss;
    }
    ss << "(";
    ss << stream(car(curr), _wrap_quotes).str();
    curr = cdr(curr);
    while (!is_nil(curr)) {
        ss << " " << stream(car(curr), _wrap_quotes).str();
        curr = cdr(curr);
    }
    ss << ")";
    return ss;
}

std::string
stringify(Expr* _e, bool _wrap_quotes)
{
    std::stringstream ss = stream(_e, _wrap_quotes);
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
core::plus2(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* a = first(args);
    Expr* b = second(args);

    if (len(args) == 0)
        return _s->env()->real(0);
    if (len(args) == 1)
        return a;
    if (!is_val(a))
        throw ProgramError("+", "cannot do math on non-value ", a);
    if (!is_val(b))
        throw ProgramError("+", "cannot do math on non-value ", b);

    if (a->type == TYPE_DECIMAL && b->type == TYPE_DECIMAL)
        return _s->env()->decimal(a->decimal + b->decimal);
    else if (a->type == TYPE_REAL && b->type == TYPE_DECIMAL)
        return _s->env()->decimal(a->real + b->decimal);
    else if (a->type == TYPE_DECIMAL && b->type == TYPE_REAL)
        return  _s->env()->decimal(a->decimal + b->real);
    return _s->env()->real(a->real + b->real);
}

Expr*
core::minus2(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* a = first(args);
    Expr* b = second(args);

    if (len(args) == 0)
        return _s->env()->real(0);
    if (len(args) == 1) {
        if (a->type == TYPE_REAL)
            return _s->env()->real(a->real * -1);
        if (a->type == TYPE_DECIMAL)
            return _s->env()->decimal(a->decimal * -1);
    }
    if (!is_val(a))
        throw ProgramError("-", "cannot do math on non-value ", a);
    if (!is_val(b))
        throw ProgramError("-", "cannot do math on non-value ", b);

    if (a->type == TYPE_DECIMAL && b->type == TYPE_DECIMAL)
        return _s->env()->decimal(a->decimal - b->decimal);
    else if (a->type == TYPE_REAL && b->type == TYPE_DECIMAL)
        return _s->env()->decimal(a->real - b->decimal);
    else if (a->type == TYPE_DECIMAL && b->type == TYPE_REAL)
        return  _s->env()->decimal(a->decimal - b->real);
    return _s->env()->real(a->real - b->real);
}

Expr*
core::multiply2(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* a = first(args);
    Expr* b = second(args);

    if (len(args) == 1)
        return _s->env()->real(0);
    if (len(args) == 1)
        return a;
    if (!is_val(a))
        throw ProgramError("*", "cannot do math on non-value ", a);
    if (!is_val(b))
        throw ProgramError("*", "cannot do math on non-value ", b);

    if (a->type == TYPE_DECIMAL && b->type == TYPE_DECIMAL)
        return _s->env()->decimal(a->decimal * b->decimal);
    else if (a->type == TYPE_REAL && b->type == TYPE_DECIMAL)
        return _s->env()->decimal(a->real * b->decimal);
    else if (a->type == TYPE_DECIMAL && b->type == TYPE_REAL)
        return  _s->env()->decimal(a->decimal * b->real);
    return _s->env()->real(a->real * b->real);
}

Expr*
core::divide2(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* a = first(args);
    Expr* b = second(args);

    if (len(args) == 1)
        return _s->env()->real(0);
    if (len(args) == 1)
        return a;
    if (!is_val(a))
        throw ProgramError("/", "cannot do math on non-value ", a);
    if (!is_val(b))
        throw ProgramError("/", "cannot do math on non-value ", b);

    if (a->type == TYPE_DECIMAL && b->type == TYPE_DECIMAL)
        return _s->env()->decimal(a->decimal / b->decimal);
    else if (a->type == TYPE_REAL && b->type == TYPE_DECIMAL)
        return _s->env()->decimal(a->real / b->decimal);
    else if (a->type == TYPE_DECIMAL && b->type == TYPE_REAL)
        return  _s->env()->decimal(a->decimal / b->real);
    return _s->env()->real(a->real / b->real);
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
    if (len(args) < 1)
        std::cout << "NIL" << std::endl;
    if (len(args) > 1)
        throw ProgramError("write", "expects single argument to write, not", args);
    _s->env()->outbuffer_put(stringify(first(args)));
    return first(args);
}

Expr*
core::newline(VariableScope* _s, Expr* _e)
{
    if (len(_e) != 0)
        throw ProgramError("newline", "expects no args, got", _e);
    _s->env()->outbuffer_put("\n");
    return _s->env()->nil();
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
core::concat2(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) == 0)
        return _s->env()->string("");
    if (len(args) == 1)
        return _s->env()->string(stringify(first(args), false));
    if (len(args) == 2)
        return _s->env()->string(stringify(first(args), false) + stringify(second(args), false));
    throw ProgramError("concat", "can only concatenate up to 2 args, not", args);
   
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
        //case TYPE_FUNCTION:    return _s->env()->symbol("function");
    case TYPE_BUILDIN:     return _s->env()->symbol("buildin");
    case TYPE_REAL:        return _s->env()->symbol("real");
    case TYPE_DECIMAL:     return _s->env()->symbol("decimal");
    case TYPE_SYMBOL:      return _s->env()->symbol("symbol");
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
    internal.add_local(second(_e)->symbol, err);
    res = _s->env()->scoped_eval(&internal, third(_e));
    return res;
}


Expr*
core::_throw(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    throw UserThrow(_s->env()->put_in_list(_s->env()->symbol("error"), args));
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
    Expr* var = nullptr;
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1)
        throw ProgramError("variable-definition", "expects single arg, but got", args);
    if (first(args)->type != TYPE_SYMBOL)
        throw ProgramError("variable-definition", "expects symbol variable, but got", first(args));
    
    var = _s->variable_get(first(args)->symbol);
    if (is_nil(var))
        return _s->env()->nil();
    return var;
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
    _s->add_local(first(_e)->symbol, val);
    return first(_e);
}

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
    fn->type = TYPE_LAMBDA;
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

Expr*
core::macro_expand(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || first(args)->type != TYPE_CONS)
        throw ProgramError("macro-expand", "expected macro call as list, got", args);
    Expr* macro_call = first(args);
    if (first(macro_call)->type != TYPE_SYMBOL)
        throw ProgramError("macro-expand",
                           "expected first arg to be a symbol pointing to a macro, not",
                           first(macro_call));

    const std::string macro_name = first(macro_call)->symbol;
    Expr* macro_fn = first(_s->variable_get(first(macro_call)->symbol));
    Expr* macro_args = cdr(macro_call);
    Expr* expanded = _macro_expander(_s, macro_name, macro_fn, macro_args);
    return expanded;
}

Expr*
core::cos(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw ProgramError("cos", "expected 1 value arg, got", args);
    if (first(args)->type == TYPE_DECIMAL)
        return _s->env()->decimal(std::cos(first(args)->decimal));
    return _s->env()->decimal(std::cos(first(args)->real));
}

Expr*
core::sin(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw ProgramError("sin", "expected 1 value arg, got", args);
    if (first(args)->type == TYPE_DECIMAL)
        return _s->env()->decimal(std::sin(first(args)->decimal));
    return _s->env()->decimal(std::sin(first(args)->real));
}

Expr*
core::tan(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw ProgramError("tan", "expected 1 value arg, got", args);
    if (first(args)->type == TYPE_DECIMAL)
        return _s->env()->decimal(std::tan(first(args)->decimal));
    return _s->env()->decimal(std::tan(first(args)->real));
}

Expr*
core::acos(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw ProgramError("acos", "expected 1 value arg, got", args);
    if (first(args)->type == TYPE_DECIMAL)
        return _s->env()->decimal(std::acos(first(args)->decimal));
    return _s->env()->decimal(std::acos(first(args)->real));
}
    
Expr*
core::asin(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw ProgramError("asin", "expected 1 value arg, got", args);
    if (first(args)->type == TYPE_DECIMAL)
        return _s->env()->decimal(std::asin(first(args)->decimal));
    return _s->env()->decimal(std::asin(first(args)->real));
}
    
Expr*
core::atan(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw ProgramError("atan", "expected 1 value arg, got", args);
    if (first(args)->type == TYPE_DECIMAL)
        return _s->env()->decimal(std::atan(first(args)->decimal));
    return _s->env()->decimal(std::atan(first(args)->real));
}
    
Expr*
core::cosh(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw ProgramError("cosh", "expected 1 value arg, got", args);
    if (first(args)->type == TYPE_DECIMAL)
        return _s->env()->decimal(std::cosh(first(args)->decimal));
    return _s->env()->decimal(std::cosh(first(args)->real));
}
    
Expr*
core::sinh(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw ProgramError("sinh", "expected 1 value arg, got", args);
    if (first(args)->type == TYPE_DECIMAL)
        return _s->env()->decimal(std::sinh(first(args)->decimal));
    return _s->env()->decimal(std::sinh(first(args)->real));
}
    
Expr*
core::tanh(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw ProgramError("tanh", "expected 1 value arg, got", args);
    if (first(args)->type == TYPE_DECIMAL)
        return _s->env()->decimal(std::tanh(first(args)->decimal));
    return _s->env()->decimal(std::tanh(first(args)->real));
}
    
Expr*
core::floor(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw ProgramError("floor", "expected 1 value arg, got", args);
    if (first(args)->type == TYPE_DECIMAL)
        return _s->env()->real(std::floor(first(args)->decimal));
    return _s->env()->real(std::floor(first(args)->real));
}
    
Expr*
core::ceil(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw ProgramError("ceil", "expected 1 value arg, got", args);
    if (first(args)->type == TYPE_DECIMAL)
        return _s->env()->real(std::ceil(first(args)->decimal));
    return _s->env()->real(std::ceil(first(args)->real));
}

Expr*
core::sqrt(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw ProgramError("sqrt", "expected 1 value arg, got", args);
    if (first(args)->type == TYPE_DECIMAL)
        return _s->env()->decimal(std::sqrt(first(args)->decimal));
    return _s->env()->decimal(std::sqrt(first(args)->real));
}
    
Expr*
core::log(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw ProgramError("log", "expected 1 value arg, got", args);
    if (first(args)->type == TYPE_DECIMAL)
        return _s->env()->real(std::log(first(args)->decimal));
    return _s->env()->real(std::log(first(args)->real));
}
    
Expr*
core::log10(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw ProgramError("log10", "expected 1 value arg, got", args);
    if (first(args)->type == TYPE_DECIMAL)
        return _s->env()->real(std::log10(first(args)->decimal));
    return _s->env()->real(std::log10(first(args)->real));
}

Expr*
core::exp(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw ProgramError("exp", "expected 1 value arg, got", args);
    if (first(args)->type == TYPE_DECIMAL)
        return _s->env()->real(std::exp(first(args)->decimal));
    return _s->env()->real(std::exp(first(args)->real));
}
    
Expr*
core::pow(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* a = first(args);
    Expr* b = second(args);
    if (len(args) != 2 || !is_val(a) || !is_val(b))
        throw ProgramError("pow", "expected 2 value args, got", args);
    if (a->type == TYPE_DECIMAL && b->type == TYPE_DECIMAL)
        return _s->env()->decimal(std::pow(a->decimal, b->decimal));
    if (a->type == TYPE_REAL && b->type == TYPE_DECIMAL)
        return _s->env()->decimal(std::pow(a->real, b->decimal));
    if (a->type == TYPE_DECIMAL && b->type == TYPE_REAL)
        return _s->env()->decimal(std::pow(a->decimal, b->real));
    return _s->env()->decimal(std::pow(a->real, b->real));
}

Expr*
core::get_time(VariableScope *_s, Expr *_e)
{
  if (len(_e) != 0)
      throw ProgramError("time", "expects no arguments, got", _e);

  using namespace std::chrono;
  const auto c = std::chrono::system_clock::now();
  long h = std::chrono::duration_cast<std::chrono::hours>(c.time_since_epoch()).count();
  long m = std::chrono::duration_cast<std::chrono::minutes>(c.time_since_epoch()).count();
  long s = std::chrono::duration_cast<std::chrono::seconds>(c.time_since_epoch()).count();
  long ms = std::chrono::duration_cast<std::chrono::milliseconds>(c.time_since_epoch()).count();

  return _s->env()->list({
          _s->env()->real(h),
          _s->env()->real(m % 60),
          _s->env()->real(s % 60),
          _s->env()->real(ms % 1000),
      });
}


const char*
std_lib(void)
{
   return "(progn "
    /* =======================================================================
      TOOL INFO
    ======================================================================= */
    " (const! *creator* \"Thomas Alexgaard\")"
       //" (const! *creator-git* \"https://github.com/thom9258/\")"
    " (const! *host* \"C++\")"

    /* =======================================================================
      PREDICATES
    ======================================================================= */
    " (fn! notnil?  (v) (not (nil? v)))"
    " (fn! real?    (v) (= (typeof v) 'real))"
    " (fn! decimal? (v) (= (typeof v) 'decimal))"
    " (fn! value?   (v) (or (= (typeof v) 'real) (= (typeof v) 'decimal)))"
    " (fn! symbol?  (v) (= (typeof v) 'symbol))"
    " (fn! string?  (v) (= (typeof v) 'string))"
    " (fn! cons?    (v) (= (typeof v) 'cons))"
    " (fn! list?    (v) (or (nil? v) (= (typeof v) 'cons)))"
    " (fn! atom?    (v) (or (nil? v) (not (cons? v))))"
    " (fn! buildin? (v) (= (typeof v) 'buildin))"
    " (fn! fn?      (v) (or (buildin? v) (= (typeof v) 'lambda)))"
    " (fn! macro?   (v) (= (typeof v) 'macro))"
    " (fn! var? (var)"
    "   (and (symbol? var) (notnil? (variable-definition var))))"
    " (fn! const? (var)"
    "   (if (not (var? var)) (return NIL))"
    "   (= 'CONSTANT (second (variable-definition var))))"

    " (fn! not (x)"
    "  (if (nil? x) T NIL))"

    /* =======================================================================
      OPERATING SYSTEM INTERACTION
    ======================================================================= */
    " (fn! load-file (f) (eval (read (slurp-file f))))"

    " (fn! time-hours (t) (first t))"
    " (fn! time-minutes (t) (second t))"
    " (fn! time-seconds (t) (third t))"
    " (fn! time-milliseconds (t) (fourth t))"

    " (fn! print (&outs)"
    "   (write (apply 'concat outs)))"
    " (fn! println (&outs)"
    "   (prog1 (write (apply 'concat outs)) (newline)))"

    /* =======================================================================
       MATH
    ======================================================================= */
    " (const! PI         3.141592)"
    " (const! PI2        (* PI 2))"
    " (const! PI/2       (/ PI 2))"
    " (const! PI/3       (/ PI 3))"
    " (const! PI/4       (/ PI 4))"
    " (const! E          2.71828)"
    " (const! E-constant 0.57721)"

    //" (fn! + (&list) (foldl _PLUS2     0 list))"
    //" (fn! - (&list) (foldl _MINUS2    0 list))"
    //" (fn! * (&list) (foldl _MULTIPLY2 1 list))"
    //" (fn! / (&list) (foldl _DIVIDE2   1 list))"

    " (fn! min (a b) (if (< a b) a b))"
    " (fn! max (a b) (if (< a b) b a))"
    " (fn! clamp (val low high) (max low (min val high)))"

    " (fn! abs (v) (if (< v 0) (* -1 v) v))"
    " (fn! % (n a) (if (> n a) (% (- n a) a) a))"
    " (fn! zero? (v) (or (= v 0) (= v 0.0)))"
    " (fn! even? (v) (= (% v 2) 0))"
    " (fn! odd? (v) (= (% v 2) 1))"
    " (fn! square (v) (* v v))"
    " (fn! cube (v) (* v v v))"

    " (fn! sum-of-squares (&values)"
       "\"Calculate the sum of squares of all values provided, eg: v1^2 + v2^2 + ... + vN^2\""
    "   (apply + (transform square values)))"

    " (fn! factorial (v)"
    "   (if (= v 0) 1 (* v (factorial (- v 1)))))"

    " (fn! bin-coeff (n k)"
    "   (/ (factorial n) (factorial (- n k)) (factorial k)))"

    " (fn! fib (n)"
    "   (if (< n 2)"
    "     n"
    "     (+ (fib (- n 1)) (fib (- n 2)))))"

    /* =======================================================================
       EVALUATION
    ======================================================================= */
    " (fn! prog1 (&body)"
    "   \"evaluate body and return first result\""
    "   (if (nil? body) NIL (first body)))"

    " (fn! prog2 (&body)"
    "   \"evaluate body and return second result\""
    "   (if (nil? body) NIL (second body)))"

    " (fn! prog3 (&body)"
    "   \"evaluate body and return third result\""
    "   (if (nil? body) NIL (third body)))"

    //" (macro! when (test &body)"
    //"  (if test (progn body)))"

    //" (macro! unless (test &body)"
    //"  (if test NIL (progn body)))"

    " (fn! apply (fn list)"
    "   \"apply function to list\""
    "   (eval (put fn list)))"

    /* =======================================================================
       LISTS
    ======================================================================= */
   " (fn! list (&vars) vars)"

    " (fn! put (v l)"
    "  \"put value onto front of list\""
    "  (cons v l))"

    " (fn! len (l)"
    "  \"calculate length of list\""
    "  (if (nil? l)"
    "    0"
    "    (+ 1 (len (rest l)))))"

    /*TODO: Reimplement this with cond instead of if*/
    " (fn! contains (value list)"
    "   \"check if value is in list\""
    "   (if (nil? list)"
    "     NIL"
    "     (if (eq (car list) value)"
    "       T"
    "       (contains value (cdr list)))))"

    " (fn! assoc (value list)"
    "   \"check if value is in list\""
    "   (if (nil? list)"
    "     NIL"
    "     (if (eq (caar list) value)"
    "       (car list)"
    "       (assoc value (cdr list)))))"

    " (fn! recurse (n)"
    "   \"create recursion calls n times (used to test "
    "tail-call-optimization)\""
    "   (if (> n 0) (recurse (- n 1)) 'did-we-blow-up?))"

    " (fn! before-n (n lst)"
    "   \"create a list containing the first n values of a given list\""
    "   (if (= n 0)"
    "     NIL"
    "     (put (first lst) (before-n (- n 1) (rest lst)))))"

    " (fn! after-n (n lst)"
    "   \"create a list containing the last n values of a given list\""
    "   (if (= n 0)"
    "     lst"
    "     (after-n (- n 1) (rest lst))))"

    " (fn! split (n lst)"
    "   \"create 2 lists split at n\""
    "   [(before-n n lst) (after-n n lst)])"

    " (fn! transform (fn lst)"
    "   \"apply fn to all value in lst and collect results\""
    "   (if (nil? lst)"
    "     NIL"
    "     (put ((variable-value 'fn) (first lst)) (transform fn (rest lst)))))"

    /*https://lwh.jp/lisp/library.html*/
    " (fn! map (fn &lists)"
    "   \"apply fn to sequencially mapped values in lists and collect results\""
    "   (throw \"map does not work yet\")"   
       //"   (write fn) (write lists)"
    "   (if (nil? (car lists))"
    "     NIL"
    "     (cons (apply fn (transform 'car lists))"
    "           (apply 'map (cons fn"
    "                            (transform 'cdr lists))))))"

    " (fn! foldl (fn init list)"
    "  \"fold a list using fn and init value\""
    "   (if list"
    "   (foldl fn"
    "     (fn init (first list))"
    "     (rest list))"
    "   init))"

    " (fn! foldr (fn init list)"
    "  \"reverse fold a list using fn and init value\""
    "   (if (nil? list)"
    "     (fn (first list)"
    "         (foldr fn init (rest list)))"
    "   init))"

    " (fn! join (a b)"
    "   (if (nil? a)"
    "     b"
    "     (cons (first a) (join (rest a) b))))"

    " (fn! reverse (list)"
    "  \"reverse a list\""
    "   (foldl (lambda (a x) (cons x a)) nil list))"

    /* =======================================================================
       ACCESSORS
    ======================================================================= */
    " (fn! head  (l) (car l))"
    " (fn! tail  (l) (cdr l))"
    " (fn! first (l) (car l))"
    " (fn! rest  (l) (cdr l))"

    " (fn! second  (l) (car (cdr l)))"
    " (fn! third   (l) (car (cdr (cdr l))))"
    " (fn! fourth  (l) (car (cdr (cdr (cdr l)))))"
    " (fn! fifth   (l) (car (cdr (cdr (cdr (cdr l))))))"
    " (fn! sixth   (l) (car (cdr (cdr (cdr (cdr (cdr l)))))))"
    " (fn! seventh (l) (car (cdr (cdr (cdr (cdr (cdr (cdr l))))))))"
    " (fn! eighth  (l) (car (cdr (cdr (cdr (cdr (cdr (cdr (cdr l)))))))))"
    " (fn! ninth   (l) (car (cdr (cdr (cdr (cdr (cdr (cdr (cdr (cdr l))))))))))"
    " (fn! tenth   (l) (car (cdr (cdr (cdr (cdr (cdr (cdr (cdr (cdr (cdr "
    " l)))))))))))"

    " (fn! cddr (l) (cdr (cdr l)))"
    " (fn! cdar (l) (cdr (car l)))"
    " (fn! cadr (l) (car (cdr l)))"
    " (fn! caar (l) (car (car l)))"

    " (fn! caddr (l) (car (cdr (cdr l))))"
    " (fn! cadar (l) (car (cdr (car l))))"
    " (fn! caadr (l) (car (car (cdr l))))"
    " (fn! caaar (l) (car (car (car l))))"
    " (fn! cdddr (l) (cdr (cdr (cdr l))))"
    " (fn! cddar (l) (cdr (cdr (car l))))"
    " (fn! cdadr (l) (cdr (car (cdr l))))"
    " (fn! cdaar (l) (cdr (car (car l))))"

    " (fn! nthcdr (n list)"
    "  \"get list subset starting from n\""
    "  (if (> 1 n)"
    "    list"
    "    (nthcdr (- n 1) (rest list))))"

    " (fn! nth (n list)"
    "  \"get nth value of list\""
    "  (head (nthcdr n list)))"

    " (fn! last (list)"
    "  \"get last value of list\""
    "  (nth (- (len list) 1) list))"

    /* =======================================================================
       VARIABLES
    ======================================================================= */
    " (macro! setq! (v x)"
    "   ['set! 'v x])"

    " (fn! setnth! (n val list)"
    "   \"set the nth value of a list\""
    "   (local! target (nthcdr n list))"
    "   (if (nil? target)"
    "     (throw \"invalid index to set, got\" n)"
    "     (setcar! target val)))"

    " (fn! variable-value (var)"
    "   \"get value associated with variable\""
    "   (first (variable-definition var)))"

    " (fn! set! (var val)"
    "   \"change variable to become value on evaluation\""
    "   (if (not (var? var))"
    "     (throw \"set! expected symbol variable, not\" var))"
    "   (if (const? var) (throw \"set! cannot set constant\" var))"
    "   (setnth! 0 val (variable-definition var)))"


    /* =======================================================================
       STRING MANAGEMENT
    ======================================================================= */
    " (fn! concat (&list) (foldl _concat2 \"\" list))"

    /* =======================================================================
       MACRO-HELPERS
    ======================================================================= */
    " (macro! unquote (x)"
    "   (throw \"unquotes need to be wrapped inside quasiquotes\"))"
    " (macro! unquote-splicing (x)"
    "   (throw \"unquotes need to be wrapped inside quasiquotes\"))"

    //https://lwh.jp/lisp/quasiquotation.html
    //https://blog.veitheller.de/Lets_Build_a_Quasiquoter.html
    " (macro! quasiquote (x)"
       //"   (if (nil? x) ['NIL]"
    "   (if (cons? x)"
    "     (if (= (car x) 'unquote)"
    "       (cadr x)"
    "       (if (= (caar x) 'unquote-splicing)"
    "         ['join (cadr (car x)) ['quasiquote (cdr x)]]"
    "         ['cons ['quasiquote (car x)]"
    "                ['quasiquote (cdr x)]]))"
    "     ['quote x]))"

       //"(macro! quasiquote (x)"
       //    "(if (cons? x)"
       //        "(if (eq (car x) 'unquote)"
       //            "(cadr x)"
       //            "(if (eq (caar x) 'unquote-splicing)"
       //                "(list 'append"
       //                        "(cadr (car x))"
       //                        "(list 'quasiquote (cdr x)))"
       //                "(list 'cons"
       //                        "(list 'quasiquote (car x))"
       //                        "(list 'quasiquote (cdr x)))))"
       //        "(list 'quote x)))"


    " 'std)";
}

#endif /*YALPP_IMPLEMENTATION*/
#endif /*YALCPP_H*/

}; /*namespace yal*/
