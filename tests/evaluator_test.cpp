#include "../lexer.h"
#include "../parser.h"
#include "../ast.h"
#include "../object.h"
#include "../evaluator.h"
#include "catch2/catch.hpp"
#include <string>
#include <tuple>
#include <vector>
using namespace std;
using obj::Object;
using ast::Program;

Object* evaluate_tests(const string& str)
{
    Lexer lexer(str);
    Parser parser(lexer);
    Program program(parser.parse_program());

    auto evaluated = evaluate(&program);

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