#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include "token.h"
#include <iostream>
#include <string>
using namespace std;

Parser::Parser(const Lexer& l) : lexer(l)
{
    advance_tokens();
    advance_tokens();
}

Program Parser::parse_program()
{
    vector<Statement*> statements;
    Program program(statements);
    while(current_token.token_type != TokenType::_EOF)
    {
        Statement* statement = parse_statement();
        if(statement != nullptr)
            program.statements.push_back(statement);
        advance_tokens();
    }
    return program; 
}

Statement* Parser::parse_statement()
{
    // the program receiving the pointer owns it
    if(current_token.token_type == TokenType::LET)
        return parse_let_statement();
    if(current_token.token_type == TokenType::RETURN)
        return parse_return_statement();
    return nullptr;
}

LetStatement* Parser::parse_let_statement()
{
    LetStatement* let_statement = new LetStatement(current_token);

    if(!expected_token(TokenType::IDENT))
        return nullptr;
    
    let_statement->name = Identifier(current_token, current_token.literal);

    if(!expected_token(TokenType::ASSIGN))
        return nullptr;

    while(current_token.token_type != TokenType::SEMICOLON)
        advance_tokens();

    return let_statement;
}

ReturnStatement* Parser::parse_return_statement()
{
    ReturnStatement* return_statement = new ReturnStatement(current_token);
    advance_tokens();

    while (current_token.token_type != TokenType::SEMICOLON)
        advance_tokens();

    return return_statement;
}

bool Parser::expected_token(const TokenType& tp)
{
    if(peek_token.token_type == tp)
    {
        advance_tokens();
        return true;
    }
    expected_token_error(tp);
    return false;
}

void Parser::advance_tokens()
{
    current_token = peek_token;
    peek_token = lexer.next_token();
}

vector<string>& Parser::errors()
{
    return errors_list;
}

void Parser::expected_token_error(const TokenType& tp)
{
    string error = "Se esperaba que el siguente token fuera ";
    error.append(getNameForValue(enums_strings, tp));
    error.append(" pero se obtuvo ");
    error.append(getNameForValue(enums_strings, peek_token.token_type) + "\n");
    errors_list.push_back(error);
}