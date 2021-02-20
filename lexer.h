#ifndef LEXER_H
#define LEXER_H
#include "token.h"
#include <string>

class Lexer
{
private:
    const std::string source;
    char current_char;
    int read_position;
    int position;
    int line;

    void read_character();
    Token keyword(const std::string&) const;
    Token read_string(bool = false);
    Token read_identifier();
    Token read_number();
    char peek_character() const;

public:
    explicit Lexer(const std::string&);
    Token next_token();
    
};

#endif // LEXER_H