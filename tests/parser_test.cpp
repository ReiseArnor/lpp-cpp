#include "../lexer.h"
#include "../ast.h"
#include "../parser.h"
#include <array>
#include <cstddef>
#include <string>
#include "../catch2/catch.hpp"
using namespace std;

TEST_CASE("Parse program", "[parser]")
{
    string str = "variable x =5;";
    Lexer lexer(str);
    Parser parser(lexer);
    
    CHECK_NOTHROW(parser.parse_program());
}

TEST_CASE("Let statements", "[parser]")
{
    string str = "variable x = 5; variable y = 10; variable foo = 20;";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program = parser.parse_program();

    REQUIRE(program.statements.size() == 3);

    for(auto s : program.statements)
        CHECK(s->token_literal() == "variable");
    
}

TEST_CASE("Identifiers", "[parser]")
{
    string str = "variable uno = 1; variable dies = 10; variable cien = 100;";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program = parser.parse_program();

    const array<string, 3> names {"uno", "dies", "cien"};

    for(size_t i = 0; i < 3; i++)
        CHECK(static_cast<LetStatement*>(program.statements.at(i))->name.value == names.at(i));
}

TEST_CASE("Parse errors", "[parser]")
{
    string str = "variable x 5;";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program = parser.parse_program();

    REQUIRE(parser.errors().size() == 1);
}

TEST_CASE("Return statement", "[parser]")
{
    string str = "regresa 5; regresa foo;";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program = parser.parse_program();

    CHECK(program.statements.size() == 2);
    for(auto& s : program.statements)
        CHECK(s->token_literal() == "regresa");
}