#ifndef TOKEN_H
#define TOKEN_H

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
    SEMICOLON
};

class Token
{
    char literal;
    TokenType token_type;
public:
    Token() = default;
    Token(TokenType t, char l)
    { 
        token_type = t;
        literal = l;
    }
    bool operator==(const Token& r) const noexcept
    {   
        if (token_type != r.token_type || literal != r.literal)
            return false;

        return true;
    }
};

#endif // TOKEN_H