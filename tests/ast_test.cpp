#include "../ast.h"
#include "../token.h"
#include <string>
#include <vector>
#include "catch2/catch.hpp"
using namespace std;

TEST_CASE("Let statement", "[ast]")
{
    string let = "variable";
    string var1 = "mi_var";
    string var2 = "otra_var";
    Program program(vector<Statement*>{ 
                        new LetStatement(
                            Token(TokenType::LET, let),
                            new Identifier(
                                Token(TokenType::IDENT, var1),
                                var1
                            ),
                            Identifier(
                                Token(TokenType::IDENT, var2),
                                var2
                            )
                        ) 
    });

    string program_str = program.to_string();

    REQUIRE(program_str == "variable mi_var = otra_var;");
}

TEST_CASE("Return statement", "[ast]")
{
    string return_value = "regresa";
    string expression = "100";
    Program program(vector<Statement*>{
                        new ReturnStatement(
                            Token(TokenType::RETURN, return_value),
                            Expression(
                                Token(TokenType::INT, expression)
                            )
                        )
    });

    string program_str = program.to_string();

    REQUIRE(program_str == "regresa 100;");
}

TEST_CASE("Expression statement", "[ast]")
{
    Program program(vector<Statement*>{
                        new ExpressionStatement{
                            Token(TokenType::IDENT, "foo", 3),
                            new Identifier(
                                Token(TokenType::IDENT, "foo", 3),
                                "foo"
                            )
                        },
                        new ExpressionStatement{
                            Token(TokenType::INT, "5"),
                            new Identifier(
                                Token(TokenType::INT, "5"),
                                "5"
                            )
                        }
    });

    string program_str = program.to_string();

    REQUIRE(program_str == "foo5");
}