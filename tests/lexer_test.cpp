#include "../token.h"
#include "../lexer.h"
#include <string>
#include <vector>
#define CATCH_CONFIG_MAIN
#include "../catch2/catch.hpp"
using namespace std;

TEST_CASE("Illegal lexers", "[lexer]")
{
    string str = "!?@";
    Lexer lexer(str);
    vector<Token> tokens;
    for(auto s : str)
        tokens.push_back(lexer.next_token());

    vector<Token> expected_tokens { 
        Token(TokenType::ILLEGAL, '!'),
        Token(TokenType::ILLEGAL, '?'),
        Token(TokenType::ILLEGAL, '@')
    };

    REQUIRE(tokens == expected_tokens);
}

TEST_CASE("One character operator", "[lexer]")
{
    string str = "=+";
    Lexer lexer(str);
    vector<Token> tokens;
    for(auto s : str)
        tokens.push_back(lexer.next_token());

    vector<Token> expected_tokens {
        Token(TokenType::ASSIGN, '='),
        Token(TokenType::PLUS, '+')
    };

    REQUIRE(tokens == expected_tokens);
}

TEST_CASE("EOF", "[lexer]")
{
    string str = "+";
    Lexer lexer(str);
    vector<Token> tokens;
    for(size_t i = 0; i <= str.size(); i++)
        tokens.push_back(lexer.next_token());

    vector<Token> expected_tokens {
        Token(TokenType::PLUS, '+'),
        Token(TokenType::_EOF, 0)
    };

    REQUIRE(tokens == expected_tokens);
}

TEST_CASE("Delimiters", "[lexer]")
{
    string str = "(){},;";
    Lexer lexer(str);
    vector<Token> tokens;
    for(auto s : str)
        tokens.push_back(lexer.next_token());

    vector<Token> expected_tokens {
        Token(TokenType::LPAREN, '('),
        Token(TokenType::RPAREN, ')'),
        Token(TokenType::LBRACE, '{'),
        Token(TokenType::RBRACE, '}'),
        Token(TokenType::COMMA, ','),
        Token(TokenType::SEMICOLON, ';')
    };

    REQUIRE(tokens == expected_tokens);
}