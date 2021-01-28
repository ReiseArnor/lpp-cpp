#ifndef TOKEN_H
#define TOKEN_H
#include <string>

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
    IF,
    ELSE,
    RETURN,
    EQ,
    NOT_EQ
};

class Token
{
public:
    std::string literal;
    TokenType token_type;

    Token() = default;
    Token(TokenType t, const char* l, std::size_t s = 1) : literal(l, s), token_type(t) {}
    Token(TokenType t, const char* b, const char* e) : literal(b, e), token_type(t) {}
    Token(TokenType t, const std::string& s) : literal(s), token_type(t) {}

    bool operator==(const Token& r) const noexcept
    {   
        if (token_type != r.token_type || literal != r.literal)
            return false;

        return true;
    }
};

#endif // TOKEN_H