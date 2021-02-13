#ifndef TOKEN_H
#define TOKEN_H
#include <algorithm>
#include <ostream>
#include <string>
#include <array>

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

template<class T>
struct NameValuePair {
    using value_type = T;
    const T value;
    const char* const name;
};

template<class Mapping, class V>
std::string getNameForValue(Mapping a, V value) {
    auto pos = std::find_if(std::begin(a), std::end(a), [&value](const typename Mapping::value_type& t){
        return (t.value == value);
    });
    return pos->name;
}

const std::array<const NameValuePair<TokenType>, 27> tokens_enums_strings {{
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
    {TokenType::IF, "IF\t"},
    {TokenType::ELSE, "ELSE\t"},
    {TokenType::RETURN, "RETURN"},
    {TokenType::EQ, "EQ\t"},
    {TokenType::NOT_EQ, "NOT_EQ"}
}};

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

    bool operator!=(const Token& r) const noexcept
    {
        if(token_type != r.token_type || literal != r.literal)
            return true;
        return false;
    }

    friend std::ostream& operator<<(std::ostream& out, const Token& t)
    {
        out << "Type: " << getNameForValue(tokens_enums_strings, t.token_type) << "\tLiteral: " << t.literal << "\n";
        return out;
    }
};

#endif // TOKEN_H