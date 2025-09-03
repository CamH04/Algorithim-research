#ifndef TRANSLATOR_HPP
#define TRANSLATOR_HPP

#include "parser.hpp"
#include <string>

// Position in Compiler Pipeline:
//   Source (.lctest) -> AST (S-expressions) -> λ-calculus IR -> Backend (.lc)
//
// Translation Strategy:
//   Variables        -> Bound variable names
//   (lambda (x) e)   -> λx. e  (with currying for multiple args)
//   (f a b)          -> ((f a) b)  (left-associative application)
//   (+ a b)          -> (ADD a b) with Church encodings
//   Numbers          -> Church numerals (with CSE caching)
//   Booleans         -> Church booleans (λt.λf.t / λt.λf.f)
//   Let-bindings     -> Lambda abstraction + application
//   Lists            -> Church pairs and nil
//   Recursion        -> Y combinator
//
// Optimizations:
//   • Numeral caching prevents recomputation of large Church numerals
//   • CSE reduces IR size and compilation time
//   • Curried functions enable partial application
//
//  Language Constructs:
//   • Arithmetic: +, -, *, / (Church numeral operations)
//   • Comparisons: =, <, >, <=, >= (Church numeral comparisons)
//   • Booleans: and, or, not, true, false
//   • Control flow: if-then-else (Church boolean conditionals)
//   • Functions: lambda with currying, recursion via Y combinator
//   • Data structures: pairs, lists (cons, car, cdr, nil)
//   • Let bindings (desugared to lambda application)
//   • Utility functions: succ, pred, zero?, null?
//
// Examples:
//   (+ 2 3)           -> ((λm.λn.λf.λx. m f (n f x)) (λf.λx.f (f x)) (λf.λx.f (f (f x))))
//   (if true 1 0)     -> (((λt.λf.t) (λf.λx.f x)) (λf.λx.x))
//   (lambda (x y) x)  -> λx.λy.x
//   (let ((x 5)) x)   -> ((λx.x) (λf.λx.f (f (f (f (f x))))))
// ============================================================

std::string translate(const std::shared_ptr<SExpr> &sexpr);
void print_cache_stats();
void clear_cache();

#endif
