#ifndef PARSER_H
#define PARSER_H
#include "ast.h"
#include "lexer.h"
#include "token.h"
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <new>
#include <stdexcept>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <fmt/format.h>

using ast::Expression;
using ast::Statement;
using ast::LetStatement;
using ast::ReturnStatement;
using ast::ExpressionStatement;
using ast::Block;
using ast::Identifier;
using ast::Integer;
using ast::Boolean;
using ast::Prefix;
using ast::Infix;
using ast::If;
using ast::Function;
using ast::Call;
using ast::StringLiteral;
using ast::Null;
using ast::AssignStatement;

using PrefixParseFn = std::function<Expression*()>;
using InfixParseFn = std::function<Expression*(Expression*)>;

using PrefixParseFns = std::map<TokenType, PrefixParseFn>;
using InfixParseFns = std::map<TokenType, InfixParseFn>;

enum class Precedence
{
    LOWEST,
    EQUALS,
    LESSGREATER,
    SUM,
    PRODUC,
    PREFIX,
    CALL
};

static constexpr std::array<std::pair<TokenType, Precedence>, 9> precedence_values
{{
    { TokenType::EQ, Precedence::EQUALS },
    { TokenType::NOT_EQ, Precedence::EQUALS },
    { TokenType::LT, Precedence::LESSGREATER },
    { TokenType::GT, Precedence::LESSGREATER },
    { TokenType::PLUS, Precedence::SUM },
    { TokenType::MINUS, Precedence::SUM },
    { TokenType::DIVISION, Precedence::PRODUC },
    { TokenType::MULTIPLICATION, Precedence::PRODUC },
    { TokenType::LPAREN, Precedence::CALL }
 }};

class Parser
{
private:
    Lexer lexer;
    Token current_token;
    Token peek_token;
    PrefixParseFns prefix_parse_fns;
    InfixParseFns infix_parse_fns;
    std::vector<std::string> errors_list;

    Statement* parse_statement();
    LetStatement* parse_let_statement();
    ReturnStatement* parse_return_statement();
    AssignStatement* parse_assign_statement();
    ExpressionStatement* parse_expression_statements();
    Expression* parse_expression(Precedence);
    Block* parse_block();
    std::vector<Identifier*> parse_function_parameters();
    std::vector<Expression*> parse_call_arguments();
    bool expected_token(const TokenType&);
    void advance_tokens();
    void expected_token_error(const TokenType&);
    InfixParseFns register_infix_fns();
    PrefixParseFns register_prefix_fns();
    Precedence get_precedence(const TokenType&);

public:
    explicit Parser(const Lexer& l);
    std::vector<Statement*> parse_program();
    std::vector<std::string>& errors();

private:
    PrefixParseFn parse_identifier = [&]() -> Expression*
    {
        return new Identifier(current_token, current_token.literal);
    };

    PrefixParseFn parse_integer = [&]() -> Expression*
    {
        return new Integer(current_token, static_cast<std::size_t>(std::stoi(current_token.literal)));
    };

    PrefixParseFn parse_prefix_expression = [&]() -> Expression*
    {
        auto prefix_expression = std::make_unique<Prefix>(current_token, current_token.literal);

        advance_tokens();
        prefix_expression->right = parse_expression(Precedence::PREFIX);

        return prefix_expression.release();
    };

    PrefixParseFn parse_boolean = [&]() -> Expression*
    {
        return new Boolean(current_token, current_token.token_type == TokenType::_TRUE);
    };

    PrefixParseFn parse_null = [&]() -> Expression*
    {
        return new Null(current_token);
    };

    PrefixParseFn parse_grouped_expression = [&]() -> Expression*
    {
        advance_tokens();
        auto expression = std::unique_ptr<Expression>(parse_expression(Precedence::LOWEST));

        if (!expected_token(TokenType::RPAREN))
            return nullptr;

        return expression.release();
    };

    PrefixParseFn parse_if = [&]() -> Expression*
    {
        auto if_expression = std::make_unique<If>(current_token);

        if(!expected_token(TokenType::LPAREN))
            return nullptr;
        advance_tokens();

        if_expression->condition = parse_expression(Precedence::LOWEST);

        if(!expected_token(TokenType::RPAREN))
            return nullptr;

        if(!expected_token(TokenType::LBRACE))
            return nullptr;
        if_expression->consequence = parse_block();

        if(peek_token.token_type == TokenType::ELSE)
        {
            advance_tokens();
            if(!expected_token(TokenType::LBRACE))
                return nullptr;
            if_expression->alternative = parse_block();
        }

        return if_expression.release();
    };

    PrefixParseFn parse_function = [&]() -> Expression*
    {
        auto function = std::make_unique<Function>(current_token);
        if(!expected_token(TokenType::LPAREN))
            return nullptr;
        function->parameters = parse_function_parameters();

        if(!expected_token(TokenType::LBRACE))
            return nullptr;

        function->body = parse_block();

        return function.release();
    };

    PrefixParseFn parse_string_literal = [&]() -> Expression*
    {
        auto string_literal = std::make_unique<StringLiteral>(current_token, current_token.literal);
        return string_literal.release();
    };

    InfixParseFn parse_infix_expression = [&](Expression* left) -> Expression*
    {
        auto infix = std::make_unique<Infix>(current_token, left, current_token.literal);

        auto precedence = get_precedence(current_token.token_type);
        advance_tokens();
        infix->right = parse_expression(precedence);

        return infix.release();
    };

    InfixParseFn parse_call = [&](Expression* function) -> Expression*
    {
        auto call = std::make_unique<Call>(current_token, function);
        call->arguments = parse_call_arguments();
        return call.release();
    };

};

#endif // PARSER_H
