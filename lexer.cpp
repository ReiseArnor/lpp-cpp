#include "lexer.h"
#include "token.h"
using namespace std;

Lexer::Lexer(const std::string& src) : source(src) 
{
    current_char = 0;
    read_position = 0;
    position = 0;
}

 void Lexer::read_character()
{
    if(read_position >= source.size())
        current_char = 0;
    else
        current_char = source.at(read_position);
    
    position = read_position;
    read_position++;
}

 Token Lexer::next_token()
{
    Token token;
    read_character();
    switch (current_char) 
    {
        case '=':
            token = { TokenType::ASSIGN, current_char };
            break;

        case '+':
            token = { TokenType::PLUS, current_char };
            break;

        case '(':
            token = { TokenType::LPAREN, current_char };
            break;

        case ')':
            token = { TokenType::RPAREN, current_char };
            break;

        case '{':
            token = { TokenType::LBRACE, current_char };
            break;

        case '}':
            token = { TokenType::RBRACE, current_char };
            break;

        case ',':
            token = { TokenType::COMMA, current_char };
            break;

        case ';':
            token = { TokenType::SEMICOLON, current_char };
            break;

        case 0:
            token = { TokenType::_EOF, current_char };
            break;

        default:
            token = { TokenType::ILLEGAL, current_char };
            break;
    }
    
    return token;
}