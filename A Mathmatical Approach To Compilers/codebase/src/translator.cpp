#include "translator.hpp"
#include <stdexcept>
#include <unordered_map>
#include <iostream>

static bool is_integer_atom(const std::string &a) {
    if (a.empty()) return false;
    size_t i = 0;
    if (a[0] == '-' && a.size() > 1) i = 1;
    for (; i < a.size(); ++i) if (!isdigit((unsigned char)a[i])) return false;
    return true;
}

static std::unordered_map<int, std::string> numeral_cache;

static std::string church_numeral(int n) {
    auto it = numeral_cache.find(n);
    if (it != numeral_cache.end()) return it->second;

    std::string lam = "λf.λx.";
    for (int i = 0; i < n; ++i) lam += "f(";
    lam += "x";
    for (int i = 0; i < n; ++i) lam += ")";

    numeral_cache[n] = lam;
    return lam;
}

static std::string app(const std::string &f, const std::string &arg) {
    return "(" + f + " " + arg + ")";
}

const std::string LADD = "(λm.λn.λf.λx. m f (n f x))";
const std::string LMUL = "(λm.λn.λf. m (n f))";
const std::string LTRUE  = "(λt.λf.t)";
const std::string LFALSE = "(λt.λf.f)";

static std::string translate_atom(const std::string &a) {
    //std::cerr << "Translating atom: " << a << "\n";
    if (is_integer_atom(a)) {
        int v = std::stoi(a);
        if (v < 0) throw std::runtime_error("Negative numbers not supported");
        return church_numeral(v);
    }
    if (a == "true") return LTRUE;
    if (a == "false") return LFALSE;

    return a; // <-- make sure we don’t accidentally drop the atom!
}


static std::string translate_list(const std::vector<std::shared_ptr<SExpr>> &lst) {
    if (lst.empty()) return "()";
    if (lst[0]->is_atom) {
        std::string head = lst[0]->atom;
        if (head == "+") {
            if (lst.size() < 3) throw std::runtime_error("Operator + needs at least two operands");
            std::string res = translate(lst[1]);
            for (size_t i = 2; i < lst.size(); ++i)
                res = app(app(LADD, res), translate(lst[i]));
            return res;
        }
        if (head == "*") {
            if (lst.size() < 3) throw std::runtime_error("Operator * needs at least two operands");
            std::string res = translate(lst[1]);
            for (size_t i = 2; i < lst.size(); ++i)
                res = app(app(LMUL, res), translate(lst[i]));
            return res;
        }
        if (head == "lambda") {
            if (lst.size() < 3) throw std::runtime_error("Malformed lambda");

            std::string body = translate(lst[2]);

            if (lst[1]->is_atom) {
                // Single argument: (lambda x body)
                return "λ" + lst[1]->atom + "." + body;
            } else {
                // Multiple arguments: (lambda (x y z) body)
                for (auto it = lst[1]->list.rbegin(); it != lst[1]->list.rend(); ++it) {
                    if (!(*it)->is_atom)
                        throw std::runtime_error("lambda arg must be symbol");
                    body = "λ" + (*it)->atom + "." + body;
                }
                return body;
            }
        }
        if (head == "if") {
            if (lst.size() != 4)
                throw std::runtime_error("if requires 3 arguments");
            std::string cond = translate(lst[1]);
            std::string thenBr = translate(lst[2]);
            std::string elseBr = translate(lst[3]);
            return app(app(cond, thenBr), elseBr);
        }

        // generic application
        std::string res = translate(lst[0]);
        for (size_t i = 1; i < lst.size(); ++i) {
            res = app(res, translate(lst[i]));
        }
        return res;
    }
    std::string res = translate(lst[0]);
    for (size_t i = 1; i < lst.size(); ++i)
        res = app(res, translate(lst[i]));
    return res;
}

std::string translate(const std::shared_ptr<SExpr> &sexpr) {
    if (sexpr->is_atom) return translate_atom(sexpr->atom);
    return translate_list(sexpr->list);
}
