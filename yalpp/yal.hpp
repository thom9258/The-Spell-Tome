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
#include <variant>
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
class ExprData;
using ExprPtr = std::shared_ptr<ExprData>;
class VariableScope;
class Environment;
typedef ExprPtr(*BuildinFn)(VariableScope*, ExprPtr);

class ExprData {
    bool is_marked = false;
    uint8_t type = TYPE_INVALID;
//   union {
//       int real;
//       float decimal;
//       char* string;
//       char* symbol;
//       BuildinFn buildin;
//       struct {
//           ExprPtr binds;
//           ExprPtr body;
//       }callable;
//       struct {
//           ExprPtr car;
//           ExprPtr cdr;
//       }cons;
//   };

       struct cons {
           ExprPtr car;
           ExprPtr cdr;
       };

    std::variant < int, float, char*, cons, BuildinFn
                   > data;


public:
    /*'friends' are the only interaction interface with the ExprPtr class.
      This was chosen to avoid null pointer access, as a nullptr is considered
      a valid 'ExprPtr'*/
    friend class GarbageCollector;
    /*TODO: this is a temporary workaround, and should be excluded*/
    friend class Environment;

    friend std::ostream& operator<<(std::ostream& os, ExprPtr _e);
    /*type specifiers*/
    friend uint8_t type(ExprPtr _e);
    friend bool is_nil(ExprPtr _e);
    friend bool is_real(ExprPtr _e);
    friend bool is_decimal(ExprPtr _e);
    friend bool is_val(ExprPtr _e);
    friend bool is_string(ExprPtr _e);
    friend bool is_symbol(ExprPtr _e);
    friend bool is_buildin(ExprPtr _e);
    friend bool is_lambda(ExprPtr _e);
    friend bool is_macro(ExprPtr _e);
    friend bool is_cons(ExprPtr _e);
    friend bool is_dotted(ExprPtr _e);

    /*garbage management methods*/
    friend bool is_marked(ExprPtr _e);
    friend void set_mark(ExprPtr _e);
    friend void clear_mark(ExprPtr _e);
    
    /*value accessors*/
    friend int         get_real(ExprPtr _e);
    friend float       get_decimal(ExprPtr _e);
    friend std::string get_str(ExprPtr _e);
    friend const char* get_cstr(ExprPtr _e);
    friend std::string get_sym(ExprPtr _e);
    friend const char* get_csym(ExprPtr _e);
    friend ExprPtr     get_binds(ExprPtr _e);
    friend ExprPtr     get_body(ExprPtr _e);

    friend ExprPtr car(ExprPtr _e);
    friend ExprPtr cdr(ExprPtr _e);
    friend ExprPtr set_callable(ExprPtr _e, ExprPtr _binds, ExprPtr _body);
    friend ExprPtr set_car(ExprPtr _e, ExprPtr _car);
    friend ExprPtr set_cdr(ExprPtr _e, ExprPtr _cdr);
    friend ExprPtr ipreverse(ExprPtr _e);

    /*friend functions that extend existing friend functions*/
    friend int len(ExprPtr _e);
    friend ExprPtr first(ExprPtr _e); 
    friend ExprPtr second(ExprPtr _e);
    friend ExprPtr third(ExprPtr _e);
    friend ExprPtr fourth(ExprPtr _e);
};
    
/*Helper that convert std::string to allocated char ptr*/
char* str_to_cstr(const std::string& _s);

/*Stringification helpers*/
std::stringstream stream(ExprPtr _e, bool _wrap_quotes);
std::string stringify(ExprPtr _e, bool _wrap_quotes=true);

enum THROWABLE_TYPES {
    THROWABLE_RETURNED,
    THROWABLE_THROWN,
};

class Throwable {
    char m_type = THROWABLE_THROWN;
    ExprPtr m_data{nullptr};
public:
    Throwable(char _type, ExprPtr _data);
    bool is_thrown(void);
    bool is_returned(void);
    ExprPtr data(void);
};

namespace core {

ExprPtr quote(VariableScope* _s, ExprPtr _e);
ExprPtr reverse_ip(VariableScope* _s, ExprPtr _e);
ExprPtr range(VariableScope* _s, ExprPtr _e);

ExprPtr car(VariableScope* _s, ExprPtr _e);
ExprPtr cdr(VariableScope* _s, ExprPtr _e);
ExprPtr cons(VariableScope* _s, ExprPtr _e);
ExprPtr is_nil(VariableScope* _s, ExprPtr _e);

ExprPtr plus2(VariableScope* _s, ExprPtr _e);
ExprPtr minus2(VariableScope* _s, ExprPtr _e);
ExprPtr multiply2(VariableScope* _s, ExprPtr _e);
ExprPtr divide2(VariableScope* _s, ExprPtr _e);

ExprPtr lessthan2(VariableScope* _s, ExprPtr _e);
ExprPtr greaterthan2(VariableScope* _s, ExprPtr _e);
ExprPtr mathequal(VariableScope* _s, ExprPtr _e);

ExprPtr slurp_file(VariableScope* _s, ExprPtr _e);
ExprPtr read(VariableScope* _s, ExprPtr _e);
ExprPtr eval(VariableScope* _s, ExprPtr _e);
ExprPtr write(VariableScope* _s, ExprPtr _e);
ExprPtr newline(VariableScope* _s, ExprPtr _e);
ExprPtr stringify(VariableScope* _s, ExprPtr _e);
ExprPtr concat2(VariableScope* _s, ExprPtr _e);

ExprPtr defglobal(VariableScope* _s, ExprPtr _e);
ExprPtr deflocal(VariableScope* _s, ExprPtr _e);
ExprPtr deflambda(VariableScope* _s, ExprPtr _e);
/*TODO: make deffunction into a macro that creates a lambda*/
ExprPtr deffunction(VariableScope* _s, ExprPtr _e);
ExprPtr defmacro(VariableScope* _s, ExprPtr _e);
ExprPtr macro_expand(VariableScope* _s, ExprPtr _e);
ExprPtr setcar(VariableScope* _s, ExprPtr _e);
ExprPtr setcdr(VariableScope* _s, ExprPtr _e);
ExprPtr variable_definition(VariableScope* _s, ExprPtr _e);

ExprPtr _try(VariableScope* _s, ExprPtr _e);
ExprPtr _throw(VariableScope* _s, ExprPtr _e);
ExprPtr _return(VariableScope* _s, ExprPtr _e);

ExprPtr _if(VariableScope* _s, ExprPtr _e);
ExprPtr _typeof(VariableScope* _s, ExprPtr _e);

/*NOTE: A lot of these can be calculated using the above buildins, but are
        pulled from c++ stl for convenience.
*/
ExprPtr cos(VariableScope* _s, ExprPtr _e);
ExprPtr sin(VariableScope* _s, ExprPtr _e);
ExprPtr tan(VariableScope* _s, ExprPtr _e);
ExprPtr acos(VariableScope* _s, ExprPtr _e);
ExprPtr asin(VariableScope* _s, ExprPtr _e);
ExprPtr atan(VariableScope* _s, ExprPtr _e);
ExprPtr cosh(VariableScope* _s, ExprPtr _e);
ExprPtr sinh(VariableScope* _s, ExprPtr _e);
ExprPtr tanh(VariableScope* _s, ExprPtr _e);

ExprPtr floor(VariableScope* _s, ExprPtr _e);
ExprPtr ceil(VariableScope* _s, ExprPtr _e);

ExprPtr pow(VariableScope* _s, ExprPtr _e);
ExprPtr sqrt(VariableScope* _s, ExprPtr _e);
ExprPtr log(VariableScope* _s, ExprPtr _e);
ExprPtr log10(VariableScope* _s, ExprPtr _e);
ExprPtr exp(VariableScope* _s, ExprPtr _e);
ExprPtr modulus(VariableScope* _s, ExprPtr _e);

ExprPtr get_time(VariableScope* _s, ExprPtr _e);

ExprPtr gc_info(VariableScope* _s, ExprPtr _e);
ExprPtr gc_run(VariableScope* _s, ExprPtr _e);
    
ExprPtr get_vars(VariableScope* _s, ExprPtr _e);

}; /*namespace core*/

class GarbageCollector {
  /*TODO:
    Currently just a dummy gc
    Needed features:
    - Page-based bulk allocation of exprs
    - a mark-and-sweep algorithm as the primary gc
  */
    std::vector<ExprPtr> m_in_use = {};
    std::size_t m_total = 0;
public:
    size_t exprs_total_count(void);
    size_t exprs_in_use_count(void);
    size_t garbage_collect(void);
    ExprPtr new_expr(uint8_t _type);
    void destroy_expr(ExprPtr _e);
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
    VariableScope* outer_scope(void);
    VariableScope create_internal(void);
    bool is_var_const(ExprPtr _var);
    ExprPtr variable_get(const std::string& _name);
    ExprPtr variable_get_this_scope(const std::string& _name);
    std::unordered_map<std::string, ExprPtr>& variables_get_all(void);
    void mark_variables(void);
    bool add_global(const std::string& _name, ExprPtr _v);
	bool add_buildin(const std::string& _name, const BuildinFn _fn);
    bool add_local(const std::string& _name, ExprPtr _v);
    void bind(const std::string& _fnname, ExprPtr _binds, ExprPtr _values);
    
    Throwable NotImplemented(const std::string& _fn);
    Throwable InternalError(const std::string& _msg);
    Throwable ProgramError(const std::string& _fn, const std::string& _msg, ExprPtr _error);
    Throwable UserThrow(ExprPtr _thrown);
    Throwable UserReturn(ExprPtr _returned);

private:
    VariableScope* m_outer = nullptr;
    Environment* m_env;
    std::unordered_map<std::string, ExprPtr> m_variables;
};

class Environment {
    friend class VariableScope;
    /*garbage management methods*/
    friend bool is_marked(ExprPtr _e);
    friend void set_mark(ExprPtr _e);
    friend void clear_mark(ExprPtr _e);

public:
    Environment(void);
    VariableScope* global_scope(void);

    /*Info*/
    size_t garbage_collect(void);
    size_t exprs_in_use(void);
    size_t exprs_total(void);
    const std::string gc_info(void);

    /*Evaluation*/
    ExprPtr read(const std::string& _program);
    ExprPtr eval(ExprPtr _e);
    /*TODO: put these into private and access as friend*/
    ExprPtr list_eval(VariableScope* _scope, ExprPtr _list);
    ExprPtr scoped_eval(VariableScope* _scope, ExprPtr _e);

    /*ExprPtr Creation*/
	ExprPtr t(void);
	ExprPtr nil(void);
	ExprPtr cons(ExprPtr _car, ExprPtr _cdr);
    ExprPtr real(int _v);
    ExprPtr decimal(float _v);
    ExprPtr symbol(const std::string& _v);
    ExprPtr string(const std::string& _v);
    ExprPtr buildin(const BuildinFn _v);
	ExprPtr lambda(ExprPtr _binds, ExprPtr _body);
	ExprPtr macro(ExprPtr _binds, ExprPtr _body);
    ExprPtr list(const std::initializer_list<ExprPtr >& _lst);
    ExprPtr put_in_list(ExprPtr _val, ExprPtr _list);

    /*Environment Creation*/
    bool load_core(void);
    bool load_file(const std::string& _path);
    bool load(const std::string& _program);
    bool add_global(const std::string& _name, ExprPtr _v);
	bool add_buildin(const std::string& _name, const BuildinFn _fn);
    bool add_local(const std::string& _name, ExprPtr _v);

    const std::stringstream& outbuffer_put(const std::string& _put);
    std::string outbuffer_getreset();

private:
    std::stringstream m_outbuffer;
    GarbageCollector m_gc;
    ExprPtr m_glob_nil = nullptr;
    ExprPtr m_glob_t = nullptr;
    VariableScope m_global_scope;
    bool m_core_loaded = false;
    ExprPtr lex_value(std::string& _token);
    ExprPtr lex(std::list<std::string>& _tokens);
};

#define YALCPP_IMPLEMENTATION
#ifdef YALCPP_IMPLEMENTATION
    
   
//bool ExprPtr::is_real(void) {return type == TYPE_REAL;}
//bool ExprPtr::is_decimal(void) {return type == TYPE_DECIMAL;}
//bool ExprPtr::is_value(void) {return (is_real() || is_decimal());}
//bool ExprPtr::is_string(void) {return type == TYPE_STRING;}
//bool ExprPtr::is_symbol(void) {return type == TYPE_SYMBOL;}
//bool ExprPtr::is_buildin(void) {return type == TYPE_BUILDIN;}
//bool ExprPtr::is_lambda(void) {return type == TYPE_LAMBDA;}
//bool ExprPtr::is_macro(void) {return type == TYPE_MACRO;}

std::ostream&
operator<<(std::ostream& os, ExprPtr _e)
{
    os << stream(_e, true).str();
    return os;
}
    

uint8_t
type(ExprPtr _e)
{
    if (_e == nullptr)
        return TYPE_INVALID;
    return _e->type;
}

bool
is_nil(ExprPtr _e)
{
    const std::string nil1 = "nil";
    const std::string nil2 = "NIL";
    if (_e == nullptr || type(_e) == TYPE_INVALID)
        return true;
    if (type(_e) == TYPE_CONS && car(_e) == nullptr && cdr(_e) == nullptr)
        return true;
    if (type(_e) == TYPE_SYMBOL && nil1 == get_sym(_e))
        return true;
    if (type(_e) == TYPE_SYMBOL && nil2 == get_sym(_e))
        return true;
    return false;
}

bool
is_real(ExprPtr _e)
{
    if (!is_nil(_e) && type(_e) == TYPE_REAL)
        return true;
    return false;
}

bool
is_decimal(ExprPtr _e)
{
    if (!is_nil(_e) && type(_e) == TYPE_DECIMAL)
        return true;
    return false;
}

bool
is_val(ExprPtr _e)
{
    if (_e == nullptr)
        return false;
    if (type(_e) == TYPE_REAL || type(_e) == TYPE_DECIMAL)
        return true;
    return false;
}

bool
is_symbol(ExprPtr _e)
{
    if (!is_nil(_e) && type(_e) == TYPE_SYMBOL)
        return true;
    return false;
}

bool
is_string(ExprPtr _e)
{
    if (!is_nil(_e) && type(_e) == TYPE_STRING)
        return true;
    return false;
}

bool
is_buildin(ExprPtr _e)
{
    if (!is_nil(_e) && type(_e) == TYPE_BUILDIN)
        return true;
    return false;
}

bool
is_lambda(ExprPtr _e)
{
    if (!is_nil(_e) && type(_e) == TYPE_LAMBDA)
        return true;
    return false;
}

bool
is_macro(ExprPtr _e)
{
    if (!is_nil(_e) && type(_e) == TYPE_MACRO)
        return true;
    return false;
}

bool
is_cons(ExprPtr _e)
{
    if (!is_nil(_e) && type(_e) == TYPE_CONS)
        return true;
    return false;
}

bool
is_dotted(ExprPtr _e)
{
    if (!is_nil(_e) && type(_e) == TYPE_CONS  &&
        !is_nil(car(_e)) && !is_cons(car(_e)) &&
        !is_nil(cdr(_e)) && !is_cons(cdr(_e)))
        return true;
    return false;
}

int
get_real(ExprPtr _e)
{
    assert(is_real(_e));
    return std::get<0>(_e->data);
}

float
get_decimal(ExprPtr _e)
{
    assert(is_decimal(_e));
    return std::get<1>(_e->data);
}

std::string
get_str(ExprPtr _e)
{
    assert(is_string(_e));
    return std::string(std::get<2>(_e->data));
}

const char*
get_cstr(ExprPtr _e)
{
    assert(is_string(_e));
    return std::get<2>(_e->data);
}

std::string
get_sym(ExprPtr _e)
{
    assert(is_symbol(_e));
    return std::string(std::get<2>(_e->data));
}

const char*
get_csym(ExprPtr _e)
{
    assert(is_symbol(_e));
    return std::get<2>(_e->data);
}

ExprPtr
get_binds(ExprPtr _e)
{
    if (!is_macro(_e) && !is_lambda(_e))
        return nullptr;
    return std::get<3>(_e->data).car;
}

ExprPtr
get_body(ExprPtr _e)
{
    if (!is_macro(_e) && !is_lambda(_e))
        return nullptr;
    return std::get<3>(_e->data).cdr;
}

ExprPtr
ipreverse(ExprPtr _list)
{
    ExprPtr curr = _list;
    ExprPtr next = nullptr;
    ExprPtr prev = nullptr;
    while (!is_nil(curr)) {
        next = cdr(curr);
        set_cdr(curr, prev);
        prev = curr;
        curr = next;
    }
    return prev;
}

ExprPtr
car(ExprPtr _e)
{
    if (is_nil(_e) || !is_cons(_e))
        return nullptr;
    return std::get<3>(_e->data).car;
}

ExprPtr
cdr(ExprPtr _e)
{
    if (is_nil(_e) || !is_cons(_e))
        return nullptr;
    return std::get<3>(_e->data).cdr;
}

ExprPtr
set_car(ExprPtr _e, ExprPtr _car)
{
    assert(is_cons(_e));
    _e->cons.car = _car;
    return _e;
}

ExprPtr
set_cdr(ExprPtr _e, ExprPtr _cdr)
{
    assert(is_cons(_e));
    _e->cons.cdr = _cdr;
    return _e;
}
    
ExprPtr
set_callable(ExprPtr _e, ExprPtr _binds, ExprPtr _body)
{
    assert(is_lambda(_e) || is_macro(_e));
    _e->callable.binds = _binds;
    _e->callable.body = _body;
    return _e;
}

int
len(ExprPtr _e)
{
    if (is_nil(_e)) return 0;
    if (!is_cons(_e)) return 1;
    int cnt = 0;
    ExprPtr tmp = _e;
    while (!is_nil(tmp)) {
        cnt++;
        tmp = cdr(tmp);
    }
    return cnt;
}

ExprPtr first(ExprPtr _e)
{ return car(_e); }
ExprPtr second(ExprPtr _e)
{ return car(cdr(_e)); }
ExprPtr third(ExprPtr _e)
{ return car(cdr(cdr(_e))); }
ExprPtr fourth(ExprPtr _e)
{ return car(cdr(cdr(cdr(_e)))); }

bool
is_marked(ExprPtr _e)
{
    if (_e == nullptr)
        return false;
    return _e->is_marked;
}

void
set_mark(ExprPtr _e)
{
    if (_e == nullptr)
        return;
    _e->is_marked = true;

    //if (!is_lambda(_e) && !is_macro(_e) && !is_cons(_e)) std::cout << "marked " << _e << std::endl;
    if (is_lambda(_e) || is_macro(_e)) {
        //std::cout << "marked callable" << std::endl;
        if (!is_marked(_e->callable.binds))
            set_mark(_e->callable.binds);
        if (!is_marked(_e->callable.body))
        set_mark(_e->callable.body);
    }
    if (is_cons(_e)) {
        //std::cout << "marked cons " << _e << std::endl;
        if (!is_marked(_e->cons.car))
            set_mark(_e->cons.car);
        if (!is_marked(_e->cons.cdr))
            set_mark(_e->cons.cdr);
    }
}

void
clear_mark(ExprPtr _e)
{
    if (_e == nullptr)
        return;
    _e->is_marked = false;
}
    
Throwable::Throwable(char _type, ExprPtr _data) : m_type(_type), m_data(_data) {}

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

ExprPtr
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
VariableScope::ProgramError(const std::string& _fn, const std::string& _msg, ExprPtr _error) {
    ExprPtr msg = env()->cons(_error, nullptr);
    msg = env()->put_in_list(env()->string(_msg), msg);
    msg = env()->put_in_list(env()->string("[" + _fn + "]"), msg);
    return Throwable(THROWABLE_THROWN, msg);
}

Throwable
VariableScope::UserReturn(ExprPtr _returned)
{
    return Throwable(THROWABLE_RETURNED, _returned);
}

Throwable
VariableScope::UserThrow(ExprPtr _thrown)
{
    return Throwable(THROWABLE_THROWN, _thrown);
}

ExprPtr
GarbageCollector::new_expr(uint8_t _type)
{
    ExprPtr out = std::make_shared<ExprData>();
    out->type = _type;
    assert(out != nullptr && "ExprPtr limit reached, could not create more ExprPtr's.");
    m_in_use.push_back(out);
    m_total++;
    return out;
}

void
GarbageCollector::destroy_expr(ExprPtr _e)
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
    /*not allowed*/
    //delete _e;
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

size_t 
GarbageCollector::garbage_collect(void)
{
    size_t removed = 0;
    for (auto it = m_in_use.begin(); it != m_in_use.end(); it++) {
        ExprPtr expr = *it;
        if (expr == nullptr) {
            continue;
        }
        if (is_marked(expr)) {
            //std::cout << "unmarkeded " << expr << std::endl;
            clear_mark(expr);
        } else {
            //std::cout << "destroyed " << expr << std::endl;
            destroy_expr(expr);
            it = m_in_use.erase(it);
            removed++;
        }
    }
    return removed;
}

size_t
GarbageCollector::exprs_total_count(void)
{
    return m_total; 
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

VariableScope* 
VariableScope::outer_scope(void)
{
    return m_outer;
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

ExprPtr
VariableScope::variable_get(const std::string& _s)
{
    ExprPtr v = nullptr;
    VariableScope* scope = this;
    while (scope != nullptr) {
        v = scope->variable_get_this_scope(_s);
        if (!is_nil(v))
            return v;
        scope = scope->m_outer;
    }
    return nullptr;
}


std::unordered_map<std::string, ExprPtr>&
VariableScope::variables_get_all(void)
{
    return m_variables;
}

void
VariableScope::mark_variables(void)
{
    for (auto kv: m_variables) {
        //std::cout << "vc marked " << kv.first << "  " << kv.second << std::endl;
        set_mark(kv.second);
}
    if (m_outer != nullptr)
        m_outer->mark_variables();
}

ExprPtr
VariableScope::variable_get_this_scope(const std::string& _s)
{
    if (m_variables.find(_s) == m_variables.end())
        return nullptr;
    return m_variables[_s];
}

bool
VariableScope::add_local(const std::string& _name, ExprPtr _v)
{
    ExprPtr entry;
    if (!is_nil(variable_get_this_scope(_name)))
        return false;
    entry = env()->list({_v});
    m_variables.insert({_name, entry});
    return true;
}

bool
VariableScope::add_global(const std::string& _name, ExprPtr _v)
{
    return global_scope()->add_local(_name, _v);
}

bool
VariableScope::add_buildin(const std::string& _name, const BuildinFn _fn)
{
    return add_global(_name, env()->buildin(_fn));
}

void
VariableScope::bind(const std::string& _fnname, ExprPtr _binds, ExprPtr _values)
{
    /*TODO: add support for keyword binds aswell*/
    //std::cout << "==================== binding =============" << std::endl;
    //std::cout << "binding for " << _fnname << std::endl;
    //std::cout << "    binding    " << stringify(_binds) << std::endl;
    //std::cout << "    binding to " << stringify(_values) << std::endl;
    std::string bindstr;
    ExprPtr rest = nullptr;

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

size_t
Environment::garbage_collect()
{
    set_mark(m_glob_nil);
    set_mark(m_glob_t);
    m_global_scope.mark_variables();
    return m_gc.garbage_collect();
}

VariableScope*
Environment::global_scope(void)
{
    return &m_global_scope;
}

size_t
Environment::exprs_in_use(void)
{
    return m_gc.exprs_in_use_count() + 2; 
}

size_t
Environment::exprs_total(void)
{
    return m_gc.exprs_total_count() + 2; 
}

const std::string
Environment::gc_info(void)
{
    auto total = exprs_total();
    auto in_use = exprs_in_use();
    std::stringstream ss;
    ss << "ExprPtr size:      " << sizeof(yal::ExprPtr) << " bytes" << std::endl
       << "Total ExprPtr's:   " << total << std::endl
       << "Current ExprPtr's: " << in_use << std::endl
       << "Current memory: " << float(in_use * sizeof(yal::ExprPtr)) / 1000000
       << " MB";
    return ss.str();
}

bool
Environment::add_local(const std::string& _name, ExprPtr _v)
{
    return m_global_scope.add_local(_name, _v);
}

bool
Environment::add_global(const std::string& _name, ExprPtr _v)
{
    return m_global_scope.add_global(_name, _v);
}

bool
Environment::add_buildin(const std::string& _name, const BuildinFn _fn)
{
    return add_global(_name, buildin(_fn));
}

Environment::Environment(void) : m_global_scope(this, nullptr) 
{
    m_glob_nil = symbol("NIL");
    m_glob_t = symbol("T");
}

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

ExprPtr
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

ExprPtr
Environment::lex(std::list<std::string>& _tokens)
{
    /*TODO: Does not support special syntax for:
    dotted lists (a . 34)
    */
    std::string token;
    ExprPtr program = nullptr;
    ExprPtr curr = nullptr;

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

ExprPtr
Environment::read(const std::string& _program)
{
    std::list<std::string> tokens;
    ExprPtr lexed = nullptr;
    tokens = tokenize(_program);
    if (tokens.empty())
        return nullptr;
    lexed = lex(tokens);
    return lexed;
}

ExprPtr
Environment::eval(ExprPtr _e)
{
    ExprPtr res = nullptr;
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

ExprPtr
_macro_expander(VariableScope* _s, const std::string& _macroname, ExprPtr _macrofn, ExprPtr _macroargs) {
    auto macro_expand = [] (auto macro_expand, VariableScope* scope, ExprPtr body) {
        if (is_nil(body))
            return body;
        if (type(body) == TYPE_SYMBOL) {
            ExprPtr s = first(scope->variable_get_this_scope(get_csym(body)));
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
    auto progn = [] (VariableScope* s, ExprPtr e) {
        ExprPtr last = nullptr;
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
    ExprPtr body_copy = _s->env()->read(stringify(get_body(_macrofn)));
    ExprPtr expanded = macro_expand(macro_expand, &internal, body_copy);
    expanded = progn(_s, expanded);
    return expanded;
}


ExprPtr
Environment::scoped_eval(VariableScope* _scope, ExprPtr _e)
{
    auto progn = [this] (VariableScope* s, ExprPtr e) {
        ExprPtr last = nullptr;
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
    auto eval_fn = [progn, this, _scope] (const std::string& name, ExprPtr fn, ExprPtr args) {
        VariableScope internal = _scope->create_internal();
        args = list_eval(&internal, args);
        internal.bind(name, fn->callable.binds, args);
        return progn(&internal, fn->callable.body);
    };

    /*mark input as being currently used so it wont be removed by the gc*/
    _scope->mark_variables();
    set_mark(_e);

    if (is_nil(_e)) return nil();

    /*Handle non-evaluating types*/
    if (type(_e) == TYPE_REAL       ||
        type(_e) == TYPE_DECIMAL    ||
        type(_e) == TYPE_STRING     )
        return _e;

    /*Handle symbols*/
    if (type(_e) == TYPE_SYMBOL) {
        ExprPtr var = nullptr;
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
        ExprPtr fn = nullptr;
        ExprPtr args = nullptr;
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

ExprPtr
Environment::list_eval(VariableScope* _scope, ExprPtr _list)
{
    ExprPtr root = nullptr;
    ExprPtr curr = _list;
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
        add_buildin("gc-run", core::gc_run);
        add_buildin("get-vars", core::get_vars);

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

ExprPtr
Environment::t(void)
{
	return m_glob_t;
}

ExprPtr
Environment::nil(void)
{
	return m_glob_nil;
}

ExprPtr
Environment::cons(ExprPtr _car, ExprPtr _cdr) {
	ExprPtr out = m_gc.new_expr(TYPE_CONS);
	if (out == nullptr)
		return nullptr;
	out->cons.car = _car;
	out->cons.cdr = _cdr;
	return out;
}

ExprPtr
Environment::real(int _v) {
     ExprPtr out = m_gc.new_expr(TYPE_REAL);
     if (out == nullptr)
       return nullptr;
     out->real = _v;
     return out;
   }

ExprPtr
Environment::decimal(float _v) {
     ExprPtr out = m_gc.new_expr(TYPE_DECIMAL);
     if (out == nullptr)
       return nullptr;
     out->decimal = _v;
     return out;
   }

ExprPtr
Environment::symbol(const std::string& _v) {
     ExprPtr out = m_gc.new_expr(TYPE_SYMBOL);
     if (out == nullptr)
       return nullptr;
     out->symbol = str_to_cstr(_v);
     return out;
}

ExprPtr 
Environment::string(const std::string& _v) {
  ExprPtr out = m_gc.new_expr(TYPE_STRING);
  if (out == nullptr)
    return nullptr;
  out->string = str_to_cstr(_v);
  return out;
}

ExprPtr 
Environment::buildin(BuildinFn _v) {
  ExprPtr out = m_gc.new_expr(TYPE_BUILDIN);
  if (out == nullptr)
    return nullptr;
  out->buildin = _v;
  return out;
}

ExprPtr
Environment::lambda(ExprPtr _binds, ExprPtr _body) {
  ExprPtr out = m_gc.new_expr(TYPE_LAMBDA);
  if (out == nullptr)
    return nullptr;
  set_callable(out, _binds, _body);
  return out;
}

ExprPtr
Environment::macro(ExprPtr _binds, ExprPtr _body) 
{
  ExprPtr out = m_gc.new_expr(TYPE_MACRO);
  if (out == nullptr)
    return nullptr;
  set_callable(out, _binds, _body);
  return out;
}

ExprPtr Environment::list(const std::initializer_list<ExprPtr>& _lst) {
    ExprPtr lst = nullptr;
    /*Iterating in reverse order*/
    for (auto it = std::rbegin(_lst); it != std::rend(_lst); ++it)
        lst = put_in_list(*it, lst);
    return lst;
}

ExprPtr
Environment::put_in_list(ExprPtr _val, ExprPtr _list)
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
stream(ExprPtr _e, bool _wrap_quotes)
{
    auto stream_value = [_wrap_quotes] (ExprPtr e) {
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
    ExprPtr curr = _e;
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
stringify(ExprPtr _e, bool _wrap_quotes)
{
    std::stringstream ss = stream(_e, _wrap_quotes);
    if (ss.rdbuf()->in_avail() == 0)
        return "";
    return ss.str();
}

ExprPtr
core::quote(VariableScope* _s, ExprPtr _e)
{
    UNUSED(_s);
    if (len(_e) > 1)
        throw _s->ProgramError("quote", "expected 1 argument, not", _e);
    return first(_e);
}

ExprPtr
core::cons(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(_e) != 2)
        throw _s->ProgramError("cons", "expected 2 arguments, got", args);
    return _s->env()->cons(first(args), second(args));
}

ExprPtr
core::car(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    ExprPtr out;
    if (len(_e) != 1)
        throw _s->ProgramError("car", "expected 1 argument, got", args);
    out = car(first(args));
    if (is_nil(out))
        return _s->env()->nil();
    return car(first(args));
}

ExprPtr
core::cdr(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    ExprPtr out;
    if (len(_e) != 1)
        throw _s->ProgramError("cdr", "expected 1 argument, got", args);
    out = cdr(first(args));
    if (is_nil(out))
        return _s->env()->nil();
    return cdr(first(args));
}

ExprPtr
core::reverse_ip(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1)
        return nullptr;
    return ipreverse(first(args));
}

ExprPtr
core::plus2(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    ExprPtr a = first(args);
    ExprPtr b = second(args);

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

ExprPtr
core::minus2(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    ExprPtr a = first(args);
    ExprPtr b = second(args);

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

ExprPtr
core::multiply2(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    ExprPtr a = first(args);
    ExprPtr b = second(args);
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

ExprPtr
core::divide2(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    ExprPtr a = first(args);
    ExprPtr b = second(args);

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

ExprPtr
core::mathequal(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    ExprPtr curr = cdr(args);
    ExprPtr prev = args;
    auto EQ2 = [] (ExprPtr a, ExprPtr b) {
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

ExprPtr
core::lessthan2(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    ExprPtr a = first(args);
    ExprPtr b = second(args);

    if (len(args) < 2)
        return _s->env()->t();
    if (!is_val(a))
        throw _s->ProgramError("<", "cannot compare non-value", a);
    if (!is_val(b))
        throw _s->ProgramError("<", "cannot compare non-value", b);

    auto LT2 = [_s] (ExprPtr a, ExprPtr b) {
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

ExprPtr
core::greaterthan2(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    ExprPtr a = first(args);
    ExprPtr b = second(args);

    if (len(args) < 2)
        return _s->env()->t();
    if (!is_val(a))
        throw _s->ProgramError("<", "cannot compare non-value", a);
    if (!is_val(b))
        throw _s->ProgramError("<", "cannot compare non-value", b);
    auto GT2 = [_s] (ExprPtr a, ExprPtr b) {
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

ExprPtr
core::is_nil(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (is_nil(first(args)))
        return _s->env()->t();
    return _s->env()->nil();
}

ExprPtr
core::slurp_file(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
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

ExprPtr
core::read(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) == 0)
        throw _s->ProgramError("read", "expects 1 argument, but did not get anything", _s->env()->nil());
    if (len(args) != 1)
        throw _s->ProgramError("read", "expects only 1 argument, got", args);
    if (type(first(args)) != TYPE_STRING)
        throw _s->ProgramError("read", "expects string as argument, not ", first(args));
    return _s->env()->read(get_str(first(args)));
}

ExprPtr
core::eval(VariableScope* _s, ExprPtr _e)
{
     ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1)
        throw _s->ProgramError("eval", "expects only 1 argument, got", args);
    return _s->env()->eval(first(args));
}

ExprPtr
core::write(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) > 1)
        throw _s->ProgramError("write", "expects single argument to write, not", args);
    _s->env()->outbuffer_put(stringify(first(args), false));
    return first(args);
}

ExprPtr
core::newline(VariableScope* _s, ExprPtr _e)
{
    if (len(_e) != 0)
        throw _s->ProgramError("newline", "expects no args, got", _e);
    _s->env()->outbuffer_put("\n");
    return _s->env()->nil();
}

ExprPtr
core::stringify(VariableScope* _s, ExprPtr _e)
{
    std::stringstream ss;
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1)
        throw _s->ProgramError("stringify", "expects only 1 argument, got", args);
    ss << stringify(first(args));
    return _s->env()->string(ss.str());
}

ExprPtr
core::concat2(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) == 0)
        return _s->env()->string("");
    if (len(args) == 1)
        return _s->env()->string(stringify(first(args), false));
    if (len(args) == 2)
        return _s->env()->string(stringify(first(args), false) + stringify(second(args), false));
    throw _s->ProgramError("concat", "can only concatenate up to 2 args, not", args);
   
}

ExprPtr
core::range(VariableScope* _s, ExprPtr _e)
{
    bool reverse = false;
    int low;
    int high;
    ExprPtr args = _s->env()->list_eval(_s, _e);
    ExprPtr out = nullptr;

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

ExprPtr
core::_if(VariableScope* _s, ExprPtr _e)
{
    ExprPtr cond = nullptr;    
    if (!(len(_e) == 2 || len(_e) == 3))
        throw _s->ProgramError("if", "expected required condition and true body, got", _e);
    cond = _s->env()->scoped_eval(_s, first(_e));
    if (!is_nil(cond))
        return _s->env()->scoped_eval(_s, second(_e));
    return _s->env()->scoped_eval(_s, third(_e));
}

ExprPtr
core::_typeof(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
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

ExprPtr
core::_try(VariableScope* _s, ExprPtr _e)
{
    ExprPtr res = nullptr;
    ExprPtr err = nullptr;
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


ExprPtr
core::_throw(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    throw _s->UserThrow(args);
    //throw _s->UserThrow(_s->env()->put_in_list(_s->env()->symbol("error"), args));
    return nullptr;
}

ExprPtr
core::_return(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) <= 1)
        throw _s->UserReturn(first(args));
    throw _s->ProgramError("return", "Return can only have 0-1 argument, got", args);
    return nullptr;
}

ExprPtr
core::variable_definition(VariableScope* _s, ExprPtr _e)
{
    ExprPtr var = nullptr;
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1)
        throw _s->ProgramError("variable-definition", "expects single arg, but got", args);
    if (type(first(args)) != TYPE_SYMBOL)
        throw _s->ProgramError("variable-definition", "expects symbol variable, but got", first(args));
    
    var = _s->variable_get(get_csym(first(args)));
    if (is_nil(var))
        return _s->env()->nil();
    return var;
}

ExprPtr
core::defglobal(VariableScope* _s, ExprPtr _e)
{
    ExprPtr val = _s->env()->scoped_eval(_s, second(_e));
    if (len(_e) != 2)
        throw _s->ProgramError("global!", "expects symbol and value, got", _e);
    if (type(first(_e)) != TYPE_SYMBOL)
        throw _s->ProgramError("global!", "expects name to be type symbol, got", first(_e));
    _s->add_global(get_csym(first(_e)), val);
    return first(_e);
}

ExprPtr
core::deflocal(VariableScope* _s, ExprPtr _e)
{
    ExprPtr val = _s->env()->scoped_eval(_s, second(_e));
    if (len(_e) != 2)
        throw _s->ProgramError("local!", "expects symbol and value, got", _e);
    if (type(first(_e)) != TYPE_SYMBOL)
        throw _s->ProgramError("local!", "expects name to be type symbol, got", first(_e));
    _s->add_local(get_csym(first(_e)), val);
    return first(_e);
}

ExprPtr
core::setcar(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (type(first(args)) != TYPE_CONS)
        throw _s->ProgramError("setcar!", "Expected cons to modify, got", first(args));
    set_car(first(args), second(args));
    return second(args);
}

ExprPtr
core::setcdr(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (type(first(args)) != TYPE_CONS)
        throw _s->ProgramError("setcdr!", "Expected cons to modify, got", first(args));
    set_cdr(first(args), second(args));
    return second(args);
}

ExprPtr
core::deflambda(VariableScope* _s, ExprPtr _e)
{
    UNUSED(_s);
    if (len(_e) < 1)
        throw _s->ProgramError("lambda", "expected arguments to be binds and body, got", _e);
    ExprPtr l = _s->env()->lambda(car(_e), cdr(_e));
    return l;
}

ExprPtr
core::deffunction(VariableScope* _s, ExprPtr _e)
{
    if (len(_e) < 2)
        throw _s->ProgramError("fn!", "expected arguments to be name, binds and body, got", _e);
    ExprPtr fn = _s->env()->lambda(second(_e), cdr(cdr(_e)));
    _s->add_global(get_csym(first(_e)), fn);
    return first(_e);
}

ExprPtr
core::defmacro(VariableScope* _s, ExprPtr _e)
{
    if (len(_e) < 2)
        throw _s->ProgramError("macro!", "expected arguments to be name, binds and body, got", _e);
    ExprPtr macro = _s->env()->macro(second(_e), cdr(cdr(_e)));
    _s->add_global(get_csym(first(_e)), macro);
    return first(_e);
}

ExprPtr
core::macro_expand(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || type(first(args)) != TYPE_CONS)
        throw _s->ProgramError("macro-expand", "expected macro call as list, got", args);
    ExprPtr macro_call = first(args);
    if (type(first(macro_call)) != TYPE_SYMBOL)
        throw _s->ProgramError("macro-expand",
                           "expected first arg to be a symbol pointing to a macro, not",
                           first(macro_call));

    const std::string macro_name = get_sym(first(macro_call));
    ExprPtr macro_fn = first(_s->variable_get(get_csym(first(macro_call))));
    ExprPtr macro_args = cdr(macro_call);
    ExprPtr expanded = _macro_expander(_s, macro_name, macro_fn, macro_args);
    return expanded;
}

ExprPtr
core::cos(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("cos", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::cos(get_decimal(first(args))));
    return _s->env()->decimal(std::cos(get_real(first(args))));
}

ExprPtr
core::sin(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("sin", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::sin(get_decimal(first(args))));
    return _s->env()->decimal(std::sin(get_real(first(args))));
}

ExprPtr
core::tan(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("tan", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::tan(get_decimal(first(args))));
    return _s->env()->decimal(std::tan(get_real(first(args))));
}

ExprPtr
core::acos(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("acos", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::acos(get_decimal(first(args))));
    return _s->env()->decimal(std::acos(get_real(first(args))));
}
    
ExprPtr
core::asin(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("asin", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::asin(get_decimal(first(args))));
    return _s->env()->decimal(std::asin(get_real(first(args))));
}
    
ExprPtr
core::atan(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("atan", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::atan(get_decimal(first(args))));
    return _s->env()->decimal(std::atan(get_real(first(args))));
}
    
ExprPtr
core::cosh(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("cosh", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::cosh(get_decimal(first(args))));
    return _s->env()->decimal(std::cosh(get_real(first(args))));
}
    
ExprPtr
core::sinh(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("sinh", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::sinh(get_decimal(first(args))));
    return _s->env()->decimal(std::sinh(get_real(first(args))));
}
    
ExprPtr
core::tanh(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("tanh", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::tanh(get_decimal(first(args))));
    return _s->env()->decimal(std::tanh(get_real(first(args))));
}
    
ExprPtr
core::floor(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("floor", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->real(std::floor(get_decimal(first(args))));
    return _s->env()->real(std::floor(get_real(first(args))));
}
    
ExprPtr
core::ceil(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("ceil", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->real(std::ceil(get_decimal(first(args))));
    return _s->env()->real(std::ceil(get_real(first(args))));
}

ExprPtr
core::sqrt(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("sqrt", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::sqrt(get_decimal(first(args))));
    return _s->env()->decimal(std::sqrt(get_real(first(args))));
}
    
ExprPtr
core::log(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("log", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::log(get_decimal(first(args))));
    return _s->env()->decimal(std::log(get_real(first(args))));
}
    
ExprPtr
core::log10(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("log10", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::log10(get_decimal(first(args))));
    return _s->env()->decimal(std::log10(get_real(first(args))));
}

ExprPtr
core::exp(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    if (len(args) != 1 || !is_val(first(args)))
        throw _s->ProgramError("exp", "expected 1 value arg, got", args);
    if (type(first(args)) == TYPE_DECIMAL)
        return _s->env()->decimal(std::exp(get_decimal(first(args))));
    return _s->env()->decimal(std::exp(get_real(first(args))));
}

ExprPtr
core::pow(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    ExprPtr a = first(args);
    ExprPtr b = second(args);
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

ExprPtr
core::modulus(VariableScope* _s, ExprPtr _e)
{
    ExprPtr args = _s->env()->list_eval(_s, _e);
    ExprPtr a = first(args);
    ExprPtr b = second(args);
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

ExprPtr
core::get_time(VariableScope *_s, ExprPtr _e)
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

ExprPtr
core::gc_info(VariableScope* _s, ExprPtr _e)
{
    if (len(_e) != 0)
        throw _s->ProgramError("gc-info", "expects no arguments, got", _e);
    return _s->env()->string(_s->env()->gc_info());
}

ExprPtr
core::gc_run(VariableScope* _s, ExprPtr _e)
{
    if (len(_e) != 0)
        throw _s->ProgramError("gc-run", "expects no arguments, got", _e);
    _s->mark_variables();
    size_t collected = _s->env()->garbage_collect();
    return _s->env()->string("GC collected " + std::to_string(collected) + " ExprPtr's");
}

//   ExprPtr
//   core::get_all_globals(VariableScope* _s, ExprPtr _e)
//   {
//       std::vector<std::string> locals{};
//       if (len(_e) != 0)
//           throw _s->ProgramError("get-all-locals", "expects no arguments, got", _e);
//       auto lmap = _s->global_scope()->variables_get_all();
//       for (auto& kv: lmap) {
//           std::string local = "";
//           if (is_macro(first(kv.second))) {
//               local += "(macro! " + kv.first + " ";
//               ExprPtr e = first(first(kv.second))
//               while (!is_nil(e)) {
//                   local += car
//
//               }
//               local +=")\n";
//           }
//           else if (is_lambda(first(kv.second))) {
//               local += "(fn! " + kv.first + " "
//                   + stringify(get_binds(first(kv.second))) + "\n"
//                   + "  " + stringify(get_body(first(kv.second))) + ")\n";
//
//           }
//           else {
//               local += kv.first + " " + stringify(first(kv.second)) + "\n";
//           }
//           locals.push_back(local);
//       }
//
//       std::string str = "";
//       for (auto s: locals)
//           str += s;
//
//       return _s->env()->string(str);
//   }

//   ExprPtr
//   core::get_all_globals(VariableScope* _s, ExprPtr _e)
//   {
//       std::vector<std::string> locals{};
//       if (len(_e) != 0)
//           throw _s->ProgramError("get-all-locals", "expects no arguments, got", _e);
//       auto lmap = _s->global_scope()->variables_get_all();
//       for (auto& kv: lmap) {
//           std::string local = "";
//           if (is_macro(first(kv.second))) {
//               local += "(macro! " + kv.first + " ";
//               ExprPtr e = first(first(kv.second))
//               while (!is_nil(e)) {
//                   local += car
//
//               }
//               local +=")\n";
//           }
//           else if (is_lambda(first(kv.second))) {
//               local += "(fn! " + kv.first + " "
//                   + stringify(get_binds(first(kv.second))) + "\n"
//                   + "  " + stringify(get_body(first(kv.second))) + ")\n";
//
//           }
//           else {
//               local += kv.first + " " + stringify(first(kv.second)) + "\n";
//           }
//           locals.push_back(local);
//       }
//
//       std::string str = "";
//       for (auto s: locals)
//           str += s;
//
//       return _s->env()->string(str);
//   }

ExprPtr
core::get_vars(VariableScope* _s, ExprPtr _e)
{
    ExprPtr locals = nullptr;
    if (len(_e) != 0)
        throw _s->ProgramError("get-globals", "expects no arguments, got", _e);

    auto get_vars_from_scope = [&locals] (VariableScope* s) {
        auto lmap = s->global_scope()->variables_get_all();
        for (auto& kv: lmap) {
            if (!is_nil(kv.second))
                locals = s->env()->put_in_list(s->env()->symbol(kv.first), locals);
        }
    };
        
    VariableScope* curr = _s;
    while (curr != nullptr) {
        get_vars_from_scope(curr);
        curr = curr->outer_scope();
    }
    return locals;
}

#endif /*YALPP_IMPLEMENTATION*/
#endif /*YALCPP_H*/

}; /*namespace yal*/
