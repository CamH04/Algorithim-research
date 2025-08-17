#include "parser.hpp"
#include <cctype>
#include <stdexcept>

// SExpr constructors
SExpr::SExpr(std::string a) : is_atom(true), atom(std::move(a)) {}
SExpr::SExpr(std::vector<std::shared_ptr<SExpr>> v) : is_atom(false), list(std::move(v)) {}

Parser::Tokenizer::Tokenizer(std::string input)
    : s(std::move(input)), i(0), _line(1), _col(1) {}

void Parser::Tokenizer::advance() {
    if (s[i] == '\n') {
        _line++;
        _col = 1;
    } else {
        _col++;
    }
    i++;
}

void Parser::Tokenizer::skip_ws() {
    while (i < s.size() && std::isspace((unsigned char)s[i])) {
        advance();
    }
}

Parser::Token Parser::Tokenizer::next() {
    skip_ws();
    if (i >= s.size()) return {Token::END, "", _line, _col};

    char c = s[i];
    int startLine = _line, startCol = _col;

    if (c == '(') { advance(); return {Token::LPAREN, "(", startLine, startCol}; }
    if (c == ')') { advance(); return {Token::RPAREN, ")", startLine, startCol}; }

    if (std::isdigit((unsigned char)c) ||
        (c == '-' && i+1 < s.size() && std::isdigit((unsigned char)s[i+1]))) {
        size_t j = i;
        if (s[j] == '-') advance();
        while (j < s.size() && std::isdigit((unsigned char)s[j])) { j++; advance(); }
        std::string num = s.substr(i, j - i);
        return {Token::NUMBER, num, startLine, startCol};
    }

    size_t j = i;
    while (j < s.size() && !std::isspace((unsigned char)s[j]) && s[j] != '(' && s[j] != ')') {
        j++; advance();
    }
    std::string sym = s.substr(i, j - i);
    return {Token::SYMBOL, sym, startLine, startCol};
}

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
        if (cur.kind == Token::END) {
            throw std::runtime_error("Unexpected end of input: missing ')' at line "
                                     + std::to_string(cur.line) + ", col " + std::to_string(cur.col));
        }
        consume(); // eat RPAREN
        return std::make_shared<SExpr>(elems);
    }

    if (cur.kind == Token::NUMBER || cur.kind == Token::SYMBOL) {
        std::string t = cur.text;
        consume();
        return std::make_shared<SExpr>(t);
    }

    if (cur.kind == Token::END) {
        throw std::runtime_error("Unexpected end of input at line "
                                 + std::to_string(cur.line) + ", col " + std::to_string(cur.col));
    }

    throw std::runtime_error("Unexpected token '" + cur.text + "' at line "
                             + std::to_string(cur.line) + ", col " + std::to_string(cur.col));
}
