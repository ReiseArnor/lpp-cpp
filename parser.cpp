#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include "token.h"
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

Parser::Parser(const Lexer& l) : lexer(l)
{
    prefix_parse_fns = register_prefix_fns();
    infix_parse_fns = register_infix_fns();
    advance_tokens();
    advance_tokens();
}

vector<Statement*> Parser::parse_program()
{
    vector<Statement*> statements;
    while(current_token.token_type != TokenType::_EOF)
    {
        Statement* statement = parse_statement();
        if(statement)
            statements.push_back(statement);
        advance_tokens();
    }
    return statements; 
}

Statement* Parser::parse_statement()
{
    // the program receiving the pointer owns it
    if(current_token.token_type == TokenType::LET)
        return parse_let_statement();
    if(current_token.token_type == TokenType::RETURN)
        return parse_return_statement();
    return parse_expression_statements();
}

LetStatement* Parser::parse_let_statement()
{
    auto let_statement = make_unique<LetStatement>(current_token);

    if(!expected_token(TokenType::IDENT))
        return nullptr;
    
    let_statement->name = static_cast<Identifier*>(parse_identifier());

    if(!expected_token(TokenType::ASSIGN))
        return nullptr;

    while(current_token.token_type != TokenType::SEMICOLON)
        advance_tokens();

    return let_statement.release();
}

ReturnStatement* Parser::parse_return_statement()
{
    auto return_statement = make_unique<ReturnStatement>(current_token);
    advance_tokens();

    while (current_token.token_type != TokenType::SEMICOLON)
        advance_tokens();

    return return_statement.release();
}

ExpressionStatement* Parser::parse_expression_statements()
{
    auto expression_statement = make_unique<ExpressionStatement>(current_token);

    expression_statement->expression = parse_expression(Precedence::LOWEST);
    if(peek_token.token_type == TokenType::SEMICOLON)
        advance_tokens();
    
    return expression_statement.release();
}

Expression* Parser::parse_expression(Precedence precedence)
{
    try
    {
        auto prefix_parse_fn = prefix_parse_fns[current_token.token_type];
        auto left_expression = prefix_parse_fn();

        while (peek_token.token_type != TokenType::SEMICOLON && precedence < peek_precedence()) 
        {
            auto infix_parse_fn = infix_parse_fns[peek_token.token_type];
            advance_tokens();
            if(left_expression)
                left_expression = infix_parse_fn(left_expression);
        }
        return left_expression;
    }
    catch(...)
    {
        string error = "No se encontró ninguna función para parsear ";
        error.append(current_token.literal + "\n");
        errors_list.push_back(error);
        return nullptr;
    } 
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

PrefixParseFns Parser::register_prefix_fns()
{
    return {
        { TokenType::IDENT, parse_identifier },
        { TokenType::INT, parse_integer },
        { TokenType::MINUS, parse_prefix_expression },
        { TokenType::NEGATION, parse_prefix_expression }
    };
}

InfixParseFns Parser::register_infix_fns()
{
    return {
        
        { TokenType::PLUS, parse_infix_expression },
        { TokenType::MINUS, parse_infix_expression },
        { TokenType::DIVISION, parse_infix_expression },
        { TokenType::MULTIPLICATION, parse_infix_expression },
        { TokenType::EQ, parse_infix_expression },
        { TokenType::NOT_EQ, parse_infix_expression },
        { TokenType::LT, parse_infix_expression },
        { TokenType::GT, parse_infix_expression }
    };
}

Precedence Parser::current_precedence()
{
    try{
    return PRECEDENCES.at(current_token.token_type);
    } catch(const out_of_range& e) {
        return Precedence::LOWEST;
    }
}

Precedence Parser::peek_precedence()
{
    try{
    return PRECEDENCES.at(peek_token.token_type);
    } catch(const out_of_range& e) {
        return Precedence::LOWEST;
    }
}