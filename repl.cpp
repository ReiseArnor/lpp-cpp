#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

void print_parser_errors(const vector<string>& errors)
{
    for(auto e : errors)
        cout << e;
}

void start_repl()
{
    for (string s = ""; s != "salir()"; getline(cin, s)) 
    {
        Lexer lexer(s);
        Parser parser(lexer);
        Program program(parser.parse_program());

        if(parser.errors().size() > 0)
        {
            print_parser_errors(parser.errors());
            continue;
        }

        cout << program.to_string() << "\n>> " ;
    }
}