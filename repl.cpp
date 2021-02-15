#include "ast.h"
#include "lexer.h"
#include "object.h"
#include "parser.h"
#include "token.h"
#include "evaluator.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;
using obj::Environment;
using ast::Programs_Guard;

void print_parser_errors(const vector<string>& errors)
{
    for(auto e : errors)
        cout << e;
}

void start_repl()
{
    auto env = make_unique<Environment>();
    Programs_Guard guard;
    for (string s = ""; s != "salir()"; getline(cin, s)) 
    {
        Lexer lexer(s);
        Parser parser(lexer);
        auto program = guard.new_program(parser.parse_program());
        if(parser.errors().size() > 0)
        {
            print_parser_errors(parser.errors());
            cout << "\n>> ";
            continue;
        }

        auto evaluated = evaluate(program, env.get());

        if(evaluated != nullptr)
            cout << evaluated->inspect();
        cout << "\n>> ";
    }
}