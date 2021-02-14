#include "../lexer.h"
#include "../parser.h"
#include "../ast.h"
#include "../object.h"
#include "../evaluator.h"
#include "catch2/catch.hpp"
#include <memory>
#include <string>
#include <tuple>
#include <vector>
using namespace std;
using obj::Object;
using ast::Program;

Object* evaluate_tests(const string& str, Environment* env = nullptr)
{
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());
    Object* evaluated = nullptr;
    if(!env)
    {
        auto temp_env = make_unique<Environment>();
        evaluated = evaluate(&program, temp_env.get());
    }
    else
        evaluated = evaluate(&program, env);

    REQUIRE(evaluated != nullptr);

    return evaluated;
}

void test_object(Object* evaluated, const int expected)
{
    auto eval = static_cast<obj::Integer*>(evaluated);
    REQUIRE(eval->value == expected);
}

void test_object(Object* evaluated, const bool expected)
{
    auto eval = static_cast<obj::Boolean*>(evaluated);
    REQUIRE(eval->value == expected);
}

void test_object(Object* evaluated, const char* expected)
{
    auto eval = static_cast<obj::Error*>(evaluated);
    REQUIRE(eval->message == expected);
}

void test_object(Object* evaluated)
{
    REQUIRE(evaluated == _NULL.get());
}

template<typename T>
void eval_and_test_objects(const vector<tuple<string, T>>& tests)
{
    for(auto& t : tests)
    {
        auto evaluated = evaluate_tests(get<0>(t));
        test_object(evaluated, get<1>(t));
    }
}

TEST_CASE("Integer evaluation", "[evaluator]")
{
    vector<tuple<string, int>> tests {
        {"5", 5},
        {"10", 10},
        {"-5", -5},
        {"-10", -10},
        {"5 + 5", 10},
        {"5 - 10", -5},
        {"2 * 2 * 2 * 2", 16},
        {"2 * 5 - 3", 7},
        {"50 / 2", 25},
        {"2 * (5 - 3)", 4},
        {"(2 + 7) / 3", 3},
        {"50 / 2 * 2 + 10", 60},
        {"5 / 2", 2}
    };

    eval_and_test_objects(tests);
}

TEST_CASE("Boolean evaluation", "[evaluator]")
{
    vector<tuple<string, bool>> tests {
        {"verdadero", true},
        {"falso", false},
        {"1 < 2", true},
        {"1 > 2", false},
        {"1 < 1", false},
        {"1 > 1", false},
        {"1 == 1", true},
        {"1 != 1", false},
        {"1 != 2", true},
        {"verdadero == verdadero", true},
        {"falso == falso", true},
        {"verdadero == falso", false},
        {"verdadero != falso", true},
        {"(1 < 2) == verdadero", true},
        {"(1 < 2) == falso", false},
        {"(1 > 2) == verdadero", false},
        {"(1 > 2) == falso", true}
    };

    eval_and_test_objects(tests);
}

TEST_CASE("Bang operator", "[evaluator]")
{
    vector<tuple<string, bool>> tests {
        {"!verdadero", false},
        {"!falso", true},
        {"!!verdadero", true},
        {"!!falso", false},
        {"!5", false},
        {"!!5", true}
    };

    eval_and_test_objects(tests);
}

TEST_CASE("If else evaluation")
{
    int dies = 10;
    int veinte = 20;
    vector<tuple<string, int*>> tests {
        {"si (verdadero) { 10 }", &dies},
        {"si (falso) { 10 }", nullptr},
        {"si (1) { 10 }", &dies},
        {"si (1 < 2) { 10 }", &dies},
        {"si (1 > 2) { 10 }", nullptr},
        {"si (1 < 2) { 10 } si_no { 20 }", &dies},
        {"si (1 > 2) { 10 } si_no { 20 }", &veinte}
    };

    for(auto& t : tests)
    {
        auto evaluated = evaluate_tests(get<0>(t));

        if(get<1>(t) == nullptr)
            test_object(evaluated);
        else
            test_object(evaluated, *get<1>(t));
    }
}

TEST_CASE("Return evaluation")
{
    vector<tuple<string, int>> tests {
        {"regresa 10;", 10},
        {"regresa 10; 9;", 10},
        {"regresa 2 * 5; 9;", 10},
        {"9; regresa 3 * 6; 9;", 18},
        {"                          \
            si (10 > 1) {           \
                si (20 > 10) {      \
                    regresa 1;      \
                }                   \
                                    \
                regresa 0;          \
            }                       \
        ", 1}
    };

    eval_and_test_objects(tests);
}

TEST_CASE("Error handling")
{
    vector<tuple<string,const char*>> tests {
        {"5 + verdadero", "Discrepancia de tipos: INTEGER + BOOLEAN cerca de la línea 1"},
        {"5 + verdadero; 9;", "Discrepancia de tipos: INTEGER + BOOLEAN cerca de la línea 1"},
        {"-verdadero", "Operador desconocido: -BOOLEAN cerca de la línea 1"},
        {"1; verdadero + falso", "Operador desconocido: BOOLEAN + BOOLEAN cerca de la línea 1"},
        {"5; verdadero - falso; 10;", "Operador desconocido: BOOLEAN - BOOLEAN cerca de la línea 1"},
        {"                                          \
            si (10 > 7) {\n                         \
                regresa verdadero + falso;\n        \
            }",
        "Operador desconocido: BOOLEAN + BOOLEAN cerca de la línea 2"
        },
        {"                                          \
            si (10 > 1) {\n                         \
                si (verdadero) {\n                  \
                    regresa verdadero * falso;\n    \
                }\n                                 \
                regresa 1;\n                        \
            }",
        "Operador desconocido: BOOLEAN * BOOLEAN cerca de la línea 3"
        },
        {"                                          \
            si (5 < 2) {\n                          \
                regresa 1;\n                        \
            } si_no {\n                             \
                regresa verdadero / falso;\n        \
            }",
        "Operador desconocido: BOOLEAN / BOOLEAN cerca de la línea 4"
        },
        {"foobar;", "Identificador sin definir: \"foobar\" cerca de la línea 1"}
    };

    eval_and_test_objects(tests);
}

TEST_CASE("Assignment evaluation")
{
    vector<tuple<string,int>> tests {
        {"variable a = 5; a;", 5},
        {"variable a = 5 * 5; a;", 25},
        {"variable a = 10; variable b = a; b;", 10},
        {"variable a = 5; variable b = a; variable c = a + b + 5; c;", 15}
    };

    auto env = new Environment();
    for(auto& t : tests)
    {
        auto evaluated = evaluate_tests(get<0>(t), env);
        test_object(evaluated, get<1>(t));
    }
}