#include "../token.h"
#include "../lexer.h"
#include <string>
#include <vector>
#include "catch2/catch.hpp"
using namespace std;

TEST_CASE("Illegal lexers", "[lexer]")
{
    string str = "$@";
    Lexer lexer(str);
    vector<Token> tokens;
    for(auto s : str)
        tokens.push_back(lexer.next_token());

    vector<Token> expected_tokens { 
        Token(TokenType::ILLEGAL, "$"),
        Token(TokenType::ILLEGAL, "@")
    };

    REQUIRE(tokens == expected_tokens);
}

TEST_CASE("One character operator", "[lexer]")
{
    string str = "=+-/*!";
    Lexer lexer(str);
    vector<Token> tokens;
    for(auto s : str)
        tokens.push_back(lexer.next_token());

    vector<Token> expected_tokens {
        Token(TokenType::ASSIGN, "="),
        Token(TokenType::PLUS, "+"),
        Token(TokenType::MINUS, "-"),
        Token(TokenType::DIVISION, "/"),
        Token(TokenType::MULTIPLICATION, "*"),
        Token(TokenType::NEGATION, "!")
    };

    REQUIRE(tokens == expected_tokens);
}

TEST_CASE("EOF", "[lexer]")
{
    SECTION("with operator")
    {
        string str = "+-+";
        Lexer lexer(str);
        vector<Token> tokens;
        for(size_t i = 0; i <= 3; i++)
            tokens.push_back(lexer.next_token());

        vector<Token> expected_tokens {
            Token(TokenType::PLUS, "+"),
            Token(TokenType::MINUS, "-"),
            Token(TokenType::PLUS, "+"),
            Token(TokenType::_EOF, "\0")
        };

        REQUIRE(tokens == expected_tokens);
    }
    SECTION("with letter")
    {
        string str = "home";
        Lexer lexer(str);
        vector<Token> tokens;
        for(size_t i = 0; i <= 1; i++)
            tokens.push_back(lexer.next_token());

        vector<Token> expected_tokens {
            Token(TokenType::IDENT, "home", 1, 4),
            Token(TokenType::_EOF, "\0")
        };

        REQUIRE(tokens == expected_tokens);
    }
    SECTION("with number")
    {
        string str = "100";
        Lexer lexer(str);
        vector<Token> tokens;
        for(size_t i = 0; i <= 1; i++)
            tokens.push_back(lexer.next_token());

        vector<Token> expected_tokens {
            Token(TokenType::INT, "100", 1, 3),
            Token(TokenType::_EOF, "\0")
        };

        REQUIRE(tokens == expected_tokens);
    }
}

TEST_CASE("Delimiters", "[lexer]")
{
    string str = "(){},;";
    Lexer lexer(str);
    vector<Token> tokens;
    for(auto s : str)
        tokens.push_back(lexer.next_token());

    vector<Token> expected_tokens {
        Token(TokenType::LPAREN, "("),
        Token(TokenType::RPAREN, ")"),
        Token(TokenType::LBRACE, "{"),
        Token(TokenType::RBRACE, "}"),
        Token(TokenType::COMMA, ","),
        Token(TokenType::SEMICOLON, ";")
    };

    REQUIRE(tokens == expected_tokens);
}

TEST_CASE("Assignment", "[lexer]")
{
    string src = "variable cinco = 5;";
    Lexer lexer(src);
    vector<Token> tokens;
    for(size_t i = 0; i <= 4; i++)
        tokens.push_back(lexer.next_token());

    vector<Token> expected_tokens {
        Token(TokenType::LET, "variable", 1, 8),
        Token(TokenType::IDENT, "cinco", 1, 5),
        Token(TokenType::ASSIGN, "="),
        Token(TokenType::INT, "5"),
        Token(TokenType::SEMICOLON, ";")
    };

    REQUIRE(tokens == expected_tokens);
}

TEST_CASE("Function declaration", "[lexer]")
{
    string src {"variable suma = procedimiento(x, y) { x + y; };"};
    Lexer lexer(src);
    vector<Token> tokens;
    for(size_t i = 0; i <= 15; i++)
        tokens.push_back(lexer.next_token());

    vector<Token> expected_tokens {
        Token(TokenType::LET, "variable", 1, 8),
        Token(TokenType::IDENT, "suma", 1, 4),
        Token(TokenType::ASSIGN, "="),
        Token(TokenType::FUNCTION, "procedimiento", 1, 13),
        Token(TokenType::LPAREN, "("),
        Token(TokenType::IDENT, "x"),
        Token(TokenType::COMMA, ","),
        Token(TokenType::IDENT, "y"),
        Token(TokenType::RPAREN, ")"),
        Token(TokenType::LBRACE, "{"),
        Token(TokenType::IDENT, "x"),
        Token(TokenType::PLUS, "+"),
        Token(TokenType::IDENT, "y"),
        Token(TokenType::SEMICOLON, ";"),
        Token(TokenType::RBRACE, "}"),
        Token(TokenType::SEMICOLON, ";")
    };

    REQUIRE(tokens == expected_tokens);
}

TEST_CASE("Function call", "[lexer]")
{
    string src {"variable resultado = suma(dos, tres);"};
    Lexer lexer(src);
    vector<Token> tokens;
    for(size_t i = 0; i <= 9; i++)
        tokens.push_back(lexer.next_token());

    vector<Token> expected_tokens {
        Token(TokenType::LET, "variable", 1, 8),
        Token(TokenType::IDENT, "resultado", 1, 9),
        Token(TokenType::ASSIGN, "="),
        Token(TokenType::IDENT, "suma", 1, 4),
        Token(TokenType::LPAREN, "("),
        Token(TokenType::IDENT, "dos", 1, 3),
        Token(TokenType::COMMA, ","),
        Token(TokenType::IDENT, "tres", 1, 4),
        Token(TokenType::RPAREN, ")"),
        Token(TokenType::SEMICOLON, ";")
    };

    REQUIRE(tokens == expected_tokens);
}

TEST_CASE("Control statement", "[lexer]")
{
    string src = "si (5 < 10) { regresa verdadero; } si_no { regresa falso; }";
    Lexer lexer(src);
    vector<Token> tokens;
    for(size_t i = 0; i <= 16; i++)
        tokens.push_back(lexer.next_token());

    vector<Token> expected_tokens {
        Token(TokenType::IF, "si", 1, 2),
        Token(TokenType::LPAREN, "("),
        Token(TokenType::INT, "5"),
        Token(TokenType::LT, "<"),
        Token(TokenType::INT, "10", 1, 2),
        Token(TokenType::RPAREN, ")"),
        Token(TokenType::LBRACE, "{"),
        Token(TokenType::RETURN, "regresa", 1, 7),
        Token(TokenType::_TRUE, "verdadero", 1, 9),
        Token(TokenType::SEMICOLON, ";"),
        Token(TokenType::RBRACE, "}"),
        Token(TokenType::ELSE, "si_no", 1, 5),
        Token(TokenType::LBRACE, "{"),
        Token(TokenType::RETURN, "regresa", 1, 7),
        Token(TokenType::_FALSE, "falso", 1, 5),
        Token(TokenType::SEMICOLON, ";"),
        Token(TokenType::RBRACE, "}")
    };
    
    REQUIRE(tokens == expected_tokens);
}

TEST_CASE("Two character operator", "[lexer]")
{
    string src = "10 == 10; 10 != 9;";
    Lexer lexer(src);
    vector<Token> tokens;
    for(size_t i = 0; i <= 7; i++)
        tokens.push_back(lexer.next_token());

    vector<Token> expected_tokens {
        Token(TokenType::INT, "10", 1, 2),
        Token(TokenType::EQ, "==", 1, 2),
        Token(TokenType::INT, "10", 1, 2),
        Token(TokenType::SEMICOLON, ";"),
        Token(TokenType::INT, "10", 1, 2),
        Token(TokenType::NOT_EQ, "!=", 1, 2),
        Token(TokenType::INT, "9"),
        Token(TokenType::SEMICOLON, ";")
    };
    
    REQUIRE(tokens == expected_tokens);
}