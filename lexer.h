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

    void read_character();

public:
    explicit Lexer(const std::string&);
    Token next_token();
    
};

#endif // LEXER_H