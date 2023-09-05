#pragma once

#include <string>

// std library to implement amongst others
//https://www.buildyourownlisp.com/chapter15_standard_library#minimalism

namespace yal {

    const char* std_lib =
        "(progn "
        /*Info*/
        " (const! *Creator* \"Thomas Alexgaard\")"
        " (const! *Creator-Git* \"https://github.com/thom9258/\")"
        " (const! *Host* \"C++\")"
        " (const! *Version* '(0 0 1))"

        " (const! NIL 'NIL)"
        " (const! nil 'NIL)"
        " (const! T 'T)"
        " (const! t 'T)"
        
        /*Predicates*/
        " (fn! real?    (v) (= (typeof v) 'real))"
        " (fn! decimal? (v) (= (typeof v) 'decimal))"
        " (fn! value?   (v) (or (= (typeof v) 'real) (= (typeof v) 'decimal)))"
        " (fn! symbol?  (v) (= (typeof v) 'symbol))"
        " (fn! string?  (v) (= (typeof v) 'string))"
        " (fn! list?    (v) (= (typeof v) 'cons))"
        " (fn! lambda?  (v) (= (typeof v) 'lambda))"
        " (fn! fn?      (v) (= (typeof v) 'function))"
        " (fn! macro?   (v) (= (typeof v) 'macro))"

        /*File Management*/
        " (fn! load-file (f) (eval (read (slurp-file f))))"
        
        /*Math*/
        " (fn! zero? (v) (or (= v 0) (= v 'decimal)))"
        //" (fn! even? (v) )"
        //" (fn! odd?  (v) )"
        
        " (const! PI         3.141592)"
        " (const! PI2        (* PI 2))"
        " (const! PI/2       (/ PI 2))"
        " (const! E          2.71828)"
        " (const! E-constant 0.57721)"
        
        /*List Management*/
        " (fn! head  (l) (car l))"
        " (fn! tail  (l) (cdr l))"
        " (fn! first (l) (car l))"
        " (fn! rest  (l) (cdr l))"
        
        " (fn! second  (l) (cdr (car l)))"
        " (fn! third   (l) (cdr (cdr (car l))))"
        " (fn! fourth  (l) (cdr (cdr (cdr (car l)))))"
        " (fn! fifth   (l) (cdr (cdr (cdr (cdr (car l))))))"
        " (fn! sixth   (l) (cdr (cdr (cdr (cdr (cdr (car l)))))))"
        " (fn! seventh (l) (cdr (cdr (cdr (cdr (cdr (cdr (car l))))))))"
        " (fn! eighth  (l) (cdr (cdr (cdr (cdr (cdr (cdr (cdr (car l)))))))))"
        " (fn! ninth   (l) (cdr (cdr (cdr (cdr (cdr (cdr (cdr (cdr (car l))))))))))"
        " (fn! tenth   (l) (cdr (cdr (cdr (cdr (cdr (cdr (cdr (cdr (cdr (car l)))))))))))"
        
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

        " (fn! len (l)"
        "  (print l)"
        "  (if (= l NIL)"
        "    (+ 1 (len (cdr l)))"
        "    0"
        " ))"





        " 'std)"
        ;
}
