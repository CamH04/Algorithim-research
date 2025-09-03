/***************************************************************************************
*    Title: A Mathematical Approach To Compilers
*    Author: Cameron Haynes
*    Date: 03/09/2025
*    Description: translator class implmentation for compiler (src lang -> LC)
***************************************************************************************/

#include "translator.hpp"
#include <stdexcept>
#include <unordered_map>
#include <iostream>
#include <sstream>

static std::unordered_map<int, std::string> numeral_cache;

static bool is_integer_atom(const std::string &a) {
    if (a.empty()) return false;
    size_t i = 0;
    if (a[0] == '-' && a.size() > 1) i = 1;
    for (; i < a.size(); ++i) {
        if (!isdigit((unsigned char)a[i])) return false;
    }
    return true;
}

static std::string church_numeral(int n) {
    if (n < 0) throw std::runtime_error("Negative numbers not supported in Church numerals");

    auto it = numeral_cache.find(n);
    if (it != numeral_cache.end()) return it->second;

    std::string result;
    if (n == 0) {
        result = "λf.λx.x";
    } else {
        result = "λf.λx.";
        for (int i = 0; i < n; ++i) {
            result += "f ";
        }
        result += "x";
        for (int i = 0; i < n; ++i) {
            result += "";
        }
    }
    numeral_cache[n] = result;
    return result;
}
//function application helper
static std::string app(const std::string &f, const std::string &arg) {
    return "(" + f + " " + arg + ")";
}

//ops
const std::string LADD = "(λm.λn.λf.λx. m f (n f x))";  // addition
const std::string LMUL = "(λm.λn.λf. m (n f))";         // multiplication
const std::string LSUB = "(λm.λn. n PRED m)";           // subtraction
const std::string LDIV = "(λm.λn. m (n PRED) m)";       // division

//bools
const std::string LTRUE  = "(λt.λf.t)";
const std::string LFALSE = "(λt.λf.f)";
const std::string LAND   = "(λp.λq. p q p)";            // AND
const std::string LOR    = "(λp.λq. p p q)";            // OR
const std::string LNOT   = "(λp.λa.λb. p b a)";         // NOT

//comparison
const std::string LEQ    = "(λm.λn. ISZERO (SUB m n))"; // <=
const std::string LLT    = "(λm.λn. NOT (LEQ n m))";    // <
const std::string LGT    = "(λm.λn. LT n m)";           // >
const std::string LGEQ   = "(λm.λn. LEQ n m)";          // >=

//help ops
const std::string LSUCC  = "(λn.λf.λx. f (n f x))";     // successor
const std::string LPRED  = "(λn.λf.λx. n (λg.λh. h (g f)) (λu.x) (λu.u))"; // predecessor
const std::string LISZERO = "(λn. n (λx.FALSE) TRUE)";  // ? == 0

// recursion
const std::string Y_COMBINATOR = "(λf. (λx. f (x x)) (λx. f (x x)))";

//pairs
const std::string LPAIR  = "(λx.λy.λf. f x y)";         // create pair
const std::string LFIRST = "(λp. p (λx.λy.x))";         // first element
const std::string LSECOND = "(λp. p (λx.λy.y))";        // second element

//pair
const std::string LNIL   = "(λx.TRUE)";                 // rmpty list
const std::string LCONS  = "(λh.λt.λf. f h t)";         // cons
const std::string LHEAD  = "FIRST";                     // list head
const std::string LTAIL  = "SECOND";                    // list tail
const std::string LISNIL = "(λl. l (λh.λt.FALSE))";     // ? == empty

static std::string translate_atom(const std::string &a) {
    if (is_integer_atom(a)) {
        int v = std::stoi(a);
        return church_numeral(v);
    }
    if (a == "true" || a == "TRUE") return LTRUE;
    if (a == "false" || a == "FALSE") return LFALSE;
    if (a == "nil" || a == "NIL") return LNIL;
    return a;
}

static std::string translate_list(const std::vector<std::shared_ptr<SExpr>> &lst) {
    if (lst.empty()) return LNIL;

    if (!lst[0]->is_atom) {
        std::string res = translate(lst[0]);
        for (size_t i = 1; i < lst.size(); ++i) {
            res = app(res, translate(lst[i]));
        }
        return res;
    }

    std::string head = lst[0]->atom;
    if (head == "+") {
        if (lst.size() < 3) throw std::runtime_error("Operator + needs at least two operands");
        std::string res = translate(lst[1]);
        for (size_t i = 2; i < lst.size(); ++i) {
            res = app(app(LADD, res), translate(lst[i]));
        }
        return res;
    }
    if (head == "*") {
        if (lst.size() < 3) throw std::runtime_error("Operator * needs at least two operands");
        std::string res = translate(lst[1]);
        for (size_t i = 2; i < lst.size(); ++i) {
            res = app(app(LMUL, res), translate(lst[i]));
        }
        return res;
    }
    if (head == "-") {
        if (lst.size() != 3) throw std::runtime_error("Operator - requires exactly two operands");
        return app(app(LSUB, translate(lst[1])), translate(lst[2]));
    }
    if (head == "/") {
        if (lst.size() != 3) throw std::runtime_error("Operator / requires exactly two operands");
        return app(app(LDIV, translate(lst[1])), translate(lst[2]));
    }
    if (head == "=" || head == "eq") {
        if (lst.size() != 3) throw std::runtime_error("Equality requires exactly two operands");
        return app(app(LEQ, translate(lst[1])), translate(lst[2]));
    }
    if (head == "<") {
        if (lst.size() != 3) throw std::runtime_error("Less than requires exactly two operands");
        return app(app(LLT, translate(lst[1])), translate(lst[2]));
    }
    if (head == ">") {
        if (lst.size() != 3) throw std::runtime_error("Greater than requires exactly two operands");
        return app(app(LGT, translate(lst[1])), translate(lst[2]));
    }
    if (head == "<=" || head == "leq") {
        if (lst.size() != 3) throw std::runtime_error("Less than or equal requires exactly two operands");
        return app(app(LGEQ, translate(lst[1])), translate(lst[2]));
    }
    if (head == ">=" || head == "geq") {
        if (lst.size() != 3) throw std::runtime_error("Greater than or equal requires exactly two operands");
        return app(app(LGEQ, translate(lst[1])), translate(lst[2]));
    }
    if (head == "and") {
        if (lst.size() < 3) throw std::runtime_error("AND requires at least two operands");
        std::string res = translate(lst[1]);
        for (size_t i = 2; i < lst.size(); ++i) {
            res = app(app(LAND, res), translate(lst[i]));
        }
        return res;
    }
    if (head == "or") {
        if (lst.size() < 3) throw std::runtime_error("OR requires at least two operands");
        std::string res = translate(lst[1]);
        for (size_t i = 2; i < lst.size(); ++i) {
            res = app(app(LOR, res), translate(lst[i]));
        }
        return res;
    }
    if (head == "not") {
        if (lst.size() != 2) throw std::runtime_error("NOT requires exactly one operand");
        return app(LNOT, translate(lst[1]));
    }
    if (head == "lambda" || head == "λ") {
        if (lst.size() < 3) throw std::runtime_error("Malformed lambda expression");
        std::string body = translate(lst[2]);
        if (lst[1]->is_atom) {
            return "λ" + lst[1]->atom + "." + body;
        } else {
            for (auto it = lst[1]->list.rbegin(); it != lst[1]->list.rend(); ++it) {
                if (!(*it)->is_atom) {
                    throw std::runtime_error("Lambda parameter must be a symbol");
                }
                body = "λ" + (*it)->atom + "." + body;
            }
            return body;
        }
    }
    if (head == "let") {
        if (lst.size() != 3) throw std::runtime_error("Let requires bindings and body");
        if (lst[1]->is_atom || lst[1]->list.empty()) {
            throw std::runtime_error("Let requires binding list");
        }

        std::string body = translate(lst[2]);
        for (auto it = lst[1]->list.rbegin(); it != lst[1]->list.rend(); ++it) {
            auto binding = *it;
            if (binding->is_atom || binding->list.size() != 2 || !binding->list[0]->is_atom) {
                throw std::runtime_error("Invalid let binding format: expected (var value)");
            }
            std::string var = binding->list[0]->atom;
            std::string val = translate(binding->list[1]);
            body = app("λ" + var + "." + body, val);
        }
        return body;
    }
    if (head == "if") {
        if (lst.size() != 4) {
            throw std::runtime_error("If requires exactly 3 arguments: condition, then-branch, else-branch");
        }
        std::string cond = translate(lst[1]);
        std::string thenBranch = translate(lst[2]);
        std::string elseBranch = translate(lst[3]);
        return app(app(cond, thenBranch), elseBranch);
    }
    if (head == "cons") {
        if (lst.size() != 3) throw std::runtime_error("Cons requires exactly two arguments");
        return app(app(LCONS, translate(lst[1])), translate(lst[2]));
    }
    if (head == "car" || head == "head") {
        if (lst.size() != 2) throw std::runtime_error("Car/head requires exactly one argument");
        return app(LHEAD, translate(lst[1]));
    }
    if (head == "cdr" || head == "tail") {
        if (lst.size() != 2) throw std::runtime_error("Cdr/tail requires exactly one argument");
        return app(LTAIL, translate(lst[1]));
    }
    if (head == "null?" || head == "nil?") {
        if (lst.size() != 2) throw std::runtime_error("Null test requires exactly one argument");
        return app(LISNIL, translate(lst[1]));
    }
    if (head == "pair") {
        if (lst.size() != 3) throw std::runtime_error("Pair requires exactly two arguments");
        return app(app(LPAIR, translate(lst[1])), translate(lst[2]));
    }
    if (head == "first" || head == "fst") {
        if (lst.size() != 2) throw std::runtime_error("First requires exactly one argument");
        return app(LFIRST, translate(lst[1]));
    }
    if (head == "second" || head == "snd") {
        if (lst.size() != 2) throw std::runtime_error("Second requires exactly one argument");
        return app(LSECOND, translate(lst[1]));
    }
    if (head == "rec" || head == "recursive") {
        if (lst.size() != 2) throw std::runtime_error("Recursive definition requires exactly one argument");
        return app(Y_COMBINATOR, translate(lst[1]));
    }
    if (head == "succ") {
        if (lst.size() != 2) throw std::runtime_error("Successor requires exactly one argument");
        return app(LSUCC, translate(lst[1]));
    }
    if (head == "pred") {
        if (lst.size() != 2) throw std::runtime_error("Predecessor requires exactly one argument");
        return app(LPRED, translate(lst[1]));
    }
    if (head == "zero?") {
        if (lst.size() != 2) throw std::runtime_error("Zero test requires exactly one argument");
        return app(LISZERO, translate(lst[1]));
    }

    // generel func application
    std::string res = translate(lst[0]);
    for (size_t i = 1; i < lst.size(); ++i) {
        res = app(res, translate(lst[i]));
    }
    return res;
}

std::string translate(const std::shared_ptr<SExpr> &sexpr) {
    if (sexpr->is_atom) {
        return translate_atom(sexpr->atom);
    }
    return translate_list(sexpr->list);
}

//util funcs
void print_cache_stats() {
    std::cout << "Church numeral cache contains " << numeral_cache.size() << " entries\n";
}
void clear_cache() {
    numeral_cache.clear();
}
