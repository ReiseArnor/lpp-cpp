#include "../token.h"
#include "../lexer.h"
#include <string>
#include <vector>
#define CATCH_CONFIG_MAIN
#include "../catch2/catch.hpp"

TEST_CASE("Illegal lexers", "[lexer]")
{
    std::string str = "!?@";
    Lexer lexer(str);
    std::vector<Token> tokens;
    for(auto s : str)
        tokens.push_back(lexer.next_token());

    std::vector<Token> expected_tokens { 
        Token(TokenType::ILLEGAL, '!'),
        Token(TokenType::ILLEGAL, '?'),
        Token(TokenType::ILLEGAL, '@')
    };

    REQUIRE(tokens == expected_tokens);
}