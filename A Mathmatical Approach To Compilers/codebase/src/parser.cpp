#include "parser.hpp"
#include <cctype>
#include <stdexcept>

// SExpr constructors
SExpr::SExpr(std::string a) : is_atom(true), atom(std::move(a)) {}
SExpr::SExpr(std::vector<std::shared_ptr<SExpr>> v) : is_atom(false), list(std::move(v)) {}

// Tokenizer implementation
Parser::Tokenizer::Tokenizer(std::string input) : s(std::move(input)), i(0) {}
void Parser::Tokenizer::skip_ws() {
    while (i < s.size() && std::isspace((unsigned char)s[i])) ++i;
}
Parser::Token Parser::Tokenizer::next() {
    skip_ws();
    if (i >= s.size()) return {Token::END, ""};
    char c = s[i];
    if (c == '(') { ++i; return {Token::LPAREN, "("}; }
    if (c == ')') { ++i; return {Token::RPAREN, ")"}; }
    if (std::isdigit((unsigned char)c) ||
        (c == '-' && i+1 < s.size() && std::isdigit((unsigned char)s[i+1]))) {
        size_t j = i;
        if (s[j] == '-') ++j;
        while (j < s.size() && std::isdigit((unsigned char)s[j])) ++j;
        std::string num = s.substr(i, j-i);
        i = j;
        return {Token::NUMBER, num};
    }
    size_t j = i;
    while (j < s.size() && !std::isspace((unsigned char)s[j]) && s[j] != '(' && s[j] != ')') ++j;
    std::string sym = s.substr(i, j-i);
    i = j;
    return {Token::SYMBOL, sym};
}

// Parser implementation
Parser::Parser(std::string in) : tz(std::move(in)) { cur = tz.next(); }
void Parser::consume() { cur = tz.next(); }

std::shared_ptr<SExpr> Parser::parse() {
    std::vector<std::shared_ptr<SExpr>> exprs;
    while (cur.kind != Token::END) exprs.push_back(parseOne());
    if (exprs.size() == 1) return exprs[0];
    return std::make_shared<SExpr>(exprs);
}

std::shared_ptr<SExpr> Parser::parseOne() {
    if (cur.kind == Token::LPAREN) {
        consume();
        std::vector<std::shared_ptr<SExpr>> elems;
        while (cur.kind != Token::RPAREN && cur.kind != Token::END) {
            elems.push_back(parseOne());
        }
        if (cur.kind == Token::RPAREN) consume();
        return std::make_shared<SExpr>(elems);
    }
    if (cur.kind == Token::NUMBER || cur.kind == Token::SYMBOL) {
        std::string t = cur.text;
        consume();
        return std::make_shared<SExpr>(t);
    }
    consume();
    return std::make_shared<SExpr>(std::string(""));
}
