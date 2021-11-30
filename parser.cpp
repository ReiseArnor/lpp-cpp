#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include "token.h"
#include "utils.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <fmt/format.h>

using namespace std;
using namespace ast;

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
    if(current_token.token_type == TokenType::IDENT && peek_token.token_type == TokenType::ASSIGN)
        return parse_assign_statement();
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

    advance_tokens();
    let_statement->value = parse_expression(Precedence::LOWEST);

    if(peek_token.token_type == TokenType::SEMICOLON)
        advance_tokens();

    return let_statement.release();
}

ReturnStatement* Parser::parse_return_statement()
{
    auto return_statement = make_unique<ReturnStatement>(current_token);
    advance_tokens();

    return_statement->return_value = parse_expression(Precedence::LOWEST);

    if(peek_token.token_type == TokenType::SEMICOLON)
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

        while (peek_token.token_type != TokenType::SEMICOLON && precedence < get_precedence(peek_token.token_type))
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
        auto error = fmt::format("No se encontró ninguna función para parsear {} cerca de la línea {}\n", current_token.literal, current_token.line);
        errors_list.push_back(error);
        return nullptr;
    }
}

Block* Parser::parse_block()
{
    auto block_statement = make_unique<Block>(current_token, vector<Statement*>());
    advance_tokens();

    while (current_token.token_type != TokenType::RBRACE && current_token.token_type != TokenType::_EOF)
    {
        auto statement = parse_statement();
        if(statement)
            block_statement->statements.push_back(statement);
        advance_tokens();
    }
    return block_statement.release();
}

vector<Identifier*> Parser::parse_function_parameters()
{
    vector<Identifier*> params;
    if (peek_token.token_type == TokenType::RPAREN)
    {
        advance_tokens();
        return params;
    }
    advance_tokens();
    auto identifier = new Identifier(current_token, current_token.literal);
    params.push_back(identifier);

    while (peek_token.token_type == TokenType::COMMA)
    {
        advance_tokens();
        advance_tokens();
        auto identifiers = new Identifier(current_token, current_token.literal);
        params.push_back(identifiers);
    }

    if(!expected_token(TokenType::RPAREN))
        return {};

    return params;
}

vector<Expression*> Parser::parse_call_arguments()
{
    auto arguments = vector<Expression*>();
    if (peek_token.token_type == TokenType::RPAREN)
    {
        advance_tokens();
        return arguments;
    }

    advance_tokens();

    Expression* expression;
    if ((expression = parse_expression(Precedence::LOWEST)))
        arguments.push_back(expression);

    while (peek_token.token_type == TokenType::COMMA)
    {
        advance_tokens();
        advance_tokens();
        if ((expression = parse_expression(Precedence::LOWEST)))
            arguments.push_back(expression);
    }

    if (!expected_token(TokenType::RPAREN))
        return {};

    return arguments;
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
    auto error = fmt::format("Se esperaba que el siguente token fuera {} pero se obtuvo {} cerca de la línea {}",
            getNameForValue(tokens_enums_strings, tp),
            getNameForValue(tokens_enums_strings, peek_token.token_type),
            current_token.line);
    errors_list.push_back(error);
}

PrefixParseFns Parser::register_prefix_fns()
{
    return {
        { TokenType::FUNCTION, parse_function },
        { TokenType::_FALSE, parse_boolean },
        { TokenType::_TRUE, parse_boolean },
        { TokenType::_NULL, parse_null},
        { TokenType::IDENT, parse_identifier },
        { TokenType::IF, parse_if },
        { TokenType::INT, parse_integer },
        { TokenType::MINUS, parse_prefix_expression },
        { TokenType::NEGATION, parse_prefix_expression },
        { TokenType::LPAREN, parse_grouped_expression },
        { TokenType::STRING, parse_string_literal }
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
        { TokenType::GT, parse_infix_expression },
        { TokenType::LPAREN, parse_call }
    };
}

AssignStatement* Parser::parse_assign_statement()
{
    auto name = static_cast<Identifier*>(parse_identifier());
    if(!expected_token(TokenType::ASSIGN))
        return nullptr;

    auto token = current_token;
    advance_tokens();

    auto value = parse_expression(Precedence::LOWEST);
    if(peek_token.token_type == TokenType::SEMICOLON)
        advance_tokens();

    return new AssignStatement(token, name, value);
}

Precedence Parser::get_precedence(const TokenType& tp)
{
    static constexpr auto PRECEDENCES = Map<TokenType, Precedence, precedence_values.size()>{{precedence_values}};

    if(PRECEDENCES.find(tp))
        return PRECEDENCES.at(tp);

    return Precedence::LOWEST;
}
