#ifndef YAL_LEXER_H
#define YAL_LEXER_H

#include "core.h"

char
_is_whitespace(char _c)
{
    if (_c == ' ' || _c == '\t' || _c == '\n' )
        return 1;
    return 0;
}

char
_can_lex_number(char* _start)
{
    if (*_start > '0' && *_start < '9')
        return 1; 
    return 0;
}

char
_can_lex_string(char* _start)
{
    if (*_start == '"')
        return 1;
    return 0;
}

char
_is_symbol_real_or_decimal(tstr* _num)
/*TODO: Make this more robust..*/
{
    tstr decimal_indicator = tstr_const(".");
    if (tstr_find(_num, &decimal_indicator) == TSTR_INVALID)
        return TYPE_REAL;
    return TYPE_DECIMAL;
}

expr*
_lex_number(Environment *_env, char* _start, int* _cursor)
{
    tstr str;
    int len = 0;
    char type;
    while (!_is_whitespace(_start[len]) && _start[len] != '(' && _start[len] != ')')
        len++;
    str = tstr_n(_start, len+1);
    (*_cursor) += tstr_length(&str);
    //printf("number lexed = %s\n", str.c_str);
    type = _is_symbol_real_or_decimal(&str);
    if (type == TYPE_REAL)
        return real(_env, tstr_to_int(&str));
    return decimal(_env, tstr_to_float(&str));
}

expr*
_lex_string(Environment *_env, char* _start, int* _cursor)
{
    int len = 1;
    tstr str;
    while (_start[len] != '"')
        len++;
    str = tstr_n(_start+1, len);
    (*_cursor) += tstr_length(&str)+1;
    return string(_env, str);
}

expr*
_lex_symbol(Environment *_env, char* _start, int* _cursor)
{
    int len = 0;
    tstr str;
    while (!_is_whitespace(_start[len]) && _start[len] != '(' && _start[len] != ')')
        len++;
    str = tstr_n(_start, len+1);
    (*_cursor) += tstr_length(&str)-1;
    return symbol(_env, str);
}
void
expr_lex(Environment* _env, expr* _root, char* _program, int* _cursor)
{
    /*TODO: Does not support special syntax for:
    quote = '
    array = []
    */
    ASSERT_INV_ENV(_env);
    assert(_program != NULL && "Invalid program given to lexer");
    assert(_cursor != NULL && "Invalid cursor given to lexer");
    assert(Buildins_len(&_env->buildins) > 0 && "No buildin functions in environment!");
    ERRCHECK_NIL(_env, _root);

    expr* curr = _root; 
    expr* extracted;
    char* p;
    while (1) {
        p = _program + (*_cursor)++;
        if (*p == '\0') {
            return;
        }
        if (_is_whitespace(*p)) {
            continue;
        }
        else if (*p == ')') {
            return;
        }
        else if (*p ==  '(') {
            extracted = cons(_env, NIL, NIL);
            expr_lex(_env, extracted, _program, _cursor);
        }
        else if (_can_lex_string(p)) {
            extracted = _lex_string(_env, p, _cursor);
        }
        else if (_can_lex_number(p)) {
            extracted = _lex_number(_env, p, _cursor);
        }
        else {
          /*We determine that if it is not
            a string or a number, it must be a symbol*/
            extracted = _lex_symbol(_env, p, _cursor);
        }
        /*Insert extracted into expression*/
        curr->car = extracted;
        curr->cdr = cons(_env, NIL, NIL);
        curr = cdr(curr);
    }
    ASSERT_UNREACHABLE();
}

#endif /*YAL_LEXERH*/
