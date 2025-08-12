
#ifndef TRANSLATOR_HPP
#define TRANSLATOR_HPP

#include "parser.hpp"
#include <string>

// ============================================================
// Position in Compiler Pipeline
//
//   Source (Lisp-like) → AST (frontend) → λ-calculus IR → Backend IR / codegen
//
// Translation Strategy:
//   Variables        → Bound variable names
//   (lambda (x) e)   → λx. e
//   (f a b)          → ((f a) b)
//   (+ a b)          → (ADD a b) with Church encodings
//   Numbers          → Church numerals
//   Booleans         → Church booleans
//   Let-bindings     → Lambda abstraction + application
//
// Transformations:
//   α-conversion — avoid variable capture
//   β-reduction — inline constant calls
//   η-conversion — remove redundant λ abstractions
//
// Evaluation Strategy:
//   Usually call-by-value for target language codegen
// ============================================================

std::string translate(const std::shared_ptr<SExpr> &sexpr);

#endif
