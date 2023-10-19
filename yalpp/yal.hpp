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
#include <math.h>
#include <chrono>
#include <memory>
#include <cassert>

#include <unordered_map>
//https://www.educative.io/answers/what-is-a-hash-map-in-cpp

#ifndef YALCPP_H
#define YALCPP_H

namespace yal {

#define ASSERT_UNREACHABLE(str) assert(str && "Unmanaged execution path observed!")  
#define UNUSED(v) ((void)v)

uint8_t mark_bit = 0b10000000;

enum TYPE {
    /*We fill out the enum values manually as we need to make sure the 8th bit is not set*/
    TYPE_INVALID = 0b0,
    TYPE_CONS    = 0b00000001,
    TYPE_REAL    = 0b00000010,
    TYPE_DECIMAL = 0b00000011,
    TYPE_SYMBOL  = 0b00000100,
    TYPE_STRING  = 0b00000101,
    TYPE_BUILDIN = 0b00000110,
    TYPE_LAMBDA  = 0b00000111,
    TYPE_MACRO   = 0b00001000,
};

/*Forward Declarations*/
struct Expr;
class VariableScope;
class Environment;
typedef Expr*(*BuildinFn)(VariableScope*, Expr*);

const size_t smallXsz = (sizeof(Expr*) * 2) / sizeof(char);

class Expr {
    bool is_marked = false;
    uint8_t type = TYPE_INVALID;
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

public:
    /*'friends' are the only interaction interface with the Expr class.
      This was chosen to avoid null pointer access, as a nullptr is considered
      a valid 'Expr'*/
    friend class GarbageCollector;
    /*TODO: this is a temporary workaround, and should be excluded*/
    friend class Environment;

    friend std::ostream& operator<<(std::ostream& os, Expr* _e);
    /*type specifiers*/
    friend uint8_t type(Expr* _e);
    friend bool is_nil(Expr* _e);
    friend bool is_real(Expr* _e);
    friend bool is_decimal(Expr* _e);
    friend bool is_val(Expr* _e);
    friend bool is_string(Expr* _e);
    friend bool is_symbol(Expr* _e);
    friend bool is_buildin(Expr* _e);
    friend bool is_lambda(Expr* _e);
    friend bool is_macro(Expr* _e);
    friend bool is_cons(Expr* _e);
    friend bool is_dotted(Expr* _e);

    /*garbage management methods*/
    friend bool is_marked(Expr* _e);
    friend void set_mark(Expr* _e);
    friend void clear_mark(Expr* _e);
    
    /*value accessors*/
    friend int         get_real(Expr* _e);
    friend float       get_decimal(Expr* _e);
    friend std::string get_str(Expr* _e);
    friend const char* get_cstr(Expr* _e);
    friend std::string get_sym(Expr* _e);
    friend const char* get_csym(Expr* _e);
    friend Expr*       get_binds(Expr* _e);
    friend Expr*       get_body(Expr* _e);

    friend Expr* car(Expr* _e);
    friend Expr* cdr(Expr* _e);
    friend Expr* set_callable(Expr* _e, Expr* _binds, Expr* _body);
    friend Expr* set_car(Expr* _e, Expr* _car);
    friend Expr* set_cdr(Expr* _e, Expr* _cdr);
    friend Expr* ipreverse(Expr* _e);

    /*friend functions that extend existing friend functions*/
    friend int len(Expr* _e);
    friend Expr* first(Expr* _e); 
    friend Expr* second(Expr* _e);
    friend Expr* third(Expr* _e);
    friend Expr* fourth(Expr* _e);
};
    
/*Helper that convert std::string to allocated char ptr*/
char* str_to_cstr(const std::string& _s);

/*Stringification helpers*/
std::stringstream stream(Expr* _e, bool _wrap_quotes);
std::string stringify(Expr* _e, bool _wrap_quotes=true);

enum THROWABLE_TYPES {
    THROWABLE_RETURNED,
    THROWABLE_THROWN,
};

class Throwable {
    char m_type = THROWABLE_THROWN;
    Expr* m_data{nullptr};
public:
    Throwable(char _type, Expr* _data);
    bool is_thrown(void);
    bool is_returned(void);
    Expr* data(void);
};

namespace core {

Expr* quote(VariableScope* _s, Expr* _e);
Expr* reverse_ip(VariableScope* _s, Expr* _e);
Expr* range(VariableScope* _s, Expr* _e);

Expr* car(VariableScope* _s, Expr* _e);
Expr* cdr(VariableScope* _s, Expr* _e);
Expr* cons(VariableScope* _s, Expr* _e);
Expr* is_nil(VariableScope* _s, Expr* _e);

Expr* plus2(VariableScope* _s, Expr* _e);
Expr* minus2(VariableScope* _s, Expr* _e);
Expr* multiply2(VariableScope* _s, Expr* _e);
Expr* divide2(VariableScope* _s, Expr* _e);

Expr* lessthan2(VariableScope* _s, Expr* _e);
Expr* greaterthan2(VariableScope* _s, Expr* _e);
Expr* mathequal(VariableScope* _s, Expr* _e);

Expr* slurp_file(VariableScope* _s, Expr* _e);
Expr* read(VariableScope* _s, Expr* _e);
Expr* eval(VariableScope* _s, Expr* _e);
Expr* write(VariableScope* _s, Expr* _e);
Expr* newline(VariableScope* _s, Expr* _e);
Expr* stringify(VariableScope* _s, Expr* _e);
Expr* concat2(VariableScope* _s, Expr* _e);

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
Expr* modulus(VariableScope* _s, Expr* _e);

Expr* get_time(VariableScope* _s, Expr* _e);

Expr* gc_info(VariableScope* _s, Expr* _e);
Expr* gc_run(VariableScope* _s, Expr* _e);

}; /*namespace core*/

class GarbageCollector {
  /*TODO:
    Currently just a dummy gc
    Needed features:
    - Page-based bulk allocation of exprs
    - a mark-and-sweep algorithm as the primary gc
  */
    std::vector<Expr*> m_in_use = {};
public:
    size_t exprs_in_use_count(void);
    Expr* new_expr(uint8_t _type);
    void destroy_expr(Expr* _e);
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
    bool add_global(const std::string& _name, Expr* _v);
	bool add_buildin(const std::string& _name, const BuildinFn _fn);
    bool add_local(const std::string& _name, Expr* _v);
    void bind(const std::string& _fnname, Expr* _binds, Expr* _values);
    
    Throwable NotImplemented(const std::string& _fn);
    Throwable InternalError(const std::string& _msg);
    Throwable ProgramError(const std::string& _fn, const std::string& _msg, Expr* _error);
    Throwable UserThrow(Expr* _thrown);
    Throwable UserReturn(Expr* _returned);

private:
    VariableScope* m_outer = nullptr;
    Environment* m_env;
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

    /*Expr Creation*/
	Expr* t(void);
	Expr* nil(void);
	Expr* cons(Expr *_car, Expr *_cdr);
    Expr* real(int _v);
    Expr* decimal(float _v);
    Expr* symbol(const std::string& _v);
    Expr* string(const std::string& _v);
    Expr* buildin(const BuildinFn _v);
	Expr* lambda(Expr* _binds, Expr* _body);
	Expr* macro(Expr* _binds, Expr* _body);
    Expr* list(const std::initializer_list<Expr *>& _lst);
    Expr* put_in_list(Expr* _val, Expr* _list);

    /*Environment Creation*/
    bool load_core(void);
    bool load_file(const std::string& _path);
    bool load(const std::string& _program);
    bool add_global(const std::string& _name, Expr* _v);
	bool add_buildin(const std::string& _name, const BuildinFn _fn);
    bool add_local(const std::string& _name, Expr* _v);

    const std::stringstream& outbuffer_put(const std::string& _put);
    std::string outbuffer_getreset();

private:
    std::stringstream m_outbuffer;
    GarbageCollector m_gc;
    VariableScope m_global_scope;
    bool m_core_loaded = false;
    Expr* lex_value(std::string& _token);
    Expr* lex(std::list<std::string>& _tokens);
};

#define YALCPP_IMPLEMENTATION
#ifdef YALCPP_IMPLEMENTATION
    
   
//bool Expr::is_real(void) {return type == TYPE_REAL;}
//bool Expr::is_decimal(void) {return type == TYPE_DECIMAL;}
//bool Expr::is_value(void) {return (is_real() || is_decimal());}
//bool Expr::is_string(void) {return type == TYPE_STRING;}
//bool Expr::is_symbol(void) {return type == TYPE_SYMBOL;}
//bool Expr::is_buildin(void) {return type == TYPE_BUILDIN;}
//bool Expr::is_lambda(void) {return type == TYPE_LAMBDA;}
//bool Expr::is_macro(void) {return type == TYPE_MACRO;}

std::ostream&
operator<<(std::ostream& os, Expr* _e)
{
    os << stream(_e, true).str();
    return os;
}
    

uint8_t
type(Expr* _e)
{
    if (_e == nullptr)
        return TYPE_INVALID;
    return _e->type;
}

bool
is_nil(Expr* _e)
{
    const std::string nil1 = "nil";
    const std::string nil2 = "NIL";
    if (_e == nullptr || type(_e) == TYPE_INVALID)
        return true;
    if (type(_e) == TYPE_CONS && _e->cons.car == nullptr && _e->cons.cdr == nullptr)
        return true;
    if (type(_e) == TYPE_SYMBOL && nil1 == _e->symbol)
        return true;
    if (type(_e) == TYPE_SYMBOL && nil2 == _e->symbol)
        return true;
    return false;
}

bool
is_real(Expr* _e)
{
    if (!is_nil(_e) && type(_e) == TYPE_REAL)
        return true;
    return false;
}

bool
is_decimal(Expr* _e)
{
    if (!is_nil(_e) && type(_e) == TYPE_DECIMAL)
        return true;
    return false;
}

bool
is_val(Expr* _e)
{
    if (_e == nullptr)
        return false;
    if (type(_e) == TYPE_REAL || type(_e) == TYPE_DECIMAL)
        return true;
    return false;
}

bool
is_symbol(Expr* _e)
{
    if (!is_nil(_e) && type(_e) == TYPE_SYMBOL)
        return true;
    return false;
}

bool
is_string(Expr* _e)
{
    if (!is_nil(_e) && type(_e) == TYPE_STRING)
        return true;
    return false;
}

bool
is_buildin(Expr* _e)
{
    if (!is_nil(_e) && type(_e) == TYPE_BUILDIN)
        return true;
    return false;
}

bool
is_lambda(Expr* _e)
{
    if (!is_nil(_e) && type(_e) == TYPE_LAMBDA)
        return true;
    return false;
}

bool
is_macro(Expr* _e)
{
    if (!is_nil(_e) && type(_e) == TYPE_MACRO)
        return true;
    return false;
}

bool
is_cons(Expr* _e)
{
    if (!is_nil(_e) && type(_e) == TYPE_CONS)
        return true;
    return false;
}

bool
is_dotted(Expr* _e)
{
    if (!is_nil(_e) && type(_e) == TYPE_CONS  &&
        !is_nil(car(_e)) && !is_cons(car(_e)) &&
        !is_nil(cdr(_e)) && !is_cons(cdr(_e)))
        return true;
    return false;
}

int
get_real(Expr* _e)
{
    assert(is_real(_e));
    return _e->real;
}

float
get_decimal(Expr* _e)
{
    assert(is_decimal(_e));
    return _e->decimal;
}

std::string
get_str(Expr* _e)
{
    assert(is_string(_e));
    return std::string(_e->string);
}

const char*
get_cstr(Expr* _e)
{
    assert(is_string(_e));
    return _e->string;
}

std::string
get_sym(Expr* _e)
{
    assert(is_symbol(_e));
    return std::string(_e->symbol);
}

const char*
get_csym(Expr* _e)
{
    assert(is_symbol(_e));
    return _e->symbol;
}

Expr*
get_binds(Expr* _e)
{
    if (!is_macro(_e) && !is_lambda(_e))
        return nullptr;
    return _e->callable.binds;
}

Expr*
get_body(Expr* _e)
{
    if (!is_macro(_e) && !is_lambda(_e))
        return nullptr;
    return _e->callable.body;
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

Expr*
set_car(Expr* _e, Expr* _car)
{
    assert(is_cons(_e));
    _e->cons.car = _car;
    return _e;
}

Expr*
set_cdr(Expr* _e, Expr* _cdr)
{
    assert(is_cons(_e));
    _e->cons.cdr = _cdr;
    return _e;
}
    
Expr*
set_callable(Expr* _e, Expr* _binds, Expr* _body)
{
    assert(is_lambda(_e) || is_macro(_e));
    _e->callable.binds = _binds;
    _e->callable.body = _body;
    return _e;
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

Expr* first(Expr* _e)
{ return car(_e); }
Expr* second(Expr* _e)
{ return car(cdr(_e)); }
Expr* third(Expr* _e)
{ return car(cdr(cdr(_e))); }
Expr* fourth(Expr* _e)
{ return car(cdr(cdr(cdr(_e)))); }

bool
is_marked(Expr* _e)
{
    if (!is_nil(_e))
        return _e->is_marked;
    return false;
}

void
set_mark(Expr* _e)
{
    if (is_nil(_e))
        return;
    if (is_lambda(_e) || is_macro(_e)) {
        set_mark(_e->callable.binds);
        set_mark(_e->callable.body);
    }
    if (is_cons(_e)) {
        set_mark(_e->cons.car);
        set_mark(_e->cons.cdr);
    }
    _e->is_marked = true;
}

void
clear_mark(Expr* _e)
{
    if (is_nil(_e))
        return;
    _e->is_marked = false;
}
    
Throwable::Throwable(char _type, Expr* _data) : m_type(_type), m_data(_data) {}

bool
Throwable::is_thrown(void)
{
    return m_type == THROWABLE_THROWN;
}

bool
Throwable::is_returned(void)
{
    return m_type == THROWABLE_RETURNED;
}

Expr*
Throwable::data(void)
{
    return m_data; 
}

Throwable
VariableScope::InternalError(const std::string& _msg)
{
    return Throwable(THROWABLE_THROWN,
                     env()->list({env()->string("[INTERNAL] " + _msg)}));
}

Throwable
VariableScope::NotImplemented(const std::string& _fn)
{
    return Throwable(THROWABLE_THROWN,
                     env()->list({env()->string("[" + _fn + "] is not implemented")}));
}

Throwable
VariableScope::ProgramError(const std::string& _fn, const std::string& _msg, Expr* _error) {
    Expr* msg = env()->cons(_error, nullptr);
    msg = env()->put_in_list(env()->string(_msg), msg);
    msg = env()->put_in_list(env()->string("[" + _fn + "]"), msg);
    return Throwable(THROWABLE_THROWN, msg);
}

Throwable
VariableScope::UserReturn(Expr* _returned)
{
    return Throwable(THROWABLE_RETURNED, _returned);
}

Throwable
VariableScope::UserThrow(Expr* _thrown)
{
    return Throwable(THROWABLE_THROWN, _thrown);
}

Expr*
GarbageCollector::new_expr(uint8_t _type)
{
    Expr* out = new Expr;
    out->type = _type;
    assert(out != nullptr && "Expr limit reached, could not create more Expr's.");
    m_in_use.push_back(out);
    return out;
}

void
GarbageCollector::destroy_expr(Expr* _e)
{
    if (_e == nullptr)
        return;
    switch(type(_e)) {
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
        destroy_expr(v);
}


size_t
GarbageCollector::exprs_in_use_count(void)
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
VariableScope::add_global(const std::string& _name, Expr* _v)
{
    return global_scope()->add_local(_name, _v);
}

bool
VariableScope::add_buildin(const std::string& _name, const BuildinFn _fn)
{
    return add_global(_name, env()->buildin(_fn));
}

void
VariableScope::bind(const std::string& _fnname, Expr* _binds, Expr* _values)
{
    /*TODO: add support for keyword binds aswell*/
    //std::cout << "==================== binding =============" << std::endl;
    //std::cout << "binding for " << _fnname << std::endl;
    //std::cout << "    binding    " << stringify(_binds) << std::endl;
    //std::cout << "    binding to " << stringify(_values) << std::endl;
    std::string bindstr;
    Expr* rest = nullptr;

    while (!is_nil(_binds)) {
        if (type(car(_binds)) != TYPE_SYMBOL)
            throw ProgramError("bind", "expected binds to be symbols, not", car(_binds));
        bindstr = get_sym(car(_binds));

        /*Handle potential & symbols that becomes the rest of values*/
        if (bindstr.size() > 1 && bindstr.find_first_of('&') == 0) {
            while (!is_nil(_values)) {
                rest = env()->put_in_list(car(_values), rest);
                _values = cdr(_values);
            }
            rest = ipreverse(rest);
            add_local(bindstr.c_str()+1, rest);
            _binds = cdr(_binds);
            _values = nullptr;
            break;
        }
        /*Handle potential & symbols that becomes the rest of values*/
        if (is_nil(car(_values)))
            add_local(get_csym(car(_binds)), env()->symbol("NIL"));
        else
            add_local(get_csym(car(_binds)), car(_values));
        _binds = cdr(_binds);
        _values = cdr(_values);
    }
    if (len(_values) > 0)
        throw ProgramError(_fnname, "too many inputs given! could not bind", _values);
}

VariableScope*
Environment::global_scope(void)
{
    return &m_global_scope;
}

size_t
Environment::exprs_in_use(void)
{
    return m_gc.exprs_in_use_count(); 
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
Environment::add_buildin(const std::string& _name, const BuildinFn _fn)
{
    return add_global(_name, buildin(_fn));
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
    auto try_trim = [&] () {
        while (_str[cursor] != '\0') {
            /*Remove comment until endline*/
            if (_str[cursor] == ';') {
              cursor++;
              while (_str[cursor] != '\0' && _str[cursor] != '\n')
                cursor++;
            }
            /*Remove whitespace character*/
            if (!is_whitespace(_str[cursor]))
              break;
            cursor++;
        }
    };
    while (_str[cursor] != '\0') {
        try_trim();
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
        if (e.is_thrown())
            return put_in_list(symbol("error"), e.data());
        if (e.is_returned())
            return e.data();
    }
    return res;
}

Expr*
_macro_expander(VariableScope* _s, const std::string& _macroname, Expr* _macrofn, Expr* _macroargs) {
    auto macro_expand = [] (auto macro_expand, VariableScope* scope, Expr* body) {
        if (is_nil(body))
            return body;
        if (type(body) == TYPE_SYMBOL) {
            Expr* s = first(scope->variable_get_this_scope(get_csym(body)));
            if (is_nil(s)) {
                return body;
            }
            s = scope->env()->list({scope->env()->symbol("quote"), s});
            return s;
        }
        if (type(body) == TYPE_CONS) {
            set_car(body, macro_expand(macro_expand, scope, car(body)));
            set_cdr(body, macro_expand(macro_expand, scope, cdr(body)));
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
                    if (e.is_thrown())
                        return s->env()->put_in_list(s->env()->symbol("error"), e.data());
                    if (e.is_returned())
                        return e.data();
                }
                e = cdr(e);
        }
        return last;
    };

    VariableScope internal = _s->create_internal();
    internal.bind(_macroname, get_binds(_macrofn), _macroargs);
    Expr* body_copy = _s->env()->read(stringify(get_body(_macrofn)));
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
                    if (e.is_thrown())
                        return put_in_list(symbol("error"), e.data());
                    if (e.is_returned())
                        return e.data();
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
    if (type(_e) == TYPE_REAL       ||
        type(_e) == TYPE_DECIMAL    ||
        type(_e) == TYPE_STRING     )
        return _e;

    /*Handle symbols*/
    if (type(_e) == TYPE_SYMBOL) {
        Expr* var = nullptr;
        if (is_nil(_e))
            return nil();
        if (_e->symbol == std::string("t") || _e->symbol == std::string("T"))
            return t();
        var = _scope->variable_get(_e->symbol);
        return first(var);
    }

    /*Handle function calls*/
    if (type(_e) == TYPE_CONS) {
        std::string name;
        Expr* fn = nullptr;
        Expr* args = nullptr;
        fn = car(_e);
        args = cdr(_e);
        if (type(fn) == TYPE_CONS)
            fn = scoped_eval(_scope, fn);
        if (type(fn) == TYPE_SYMBOL) {
            name = fn->symbol;
            fn = first(_scope->variable_get(fn->symbol));
        }
        else {
            name = "lambda";
        }
        if (is_nil(fn))
            throw _scope->ProgramError("eval", "could not evaluate unknown function", first(_e));

        if (type(fn) == TYPE_BUILDIN) {
            return fn->buildin(_scope, args);
        }
        if (type(fn) == TYPE_LAMBDA) {
            return eval_fn(name, fn, args);
        }
        if (type(fn) == TYPE_MACRO) {
            return scoped_eval(_scope, _macro_expander(_scope, name, fn, args));
        }
        throw _scope->ProgramError("eval", "could not find function called", fn);
    }
    throw _scope->ProgramError("eval", "Got something that could not be evaluated", _e);
    return nil();
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
    if (m_core_loaded)
        return true;
    try {
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
        add_buildin("cons", core::cons);
        add_buildin("range", core::range);
        add_buildin("reverse!", core::reverse_ip);
        add_buildin("car", core::car);
        add_buildin("cdr", core::cdr);
        add_buildin("_PLUS2", core::plus2);
        add_buildin("_MINUS2", core::minus2);
        add_buildin("_MULTIPLY2", core::multiply2);
        add_buildin("_DIVIDE2", core::divide2);
        add_buildin("=", core::mathequal);
        add_buildin("<", core::lessthan2);
        add_buildin(">", core::greaterthan2);
        add_buildin("nil?", core::is_nil);
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
        add_buildin("_CONCAT2", core::concat2);
        add_global("real-max", real(std::numeric_limits<int>::max()));
        add_global("real-min", real(std::numeric_limits<int>::min()));
        add_global("decimal-max", decimal(std::numeric_limits<float>::max()));
        add_global("decimal-min", decimal(std::numeric_limits<float>::min()));
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
        add_buildin("%", core::modulus);
        add_buildin("time", core::get_time);
        add_buildin("gc-info", core::gc_info);
        add_buildin("gc", core::gc_run);

        eval(read(" (fn! progn (&body)"
                  "   \"evaluate body and return last result\""
                  "   (if (nil? body)"
                  "     NIL"
                  "     (last body)))"));

    }
    catch (std::exception& e) {
        std::cout << "something wrong with buildins!" << std::endl
                  << e.what() << std::endl;
        return false;
    }
    m_core_loaded = true;
    return true;
}

bool
Environment::load(const std::string& _program)
{
    eval(read(_program));
    return true;
}

bool
Environment::load_file(const std::string& _path)
{
    std::string tmp;
    std::ifstream ifs;
    std::stringstream ss;
    ifs.open(_path);
    if (!ifs)
        return false;

    while (std::getline(ifs, tmp))
        ss << tmp << std::endl;
    ifs.close(); 
    eval(read(ss.str()));
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
Environment::cons(Expr *_car, Expr *_cdr) {
	Expr *out = m_gc.new_expr(TYPE_CONS);
	if (out == nullptr)
		return nullptr;
	out->cons.car = _car;
	out->cons.cdr = _cdr;
	return out;
}

Expr *
Environment::real(int _v) {
     Expr *out = m_gc.new_expr(TYPE_REAL);
     if (out == nullptr)
       return nullptr;
     out->real = _v;
     return out;
   }

Expr*
Environment::decimal(float _v) {
     Expr *out = m_gc.new_expr(TYPE_DECIMAL);
     if (out == nullptr)
       return nullptr;
     out->decimal = _v;
     return out;
   }

Expr*
Environment::symbol(const std::string& _v) {
     Expr *out = m_gc.new_expr(TYPE_SYMBOL);
     if (out == nullptr)
       return nullptr;
     out->symbol = str_to_cstr(_v);
     return out;
}

Expr *
Environment::string(const std::string& _v) {
  Expr *out = m_gc.new_expr(TYPE_STRING);
  if (out == nullptr)
    return nullptr;
  out->string = str_to_cstr(_v);
  return out;
}

Expr *
Environment::buildin(BuildinFn _v) {
  Expr *out = m_gc.new_expr(TYPE_BUILDIN);
  if (out == nullptr)
    return nullptr;
  out->buildin = _v;
  return out;
}

Expr*
Environment::lambda(Expr* _binds, Expr* _body) {
  Expr *out = m_gc.new_expr(TYPE_LAMBDA);
  if (out == nullptr)
    return nullptr;
  set_callable(out, _binds, _body);
  return out;
}

Expr*
Environment::macro(Expr* _binds, Expr* _body) 
{
  Expr *out = m_gc.new_expr(TYPE_MACRO);
  if (out == nullptr)
    return nullptr;
  set_callable(out, _binds, _body);
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
str_to_cstr(const std::string& _s) 
{
    char* out = new char[_s.size()+1];
    out[_s.size()] = '\0';
    std::memcpy(out, _s.c_str(), _s.size());
    return out;
}


std::stringstream
stream(Expr* _e, bool _wrap_quotes)
{
    auto stream_value = [_wrap_quotes] (Expr* e) {
        std::stringstream ss;
        if (is_nil(e)) {
            ss << "NIL";
            return ss;
        }
        switch (type(e)) {
        case TYPE_CONS:        ss << "(" << stream(e, _wrap_quotes).str() << ")"; break;
        case TYPE_LAMBDA:      ss << "#<lambda>";                   break;
        case TYPE_MACRO:       ss << "#<macro>";                    break;
        case TYPE_BUILDIN:     ss << "#<buildin>";                  break;
        case TYPE_REAL:        ss << get_real(e);                   break;
        case TYPE_DECIMAL:     ss << get_decimal(e);                break;
        case TYPE_SYMBOL:      ss << get_sym(e);                    break;
        case TYPE_STRING:      
            if (_wrap_quotes)
                ss << "\"" << get_str(e) << "\"";
            else
              ss << get_str(e);
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
    if (len(_e) > 1)
        throw _s->ProgramError("quote", "expected 1 argument, not", _e);
    return first(_e);
}

Expr*
core::cons(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(_e) != 2)
        throw _s->ProgramError("cons", "expected 2 arguments, got", args);
    return _s->env()->cons(first(args), second(args));
}

Expr*
core::car(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* out;
    if (len(_e) != 1)
        throw _s->ProgramError("car", "expected 1 argument, got", args);
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
        throw _s->ProgramError("cdr", "expected 1 argument, got", args);
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

    if (len(args) < 2)
        throw _s->ProgramError("+", "cannot do math without 2 values", args);
    if (!is_val(a))
        throw _s->ProgramError("+", "cannot do math on non-value", a);
    if (!is_val(b))
        throw _s->ProgramError("+", "cannot do math on non-value", b);

    if (type(a) == TYPE_DECIMAL && type(b) == TYPE_DECIMAL)
        return _s->env()->decimal(get_decimal(a) + get_decimal(b));
    else if (type(a) == TYPE_REAL && type(b) == TYPE_DECIMAL)
        return _s->env()->decimal(get_real(a) + get_decimal(b));
    else if (type(a) == TYPE_DECIMAL && type(b) == TYPE_REAL)
        return  _s->env()->decimal(get_decimal(a) + get_real(b));
    return _s->env()->real(get_real(a) + get_real(b));
}

Expr*
core::minus2(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* a = first(args);
    Expr* b = second(args);

    if (len(args) < 2)
        throw _s->ProgramError("-", "cannot do math without 2 values", args);
    if (!is_val(a))
        throw _s->ProgramError("-", "cannot do math on non-value", a);
    if (!is_val(b))
        throw _s->ProgramError("-", "cannot do math on non-value", b);

    if (type(a) == TYPE_DECIMAL && type(b) == TYPE_DECIMAL)
        return _s->env()->decimal(get_decimal(a) - get_decimal(b));
    else if (type(a) == TYPE_REAL && type(b) == TYPE_DECIMAL)
        return _s->env()->decimal(get_real(a) - get_decimal(b));
    else if (type(a) == TYPE_DECIMAL && type(b) == TYPE_REAL)
        return  _s->env()->decimal(get_decimal(a) - get_real(b));
    return _s->env()->real(get_real(a) - get_real(b));
}

Expr*
core::multiply2(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* a = first(args);
    Expr* b = second(args);
    if (len(args) < 2)
        throw _s->ProgramError("*", "cannot do math without 2 values", args);
    if (!is_val(a))
        throw _s->ProgramError("*", "cannot do math on non-value", a);
    if (!is_val(b))
        throw _s->ProgramError("*", "cannot do math on non-value", b);

    if (type(a) == TYPE_DECIMAL && type(b) == TYPE_DECIMAL)
        return _s->env()->decimal(get_decimal(a) * get_decimal(b));
    else if (type(a) == TYPE_REAL && type(b) == TYPE_DECIMAL)
        return _s->env()->decimal(get_real(a) * get_decimal(b));
    else if (type(a) == TYPE_DECIMAL && type(b) == TYPE_REAL)
        return  _s->env()->decimal(get_decimal(a) * get_real(b));
    return _s->env()->real(get_real(a) * get_real(b));
}

Expr*
core::divide2(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* a = first(args);
    Expr* b = second(args);

    if (len(args) < 2)
        throw _s->ProgramError("/", "cannot do math without 2 values", args);
    if (!is_val(a))
        throw _s->ProgramError("/", "cannot do math on non-value", a);
    if (!is_val(b))
        throw _s->ProgramError("/", "cannot do math on non-value", b);

     if ((type(a) == TYPE_DECIMAL && get_decimal(a) == 0) ||
        (type(a) == TYPE_REAL && get_real(a) == 0)        )
        return _s->env()->decimal(0);

    if ((type(b) == TYPE_DECIMAL && get_decimal(b) == 0) ||
        (type(b) == TYPE_REAL && get_real(b) == 0)        )
        throw _s->ProgramError("/", "divide by zero", args);

    if (type(a) == TYPE_DECIMAL && type(b) == TYPE_DECIMAL)
        return _s->env()->decimal(get_decimal(a) / get_decimal(b));
    else if (type(a) == TYPE_REAL && type(b) == TYPE_DECIMAL)
        return _s->env()->decimal(get_real(a) / get_decimal(b));
    else if (type(a) == TYPE_DECIMAL && type(b) == TYPE_REAL)
        return  _s->env()->decimal(get_decimal(a) / get_real(b));
    return _s->env()->decimal(get_real(a) / get_real(b));
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
        if (type(a) == TYPE_DECIMAL && type(b) == TYPE_DECIMAL)
            return get_decimal(a) == get_decimal(b);
        if (type(a) == TYPE_DECIMAL && type(b) == TYPE_DECIMAL)
            return get_decimal(a) == get_decimal(b);
        else if (type(a) == TYPE_REAL && type(b) == TYPE_DECIMAL)
            return get_real(a) == get_decimal(b);
        else if (type(a) == TYPE_DECIMAL && type(b) == TYPE_REAL)
            return  get_decimal(a) == get_real(b);
        else if (type(a) == TYPE_REAL && type(b) == TYPE_REAL)
            return get_real(a) == get_real(b);
        else if (type(a) == TYPE_SYMBOL && type(b) == TYPE_SYMBOL)
            return get_sym(a) == get_sym(b);
        else if (type(a) == TYPE_STRING && type(b) == TYPE_STRING)
            return get_str(a) == get_str(b);
        else if (type(a) == TYPE_CONS && type(b) == TYPE_CONS)
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

Expr*
core::lessthan2(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* a = first(args);
    Expr* b = second(args);

    if (len(args) < 2)
        return _s->env()->t();
    if (!is_val(a))
        throw _s->ProgramError("<", "cannot compare non-value", a);
    if (!is_val(b))
        throw _s->ProgramError("<", "cannot compare non-value", b);

    auto LT2 = [_s] (Expr* a, Expr* b) {
        if (type(a) == TYPE_DECIMAL && type(b) == TYPE_DECIMAL)
            return get_decimal(a) < get_decimal(b);
        else if (type(a) == TYPE_REAL && type(b) == TYPE_DECIMAL)
            return get_real(a) < get_decimal(b);
        else if (type(a) == TYPE_DECIMAL && type(b) == TYPE_REAL)
            return  get_decimal(a) < get_real(b);
        return get_real(a) < get_real(b);
    };
    if (!LT2(a, b))
        return _s->env()->nil();
    return _s->env()->t();
}

Expr*
core::greaterthan2(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* a = first(args);
    Expr* b = second(args);

    if (len(args) < 2)
        return _s->env()->t();
    if (!is_val(a))
        throw _s->ProgramError("<", "cannot compare non-value", a);
    if (!is_val(b))
        throw _s->ProgramError("<", "cannot compare non-value", b);
    auto GT2 = [_s] (Expr* a, Expr* b) {
        if (type(a) == TYPE_DECIMAL && type(b) == TYPE_DECIMAL)
            return get_decimal(a) > get_decimal(b);
        else if (type(a) == TYPE_REAL && type(b) == TYPE_DECIMAL)
            return get_real(a) > get_decimal(b);
        else if (type(a) == TYPE_DECIMAL && type(b) == TYPE_REAL)
            return  get_decimal(a) > get_real(b);
        return get_real(a) > get_real(b);
    };

    if (!GT2(a, b))
        return _s->env()->nil();
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
core::slurp_file(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    std::string content;
    std::ifstream ifs;
    std::stringstream ss;
    if (len(args) != 1)
        throw _s->ProgramError("slurp-file", "expects only 1 argument, got", args);
    if (type(first(args)) != TYPE_STRING)
        throw _s->ProgramError("slurp-file", "expects string as argument, got", first(args));
    ifs.open(get_str(first(args)));
    if (!ifs)
        throw _s->ProgramError("slurp-file", "could not open file", first(args));

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
        throw _s->ProgramError("read", "expects 1 argument, but did not get anything", _s->env()->nil());
    if (len(args) != 1)
        throw _s->ProgramError("read", "expects only 1 argument, got", args);
    if (type(first(args)) != TYPE_STRING)
        throw _s->ProgramError("read", "expects string as argument, not ", first(args));
    return _s->env()->read(get_str(first(args)));
}

Expr*
core::eval(VariableScope* _s, Expr* _e)
{
     Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1)
        throw _s->ProgramError("eval", "expects only 1 argument, got", args);
    return _s->env()->eval(first(args));
}

Expr*
core::write(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) > 1)
        throw _s->ProgramError("write", "expects single argument to write, not", args);
    _s->env()->outbuffer_put(stringify(first(args), false));
    return first(args);
}

Expr*
core::newline(VariableScope* _s, Expr* _e)
{
    if (len(_e) != 0)
        throw _s->ProgramError("newline", "expects no args, got", _e);
    _s->env()->outbuffer_put("\n");
    return _s->env()->nil();
}

Expr*
core::stringify(VariableScope* _s, Expr* _e)
{
    std::stringstream ss;
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1)
        throw _s->ProgramError("stringify", "expects only 1 argument, got", args);
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
    throw _s->ProgramError("concat", "can only concatenate up to 2 args, not", args);
   
}

Expr*
core::range(VariableScope* _s, Expr* _e)
{
    bool reverse = false;
    int low;
    int high;
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* out = nullptr;

    if (get_real(first(args)) < get_real(second(args))) {
        low = get_real(first(args));
        high = get_real(second(args));
    } else {
        low = get_real(second(args));
        high = get_real(first(args));
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
    if (!(len(_e) == 2 || len(_e) == 3))
        throw _s->ProgramError("if", "expected required condition and true body, got", _e);
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
        throw _s->ProgramError("typeof", "expected 1 argument, got", args);
    if (is_nil(first(args)))
        return nullptr;
    switch (type(first(args))) {
    case TYPE_CONS:        return _s->env()->symbol("cons");
    case TYPE_LAMBDA:      return _s->env()->symbol("lambda");
    case TYPE_MACRO:       return _s->env()->symbol("macro");
    case TYPE_BUILDIN:     return _s->env()->symbol("buildin");
    case TYPE_REAL:        return _s->env()->symbol("real");
    case TYPE_DECIMAL:     return _s->env()->symbol("decimal");
    case TYPE_SYMBOL:      return _s->env()->symbol("symbol");
    case TYPE_STRING:      return _s->env()->symbol("string");
    default:
        throw _s->ProgramError("typeof", "Could not determine type of input from", first(args));
    };
}

Expr*
core::_try(VariableScope* _s, Expr* _e)
{
    Expr* res = nullptr;
    Expr* err = nullptr;
    bool was_err = false;
    
    /*Check all requirements for catching is in place*/
    if (len(_e) != 3)
        throw _s->ProgramError("try", "expected 3 inputs, not", _e);
    if (type(second(_e)) != TYPE_SYMBOL)
        throw _s->ProgramError("try", "arg 2 expected symbol for error, not", second(_e));
    if (is_nil(third(_e)) || !is_cons(_e))
        throw _s->ProgramError("try",
                               "arg 3 expected expression for error evaluation, not",
                               third(_e));

    try {
        res = _s->env()->scoped_eval(_s, first(_e));
    } catch (Throwable& e) {
        if (e.is_thrown()) {
            err = _s->env()->put_in_list(_s->env()->symbol("error"), e.data());
            was_err = true;
        }
        if (e.is_returned()) {
            throw e;
        }
    }
    /*Return if no error occoured*/
    if (!was_err)
        return res;
    std::cout << "error resolution " << stringify(third(_e)) << std::endl;

    /*Manage error*/
    VariableScope internal = _s->create_internal();
    internal.add_local(get_csym(second(_e)), err);
    return _s->env()->scoped_eval(&internal, third(_e));
}


Expr*
core::_throw(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    throw _s->UserThrow(args);
    //throw _s->UserThrow(_s->env()->put_in_list(_s->env()->symbol("error"), args));
    return nullptr;
}

Expr*
core::_return(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) <= 1)
        throw _s->UserReturn(first(args));
    throw _s->ProgramError("return", "Return can only have 0-1 argument, got", args);
    return nullptr;
}

Expr*
core::variable_definition(VariableScope* _s, Expr* _e)
{
    Expr* var = nullptr;
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1)
        throw _s->ProgramError("variable-definition", "expects single arg, but got", args);
    if (type(first(args)) != TYPE_SYMBOL)
        throw _s->ProgramError("variable-definition", "expects symbol variable, but got", first(args));
    
    var = _s->variable_get(get_csym(first(args)));
    if (is_nil(var))
        return _s->env()->nil();
    return var;
}

Expr*
core::defglobal(VariableScope* _s, Expr* _e)
{
    Expr* val = _s->env()->scoped_eval(_s, second(_e));
    if (len(_e) != 2)
        throw _s->ProgramError("global!", "expects symbol and value, got", _e);
    if (type(first(_e)) != TYPE_SYMBOL)
        throw _s->ProgramError("global!", "expects name to be type symbol, got", first(_e));
    _s->add_global(get_csym(first(_e)), val);
    return first(_e);
}

Expr*
core::deflocal(VariableScope* _s, Expr* _e)
{
    Expr* val = _s->env()->scoped_eval(_s, second(_e));
    if (len(_e) != 2)
        throw _s->ProgramError("local!", "expects symbol and value, got", _e);
    if (type(first(_e)) != TYPE_SYMBOL)
        throw _s->ProgramError("local!", "expects name to be type symbol, got", first(_e));
    _s->add_local(get_csym(first(_e)), val);
    return first(_e);
}

Expr*
core::setcar(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (type(first(args)) != TYPE_CONS)
        throw _s->ProgramError("setcar!", "Expected cons to modify, got", first(args));
    set_car(first(args), second(args));
    return second(args);
}

Expr*
core::setcdr(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (type(first(args)) != TYPE_CONS)
        throw _s->ProgramError("setcdr!", "Expected cons to modify, got", first(args));
    set_cdr(first(args), second(args));
    return second(args);
}

Expr*
core::deflambda(VariableScope* _s, Expr* _e)
{
    UNUSED(_s);
    if (len(_e) < 1)
        throw _s->ProgramError("lambda", "expected arguments to be binds and body, got", _e);
    Expr* l = _s->env()->lambda(car(_e), cdr(_e));
    return l;
}

Expr*
core::deffunction(VariableScope* _s, Expr* _e)
{
    if (len(_e) < 2)
        throw _s->ProgramError("fn!", "expected arguments to be name, binds and body, got", _e);
    Expr* fn = _s->env()->lambda(second(_e), cdr(cdr(_e)));
    _s->add_global(get_csym(first(_e)), fn);
    return first(_e);
}

Expr*
core::defmacro(VariableScope* _s, Expr* _e)
{
    if (len(_e) < 2)
        throw _s->ProgramError("macro!", "expected arguments to be name, binds and body, got", _e);
    Expr* macro = _s->env()->macro(second(_e), cdr(cdr(_e)));
    _s->add_global(get_csym(first(_e)), macro);
    return first(_e);
}

Expr*
core::macro_expand(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || type(first(args)) != TYPE_CONS)
        throw _s->ProgramError("macro-expand", "expected macro call as list, got", args);
    Expr* macro_call = first(args);
    if (type(first(macro_call)) != TYPE_SYMBOL)
        throw _s->ProgramError("macro-expand",
                           "expected first arg to be a symbol pointing to a macro, not",
                           first(macro_call));

    const std::string macro_name = get_sym(first(macro_call));
    Expr* macro_fn = first(_s->variable_get(get_csym(first(macro_call))));
    Expr* macro_args = cdr(macro_call);
    Expr* expanded = _macro_expander(_s, macro_name, macro_fn, macro_args);
    return expanded;
}

Expr*
core::cos(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("cos", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::cos(get_decimal(first(args))));
    return _s->env()->decimal(std::cos(get_real(first(args))));
}

Expr*
core::sin(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("sin", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::sin(get_decimal(first(args))));
    return _s->env()->decimal(std::sin(get_real(first(args))));
}

Expr*
core::tan(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("tan", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::tan(get_decimal(first(args))));
    return _s->env()->decimal(std::tan(get_real(first(args))));
}

Expr*
core::acos(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("acos", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::acos(get_decimal(first(args))));
    return _s->env()->decimal(std::acos(get_real(first(args))));
}
    
Expr*
core::asin(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("asin", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::asin(get_decimal(first(args))));
    return _s->env()->decimal(std::asin(get_real(first(args))));
}
    
Expr*
core::atan(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("atan", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::atan(get_decimal(first(args))));
    return _s->env()->decimal(std::atan(get_real(first(args))));
}
    
Expr*
core::cosh(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("cosh", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::cosh(get_decimal(first(args))));
    return _s->env()->decimal(std::cosh(get_real(first(args))));
}
    
Expr*
core::sinh(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("sinh", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::sinh(get_decimal(first(args))));
    return _s->env()->decimal(std::sinh(get_real(first(args))));
}
    
Expr*
core::tanh(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("tanh", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::tanh(get_decimal(first(args))));
    return _s->env()->decimal(std::tanh(get_real(first(args))));
}
    
Expr*
core::floor(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("floor", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->real(std::floor(get_decimal(first(args))));
    return _s->env()->real(std::floor(get_real(first(args))));
}
    
Expr*
core::ceil(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("ceil", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->real(std::ceil(get_decimal(first(args))));
    return _s->env()->real(std::ceil(get_real(first(args))));
}

Expr*
core::sqrt(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("sqrt", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::sqrt(get_decimal(first(args))));
    return _s->env()->decimal(std::sqrt(get_real(first(args))));
}
    
Expr*
core::log(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("log", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::log(get_decimal(first(args))));
    return _s->env()->decimal(std::log(get_real(first(args))));
}
    
Expr*
core::log10(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("log10", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::log10(get_decimal(first(args))));
    return _s->env()->decimal(std::log10(get_real(first(args))));
}

Expr*
core::exp(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("exp", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::exp(get_decimal(first(args))));
    return _s->env()->decimal(std::exp(get_real(first(args))));
}

Expr*
core::pow(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* a = first(args);
    Expr* b = second(args);
    if (len(args) != 2 || !is_val(a) || !is_val(b))
        throw _s->ProgramError("pow", "expected 2 value args, got", args);
    if (type(a) == TYPE_DECIMAL && type(b) == TYPE_DECIMAL)
        return _s->env()->decimal(std::pow(get_decimal(a), get_decimal(b)));
    if (type(a) == TYPE_REAL && type(b) == TYPE_DECIMAL)
        return _s->env()->decimal(std::pow(get_real(a), get_decimal(b)));
    if (type(a) == TYPE_DECIMAL && type(b) == TYPE_REAL)
        return _s->env()->decimal(std::pow(get_decimal(a), get_real(b)));
    return _s->env()->decimal(std::pow(get_real(a), get_real(b)));
}

Expr*
core::modulus(VariableScope* _s, Expr* _e)
{
    Expr* args = _s->env()->list_eval(_s, _e);
    Expr* a = first(args);
    Expr* b = second(args);
    if (len(args) != 2 || !is_val(a) || !is_val(b))
        throw _s->ProgramError("%", "expected 2 value args, got", args);
    if (type(a) == TYPE_DECIMAL && type(b) == TYPE_DECIMAL)
        return _s->env()->decimal(std::fmod(get_decimal(a), get_decimal(b)));
    if (type(a) == TYPE_REAL && type(b) == TYPE_DECIMAL)
        return _s->env()->decimal(std::fmod(get_real(a), get_decimal(b)));
    if (type(a) == TYPE_DECIMAL && type(b) == TYPE_REAL)
        return _s->env()->decimal(std::fmod(get_decimal(a), get_real(b)));
    return _s->env()->decimal(std::fmod(get_real(a), get_real(b)));
}

Expr*
core::get_time(VariableScope *_s, Expr *_e)
{
  if (len(_e) != 0)
      throw _s->ProgramError("time", "expects no arguments, got", _e);

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

Expr*
core::gc_info(VariableScope* _s, Expr* _e)
{
    if (len(_e) != 0)
        throw _s->ProgramError("gc-info", "expects no arguments, got", _e);
    return _s->env()->string(_s->env()->gc_info());
}

Expr*
core::gc_run(VariableScope* _s, Expr* _e)
{
    UNUSED(_s);
    UNUSED(_e);
    return nullptr;
}

#endif /*YALPP_IMPLEMENTATION*/
#endif /*YALCPP_H*/

}; /*namespace yal*/
