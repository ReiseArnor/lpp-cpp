#ifndef TOKEN_H
#define TOKEN_H
#include <algorithm>
#include <ostream>
#include <string>
#include <array>
#include <fmt/core.h>
#include "utils.h"

enum class TokenType
{
    ASSIGN,
    COMMA,
    _EOF,
    FUNCTION,
    IDENT,
    ILLEGAL,
    INT,
    LBRACE,
    LET,
    LPAREN,
    PLUS,
    RBRACE,
    RPAREN,
    SEMICOLON,
    LT,
    GT,
    MINUS,
    DIVISION,
    MULTIPLICATION,
    NEGATION,
    _TRUE,
    _FALSE,
    _NULL,
    IF,
    ELSE,
    RETURN,
    EQ,
    NOT_EQ,
    STRING
};

static constexpr std::array<NameValuePair<TokenType>, 29> tokens_enums_strings {{
    {TokenType::ASSIGN, "ASSIGN"},
    {TokenType::COMMA, "COMMA\t"},
    {TokenType::_EOF, "EOF\t"},
    {TokenType::FUNCTION, "FUNCTION"},
    {TokenType::IDENT, "IDENT\t"},
    {TokenType::ILLEGAL, "ILLEGAL"},
    {TokenType::INT, "INT\t"},
    {TokenType::LBRACE, "LBRACE"},
    {TokenType::LET, "LET\t"},
    {TokenType::LPAREN, "LPAREN"},
    {TokenType::PLUS, "PLUS\t"},
    {TokenType::RBRACE, "RBRACE"},
    {TokenType::RPAREN, "RPAREN"},
    {TokenType::SEMICOLON, "SEMICOLON"},
    {TokenType::LT, "LT\t"},
    {TokenType::GT, "GT\t"},
    {TokenType::MINUS, "MINUS\t"},
    {TokenType::DIVISION, "DIVISION"},
    {TokenType::MULTIPLICATION, "MULTIPLICATION"},
    {TokenType::NEGATION, "NEGATION"},
    {TokenType::_TRUE, "TRUE\t"},
    {TokenType::_FALSE, "FALSE\t"},
    {TokenType::_NULL, "NULL\t"},
    {TokenType::IF, "IF\t"},
    {TokenType::ELSE, "ELSE\t"},
    {TokenType::RETURN, "RETURN"},
    {TokenType::EQ, "EQ\t"},
    {TokenType::NOT_EQ, "NOT_EQ"},
    {TokenType::STRING, "STRING"}
}};

class Token
{
public:
    std::string literal;
    TokenType token_type;
    int line;

    Token() = default;
    Token(const TokenType t, const char* l, const int line = 1, const std::size_t s = 1) : literal(l, s), token_type(t), line(line) {}
    Token(const TokenType t, const char* b, const char* e, const int line = 1) : literal(b, e), token_type(t), line(line) {}
    Token(const TokenType t, const std::string& s, const int line = 1) : literal(s), token_type(t), line(line) {}

    bool operator==(const Token& r) const noexcept
    {
        if (token_type != r.token_type || literal != r.literal)
            return false;
        return true;
    }

    bool operator!=(const Token& r) const noexcept
    {
        if(token_type != r.token_type || literal != r.literal)
            return true;
        return false;
    }

    friend std::ostream& operator<<(std::ostream& out, const Token& t)
    {
        out << fmt::format("Type: {} \tLiteral: {}\n", getNameForValue(tokens_enums_strings, t.token_type), t.literal);
        return out;
    }
};

#endif // TOKEN_H
