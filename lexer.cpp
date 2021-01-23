#include "lexer.h"
using namespace std;

Lexer::Lexer(const std::string& src) : source(src) 
{
    current_char = ' ';
    read_position = 0;
    position = 0;
}

 void Lexer::read_character()
{
    if(read_position >= source.size())
        current_char = ' ';
    else
        current_char = source.at(read_position);
    
    position = read_position;
    read_position++;
}

 Token Lexer::next_token()
{
    read_character();
    Token token { TokenType::ILLEGAL, current_char };
    return token;
}