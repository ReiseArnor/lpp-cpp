#include "../lexer.h"
#include "../ast.h"
#include "../parser.h"
#include <array>
#include <cstddef>
#include <string>
#include <vector>
#include "catch2/catch.hpp"
using namespace std;

void test_program_statements(Parser& parser, const Program& program, int expected_statement_count = 1)
{
    REQUIRE(parser.errors().size() == 0);
    REQUIRE(program.statements.size() == expected_statement_count);
}

void test_literal(Expression* expression, const string& expected_value)
{
    auto identifier = static_cast<Identifier*>(expression);
    REQUIRE(identifier->value == expected_value);
    REQUIRE(identifier->token_literal() == expected_value);
}

void test_literal(Expression* expression, const int expected_value) 
{
    auto integer = static_cast<Integer*>(expression); 
    REQUIRE(integer->value == expected_value); 
    REQUIRE(integer->token_literal() == to_string(expected_value));
}
/*
Can't parse expressions inside let statements yet!
TEST_CASE("Parse program", "[parser]")
{
    string str = "variable x =5;";
    Lexer lexer(str);
    Parser parser(lexer);
    Program expected_program{ vector<Statement*> {
                                new LetStatement(
                                    Token(TokenType::LET, "variable", 8),
                                    new Identifier(
                                        Token(TokenType::IDENT, "x"),
                                        "x"),
                                    Expression(Token(TokenType::INT, "5"))
                                )
    }};

    REQUIRE(expected_program == parser.parse_program());
}
*/
TEST_CASE("Let statements", "[parser]")
{
    string str = "variable x = 5; variable y = 10; variable foo = 20;";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());

    REQUIRE(program.statements.size() == 3);

    for(auto s : program.statements)
        CHECK(s->token_literal() == "variable");
    
}

TEST_CASE("Identifiers", "[parser]")
{
    string str = "variable uno = 1; variable dies = 10; variable cien = 100;";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());

    const array<string, 3> names {"uno", "dies", "cien"};

    for(size_t i = 0; i < 3; i++)
        CHECK(static_cast<LetStatement*>(program.statements.at(i))->name->value == names.at(i));
}

TEST_CASE("Parse errors", "[parser]")
{
    string str = "variable x 5;";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());

    REQUIRE(parser.errors().size() == 1);
}

TEST_CASE("Return statement", "[parser]")
{
    string str = "regresa 5; regresa foo;";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());

    CHECK(program.statements.size() == 2);
    for(auto& s : program.statements)
        CHECK(s->token_literal() == "regresa");
}

TEST_CASE("Identifier expression", "parser")
{
    string str = "foobar;";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());

    auto expression_statement = static_cast<ExpressionStatement*>(program.statements.at(0));

    REQUIRE(expression_statement != nullptr);

    test_literal(expression_statement->expression, "foobar");
}

TEST_CASE("Integer expression", "[parser]")
{
    string str = "5;";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());

    test_program_statements(parser, program);

    auto expression_statement = static_cast<ExpressionStatement*>(program.statements.at(0));
    
    test_literal(expression_statement->expression, 5);
}