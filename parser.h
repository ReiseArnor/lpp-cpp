#ifndef PARSER_H
#define PARSER_H
#include "ast.h"
#include "lexer.h"
#include "token.h"
#include <vector>

class Parser
{
    Lexer lexer;
    Token current_token;
    Token peek_token;
    Statement* parse_statement();
    LetStatement* parse_let_statement();
    bool expected_token(const TokenType tp);
    void advance_tokens();

public:
    explicit Parser(const Lexer& l);
    Program parse_program();
};

#endif // PARSER_H