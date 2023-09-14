#pragma once

#include <string>

// std library to implement amongst others
//https://www.buildyourownlisp.com/chapter15_standard_library#minimalism

namespace yal {

const char *std_lib =
    "(progn "
    /*Info*/
    " (const! *creator* \"Thomas Alexgaard\")"
    " (const! *creator-git* \"https://github.com/thom9258/\")"
    " (const! *host* \"C++\")"
    " (const! *version* '(0 0 1))"

    /*Predicates*/
    " (fn! notnil?  (v) (not (nil? v)))"
    " (fn! real?    (v) (= (typeof v) 'real))"
    " (fn! decimal? (v) (= (typeof v) 'decimal))"
    " (fn! value?   (v) (or (= (typeof v) 'real) (= (typeof v) 'decimal)))"
    " (fn! symbol?  (v) (= (typeof v) 'symbol))"
    " (fn! string?  (v) (= (typeof v) 'string))"
    " (fn! cons?    (v) (= (typeof v) 'cons))"
    " (fn! list?    (v) (or (nil? v) (= (typeof v) 'cons)))"
    " (fn! lambda?  (v) (= (typeof v) 'lambda))"
    " (fn! fn?      (v) (= (typeof v) 'function))"
    " (fn! macro?   (v) (= (typeof v) 'macro))"

    " (fn! var? (var)"
    "   (and (symbol? var) (notnil? (variable-definition var))))"

    " (fn! const? (var)"
    "   (if (not (var? var)) (return NIL))"
    "   (= 'CONSTANT (third (variable-definition var))))"

    /*TODO: use this one with a folding function for multiple values*/
    //" (fn! _EQUAL2 (a b)"
    //"   (= (stringify a) (stringify b)))"


    /*File Management*/
    " (fn! load-file (f) (eval (read (slurp-file f))))"

    /*Math*/
    " (fn! zero? (v) (or (= v 0) (= v 0.0)))"
    " (fn! abs (v) (if (< v 0) (* -1 v) v))"
    //" (fn! even? (v) )"
    //" (fn! odd?  (v) )"

    " (const! PI         3.141592)"
    " (const! PI2        (* PI 2))"
    " (const! PI/2       (/ PI 2))"
    " (const! PI/3       (/ PI 3))"
    " (const! PI/4       (/ PI 4))"
    " (const! E          2.71828)"
    " (const! E-constant 0.57721)"

    /*List Management*/
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
    "l)))))))))))"

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

    " (fn! not (x)"
    "  (if (nil? x) T NIL))"

    " (macro! setq! (v x)"
    "   ['set! 'v x])"

    " (macro! when (test &body)"
    "  (if test (progn body)))"

    " (macro! unless (test &body)"
    "  (if test NIL (progn body)))"

    " (fn! put (v l)"
    "  \"Join value onto front of list\""
    "  (cons v l))"

    " (fn! apply (fn list)"
    "   \"apply function to list\""
    "   (eval (put fn list)))"


    " (fn! len (l)"
    "  \"calculate length of list\""
    "  (if (nil? l)"
    "    0"
    "    (+ 1 (len (rest l)))))"

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

    " (fn! reduce (fn init list)"
    "   \"reduce list by applying fn starting with init\""
    "   (if (nil? list)"
    "     init"
    "     (reduce fn (fn init (first list)) (rest list))"
    " ))"

    " (fn! format (&args)"
    "   \"stringify and concatenate arguments to single string\""
    " (throw \"format not implemented\"))"

    " (fn! print (&args)"
    "   \"print formatted string\""
    "   (throw \"print not implemented\")"
    "   (write (format args)))"

    " (fn! recurse (n)"
    "   \"create recursion calls n times (used to test "
    "tail-call-optimization)\""
    "   (if (> n 0) (recurse (- n 1)) 'did-we-blow-up?))"

    " (fn! setnth! (n val list)"
    "   \"set the nth value of a list\""
    "   (local! target (nthcdr n list))"
    "   (if (nil? target)"
    "     (throw \"invalid index to set, got\" n)"
    "     (setcar! target val)))"

    " (fn! variable-value (var)"
    "   \"get value associated with variable\""
    "   (second (variable-definition var)))"

    " (fn! set! (var val)"
    "   \"change variable to become value on evaluation\""
    "   (if (not (var? var)) (throw \"set! expected symbol variable, not\" "
    "var))"
    "   (if (const? var) (throw \"set! cannot set constant\" var))"
    "   (setnth! 1 val (variable-definition var)))"

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

    " (fn! map (fn lst)"
    "   \"apply fn to all value in lst and collect results\""
    "   (if (nil? lst)"
    "     NIL"
    "     (put ((variable-value 'fn) (first lst)) (map fn (rest lst)))))"

    /*TODO: this one should be a macro to allow &body instead of single body*/
    " (fn! scope (body)"
    "   \"evaluate body in internal scope\""
    "   ((lambda (x) (eval x)) body))"

    " (fn! let1 (vardef body)"
    "   \"evaluate body in internal scope\""
    "   (scope (progn (apply 'local! vardef) body)))"



    //" (fn! reduce (fn start lst)"
    //"   \"evaluate fn incrementally along lst using start value\""
    //"   (write start) (write lst)"
    //"   (if (nil? lst)"
    //"     start"
    //"     (reduce fn ((variable-value 'fn) (first lst)) (rest lst))))"



    //" (fn! reduce (f start lst)"
    //"   \"evaluate fn incrementally along lst using start value\""
    ////"   (write fn) (write start) (write lst)"
    //"     )"

/*
(fun {foldl f z l} {
  if (== l nil)
    {z}
    {foldl f (f z (fst l)) (tail l)}
})
*/

    " 'std)";
}
