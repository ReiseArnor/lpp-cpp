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

void test_object(Object* evaluated, int expected)
{
    auto eval = static_cast<obj::Integer*>(evaluated);
    REQUIRE(eval->value == expected);
}

TEST_CASE("Integer evaluation", "[evaluator]")
{
    vector<tuple<string, int>> tests {
        {"5", 5},
        {"10", 10}
    };

    for(auto& t : tests)
    {
        auto evaluated = evaluate_tests(get<0>(t));
        test_object(evaluated, get<1>(t));
    }
}

