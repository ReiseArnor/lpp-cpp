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

void test_literal(Expression* expression, const char* expected_value)
{
    auto identifier = static_cast<Identifier*>(expression);
    CHECK(identifier->value == expected_value);
    CHECK(identifier->token_literal() == expected_value);
}

void test_literal(Expression* expression, const int expected_value)
{
    auto integer = static_cast<Integer*>(expression); 
    CHECK(integer->value == expected_value); 
    CHECK(integer->token_literal() == to_string(expected_value));
}

void test_literal(Expression* expression, const bool expected_value)
{
    auto boolean = static_cast<Boolean*>(expression); 
    CHECK(boolean->value == expected_value);
    CHECK(boolean->token_literal() == (expected_value ? "verdadero" : "falso"));
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


TEST_CASE("Parse program", "[parser]")
{
    string str = "variable x =5;";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());
    Program expected_program{ vector<Statement*> {
                                new LetStatement(
                                    Token(TokenType::LET, "variable", 8),
                                    new Identifier(
                                        Token(TokenType::IDENT, "x"),
                                        "x"),
                                    new Expression(Token(TokenType::INT, "5"))
                                )
    }};

    REQUIRE(program == expected_program);
}

TEST_CASE("Let statements", "[parser]")
{
    string str = "variable x = 5; variable y = 10; variable foo = 20; variable bar = verdadero";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());

    REQUIRE(program.statements.size() == 4);
    auto expected_operators_and_values = vector<tuple<const char*,int>>{ {"x", 5}, {"y", 10}, {"foo", 20}, {"bar", true}};
    
    for(size_t i = 0; i < 4; i++)
    {
        REQUIRE(program.statements.at(i)->token_literal() == "variable");
        auto let_statement = static_cast<LetStatement*>(program.statements.at(i));

        test_literal(let_statement->name, get<0>(expected_operators_and_values.at(i)));

        i < 3 ? test_literal(let_statement->value, get<1>(expected_operators_and_values.at(i))) // for the ints
            :   test_literal(let_statement->value, (bool)get<1>(expected_operators_and_values.at(i))); // for the bool
    }
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
    string str = "regresa 5; regresa foo; regresa verdadero; regresa falso;";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());

    REQUIRE(program.statements.size() == 4);
    
    auto statements = vector<ReturnStatement*>();
    for(size_t i = 0; i < 4; i++)
    {
        REQUIRE(program.statements.at(i)->token_literal() == "regresa");
        statements.push_back(static_cast<ReturnStatement*>(program.statements.at(i)));
    }

    test_literal(statements.at(0)->return_value, 5);
    test_literal(statements.at(1)->return_value, "foo");
    test_literal(statements.at(2)->return_value, true);
    test_literal(statements.at(3)->return_value, false);
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
    string str = "!5; -15; !verdadero;";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());

    test_program_statements(parser, program, 3);
    
    const array<const string, 3> op { "!", "-", "!" };
    const array<const string, 3> right_expression { "5", "15", "verdadero" };
    
    for(size_t i = 0; i < 3; i++)
    {
        auto expression_statement = static_cast<ExpressionStatement*>(program.statements.at(i));
        auto prefix_expression = static_cast<Prefix*>(expression_statement->expression);

        REQUIRE(prefix_expression->token_literal() == op.at(i));
        REQUIRE(prefix_expression->right->token_literal() == right_expression.at(i));
    }
}

TEST_CASE("Infix expression", "[parser]")
{
    string str = "  5 + 5;\
                    5 - 5;\
                    5 * 5;\
                    5 / 5;\
                    5 > 5;\
                    5 < 5;\
                    5 == 5;\
                    5 != 5;\
                    verdadero == verdadero;\
                    falso == falso;\
                    verdadero != falso;";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());

    test_program_statements(parser, program, 11);

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

    const vector<tuple<bool,string,bool>> expected_operators_and_booleans{
        {true, "==", true},
        {false, "==", false},
        {true, "!=", false}
    };

    for(size_t i = 0; i < 8; i++)
    {
        auto expression_statement = static_cast<ExpressionStatement*>(program.statements.at(i));
        auto tuple = expected_operators_and_values.at(i);

        test_infix_expression(
            expression_statement->expression,
            get<0>(tuple),
            get<1>(tuple),
            get<2>(tuple));
    }

    size_t tuple_count = 0;
    for(size_t i = 8; i < 11; i++)
    {
        auto expression_statement = static_cast<ExpressionStatement*>(program.statements.at(i));
        auto tuple = expected_operators_and_booleans.at(tuple_count);

        test_infix_expression(
            expression_statement->expression,
            get<0>(tuple),
            get<1>(tuple),
            get<2>(tuple));
        tuple_count++;
    }
}

TEST_CASE("Boolean expression", "[parser]")
{
    string str = "verdadero; falso;";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());

    test_program_statements(parser, program, 2);

    const array<bool, 2> expected_values { true, false };
    
    for(size_t i = 0; i < 2; i++)
    {
        auto expression_statement =static_cast<ExpressionStatement*>(program.statements.at(i));

        test_literal(expression_statement->expression, expected_values[i]);
    }
}

TEST_CASE("Operator precedence", "[parser]")
{
    const vector<tuple<string, string, int>> test_sources = {
        {"-a * b;", "((-a) * b)", 1},
        {"!-a;", "(!(-a))", 1},
        {"a + b / c;", "(a + (b / c))", 1},
        {"3 + 4; -5 * 5;", "(3 + 4)((-5) * 5)", 2},
        {"2 / 2 + 1 * 1;", "((2 / 2) + (1 * 1))", 1},
        {"2 * 2 / 2 * 2;", "(((2 * 2) / 2) * 2)", 1},
        {"2 / 2 * 2 / 2;", "(((2 / 2) * 2) / 2)", 1},
        {"!5;","(!5)", 1},
        {"-i * a / 5 + -7;", "((((-i) * a) / 5) + (-7))", 1},
        {"verdadero;", "verdadero", 1},
        {"falso;", "falso", 1},
        {"3 > 5 == verdadero;", "((3 > 5) == verdadero)", 1},
        {"3 < 5 == falso;", "((3 < 5) == falso)", 1},
        {"1 + (2 + 3) + 4;", "((1 + (2 + 3)) + 4)", 1},
        {"(5 + 5) * 2;", "((5 + 5) * 2)", 1},
        {"2 / (5 + 5);", "(2 / (5 + 5))", 1},
        {"-(5 + 5);", "(-(5 + 5))", 1},
        {"a + suma(b * c) + d;", "((a + suma((b * c))) + d)", 1},
        {"suma(a, b, 1, 2 * 3, 4 + 5, suma(6, 7 * 8));",
            "suma(a, b, 1, (2 * 3), (4 + 5), suma(6, (7 * 8)))", 1},
        {"suma(a + b + c * d / f + g);", "suma((((a + b) + ((c * d) / f)) + g))", 1}
    };

    for(size_t i = 0; i < test_sources.size(); i++)
    {
        auto tuple = test_sources.at(i);
        Lexer lexer(get<0>(tuple));
        Parser parser(lexer);
        Program program(parser.parse_program());

        test_program_statements(parser, program, get<2>(tuple));
        REQUIRE(program.to_string() == get<1>(tuple));
    }
}

TEST_CASE("If statement", "[parser]")
{
    string str = "si (x < y) { z }";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());

    test_program_statements(parser, program);

    auto if_expression = static_cast<If*>(static_cast<ExpressionStatement*>(program.statements.at(0))->expression);

    test_infix_expression(if_expression->condition, "x", "<", "y");

    REQUIRE(if_expression->consequence->statements.size() == 1);

    auto consequence_statement = static_cast<ExpressionStatement*>(if_expression->consequence->statements.at(0));

    test_literal(consequence_statement->expression, "z");

    REQUIRE(if_expression->alternative == nullptr);
}


TEST_CASE("Else statement", "[parser]")
{
    string str = "si (x < y) { z } si_no { w }";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());

    test_program_statements(parser, program);

    auto if_expression = static_cast<If*>(static_cast<ExpressionStatement*>(program.statements.at(0))->expression);
    test_infix_expression(if_expression->condition, "x", "<", "y");

    REQUIRE(if_expression->consequence->statements.size() == 1);
    auto consequence_statement = static_cast<ExpressionStatement*>(if_expression->consequence->statements.at(0));
    test_literal(consequence_statement->expression, "z");

    REQUIRE(if_expression->alternative->statements.size() == 1);
    auto alternative_statement = static_cast<ExpressionStatement*>(if_expression->alternative->statements.at(0));
    test_literal(alternative_statement->expression, "w");
}

TEST_CASE("Funtion literal", "[parser]")
{
    string str = "procedimiento(x, y) { x + y}";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());

    test_program_statements(parser, program);

    auto function_literal = static_cast<Function*>(static_cast<ExpressionStatement*>(program.statements.at(0))->expression);

    REQUIRE(function_literal->parameters.size() == 2);
    test_literal(function_literal->parameters.at(0), "x");
    test_literal(function_literal->parameters.at(1), "y");

    REQUIRE(function_literal->body != nullptr);
    REQUIRE(function_literal->body->statements.size() == 1);
    
    auto body = static_cast<ExpressionStatement*>(function_literal->body->statements.at(0));
    test_infix_expression(body->expression, "x", "+", "y");
}

TEST_CASE("Function parameters", "[parser]")
{
    array<map<string, vector<const char*>>, 3> tests_array;
    tests_array.at(0)["input"] = {"procedimiento() {}"};
    tests_array.at(0)["expected_params"] = {};
    tests_array.at(1)["input"] = {"procedimiento(x) {}"};
    tests_array.at(1)["expected_params"] = {"x"};
    tests_array.at(2)["input"] = {"procedimiento(x, y, z) {}"};
    tests_array.at(2)["expected_params"] = {"x", "y", "z"};

    for(auto& test : tests_array)
    {
        Lexer lexer(test["input"].at(0));
        Parser parser(lexer);
        Program program(parser.parse_program());

        auto funcion = static_cast<Function*>(static_cast<ExpressionStatement*>(program.statements.at(0))->expression);

        for (size_t i = 0; i < test["expected_params"].size(); i++) 
        {
            test_literal(funcion->parameters.at(i), test["expected_params"].at(i));
        }
    }

}

TEST_CASE("Call expression", "[parser]")
{
    string str = "suma(1, 2 * 3, 4 + 5);";
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());

    test_program_statements(parser, program);

    auto call = static_cast<Call*>(static_cast<ExpressionStatement*>(program.statements.at(0))->expression);
    test_literal(call->function, "suma");

    REQUIRE(call->arguments.size() == 3);
    test_literal(call->arguments.at(0), 1);
    test_infix_expression(call->arguments.at(1), 2, "*", 3);
    test_infix_expression(call->arguments.at(2), 4, "+", 5);
}