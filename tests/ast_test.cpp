#include "../ast.h"
#include "../token.h"
#include <string>
#include <vector>
#include "../catch2/catch.hpp"
using namespace std;

TEST_CASE("Let statement", "[ast]")
{
    string let = "variable";
    string var1 = "mi_var";
    string var2 = "otra_var";
    Program program = Program(vector<Statement*>{ 
                        new LetStatement(
                            Token(TokenType::LET, let),
                            Identifier(
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
    Program program = Program(vector<Statement*>{
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