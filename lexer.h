#ifndef LEXER_H
#define LEXER_H
#include "token.h"
#include <cstddef>
#include <string>

class Lexer
{
private:
    const std::string source;
    char current_char;
    std::size_t read_position;
    std::size_t position;
    int line;

    void read_character();
    Token keyword(const std::string&) const;
    Token read_string(char);
    Token read_identifier();
    Token read_number();
    char peek_character() const;

public:
    explicit Lexer(const std::string&);
    Token next_token();
    
};

#endif // LEXER_H
