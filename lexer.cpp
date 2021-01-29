#include "lexer.h"
#include "token.h"
#include <iostream>
#include <string>
#include <map>
using namespace std;

bool is_indentation(char);
bool is_number(char);
bool is_identifier(char);
bool skip_whitespace(char);

Lexer::Lexer(const string& src) : source(src) 
{
    current_char = ' ';
    read_position = 0;
    position = 0;
}

 void Lexer::read_character()
 // TODO switch to char pointers instead of ints
{
    if(read_position >= source.size())
        current_char = '\0';
    else
        current_char = source.at(read_position);

    position = read_position;
    read_position++;
}

 Token Lexer::next_token()
{
    read_character();
    while (skip_whitespace(current_char))
        read_character();

    switch (current_char) 
    {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
            return read_identifier();
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return read_number();
        case '=':
            if(peek_character() == '=') 
            {   read_position++;
                return Token { TokenType::EQ, "==", 2 }; } 
            return Token { TokenType::ASSIGN, &current_char };
        case '+':
            return Token { TokenType::PLUS, &current_char };
        case '-':
            return Token { TokenType::MINUS, &current_char };
        case '/':
            return Token { TokenType::DIVISION, &current_char };
        case '*':
            return Token { TokenType::MULTIPLICATION, &current_char };
        case '!':
            if(peek_character() == '=') 
            {   read_position++;
                return Token { TokenType::NOT_EQ, "!=", 2 }; }
            return Token { TokenType::NEGATION, &current_char };
        case '<':
            return Token { TokenType::LT, &current_char };
        case '>':
            return Token { TokenType::GT, &current_char };
        case '(':
            return Token { TokenType::LPAREN, &current_char };
        case ')':
            return Token { TokenType::RPAREN, &current_char };
        case '{':
            return Token { TokenType::LBRACE, &current_char };
        case '}':
            return Token { TokenType::RBRACE, &current_char };
        case ',':
            return Token { TokenType::COMMA, &current_char };
        case ';':
            return Token { TokenType::SEMICOLON, &current_char };
        case '\0':
            return Token { TokenType::_EOF, &current_char };
        default:
            return Token { TokenType::ILLEGAL, &current_char };
    }
}

Token Lexer::read_identifier()
{
    const char* begin = &source.at(position);
    const char* end;
    while (is_identifier(current_char)) read_character();
    if(current_char == '\0') {end = &source.at(position - 1); end++; }
    else end = &source.at(position);
    read_position = position;

    return keyword(string(begin, end));
}

Token Lexer::read_number()
{
    const char* begin = &source.at(position);
    const char* end;
    while (is_number(current_char)) read_character();
    if(current_char == '\0'){ end = &source.at(position - 1); end++; }
    else end = &source.at(position);
    read_position = position;

    return Token { TokenType::INT, begin, end };
}

Token Lexer::keyword(const string& s) const
{
    map<string, TokenType> keywords = {
        {"variable", TokenType::LET},
        {"procedimiento", TokenType::FUNCTION},
        {"regresa", TokenType::RETURN},
        {"si", TokenType::IF},
        {"si_no", TokenType::ELSE},
        {"verdadero", TokenType::_TRUE},
        {"falso", TokenType::_FALSE}
    };

    if(keywords.find(s) != keywords.end())
        return Token(keywords[s], s);
    
    return Token(TokenType::IDENT, s);
}

char Lexer::peek_character() const
{
    return read_position >= source.size() ? '\0' : source.at(read_position);
}

bool is_identifier(char c)
{
    switch (c) 
    {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '_':
            return true;
        default:
            return false;
    }
}

bool is_number(char c)
{
    switch (c) 
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return true;
        default:
            return false;
    }
}

bool skip_whitespace(char c)
{
    switch (c)
    {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            return true;
        default:
            return false;
    }
}