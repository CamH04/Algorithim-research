#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <memory>

// S-expression (Representing numbers as higher order functions)
struct SExpr {
    bool is_atom;
    std::string atom;
    std::vector<std::shared_ptr<SExpr>> list;
    SExpr(std::string a);
    SExpr(std::vector<std::shared_ptr<SExpr>> v);
};

class Parser {
public:
    Parser(std::string in);
    std::shared_ptr<SExpr> parse();
private:
    struct Token {
        enum Kind { LPAREN, RPAREN, NUMBER, SYMBOL, END } kind;
        std::string text;
    };
    class Tokenizer {
    public:
        Tokenizer(std::string input);
        Token next();
    private:
        std::string s;
        size_t i;
        void skip_ws();
    };
    Token cur;
    Tokenizer tz;
    void consume();
    std::shared_ptr<SExpr> parseOne();
};

#endif
