#ifndef PARSER_H
#define PARSER_H
#include "ast.h"
#include "lexer.h"
#include "token.h"
#include <vector>
#include <string>

class Parser
{
    Lexer lexer;
    Token current_token;
    Token peek_token;
    std::vector<std::string> errors_list;
    Statement* parse_statement();
    LetStatement* parse_let_statement();
    ReturnStatement* parse_return_statement();
    bool expected_token(const TokenType&);
    void advance_tokens();
    void expected_token_error(const TokenType&);

public:
    explicit Parser(const Lexer& l);
    Program parse_program();
    std::vector<std::string>& errors();
};

#endif // PARSER_H