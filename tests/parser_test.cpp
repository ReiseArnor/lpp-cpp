#include "../lexer.h"
#include "../ast.h"
#include "../parser.h"
#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include "catch2/catch.hpp"
using namespace std;

void test_program_statements(Parser& parser, const Program& program, int expected_statement_count = 1)
{
    for(auto& e : parser.errors())
        cout << e;
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

template<typename T>
void test_infix_expression(
    Expression* expression,
    const T& expected_left, 
    const string& expected_operator,
    const T& expected_right
    )
{
    auto infix = static_cast<Infix*>(expression);
    test_literal(infix->left, expected_left);
    REQUIRE(infix->operatr == expected_operator);
    test_literal(infix->right, expected_right);
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

TEST_CASE("Prefix expression", "[parser]")
{
    string str = "!5; -15;";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());

    test_program_statements(parser, program, 2);
    
    const array<const string, 2> op { "!", "-" };
    const array<const string, 2> right_expression { "5", "15" };
    
    for(size_t i = 0; i < 2; i++)
    {
        auto expression_statement = static_cast<ExpressionStatement*>(program.statements.at(i));
        auto prefix_expression = static_cast<Prefix*>(expression_statement->expression);

        REQUIRE(prefix_expression->token_literal() == op.at(i));
        REQUIRE(prefix_expression->right->token_literal() == right_expression.at(i));
    }
}

TEST_CASE("Infix expression", "[parser]")
{
    string str = "  5 + 5; 5 - 5; 5 * 5; 5 / 5; 5 > 5; 5 < 5; 5 == 5; 5 != 5;";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());

    test_program_statements(parser, program, 8);

    const vector<tuple<int,string,int>> expected_operators_and_values{
        {5, "+", 5},
        {5, "-", 5},
        {5, "*", 5},
        {5, "/", 5},
        {5, ">", 5},
        {5, "<", 5},
        {5, "==", 5},
        {5, "!=", 5}
    };

    for(size_t i = 0; i < 7; i++)
    {
        auto expression_statement = static_cast<ExpressionStatement*>(program.statements.at(i));
        auto tuple = expected_operators_and_values.at(i);

        test_infix_expression(
            expression_statement->expression,
            std::get<0>(tuple),
            std::get<1>(tuple),
            std::get<2>(tuple));
    }
}