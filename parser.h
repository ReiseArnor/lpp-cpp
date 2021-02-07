#ifndef PARSER_H
#define PARSER_H
#include "ast.h"
#include "lexer.h"
#include "token.h"
#include <cstdlib>
#include <functional>
#include <new>
#include <stdexcept>
#include <vector>
#include <string>
#include <map>
#include <memory>

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

static const std::map<const TokenType, const Precedence> PRECEDENCES
{
    { TokenType::EQ, Precedence::EQUALS },
    { TokenType::NOT_EQ, Precedence::EQUALS },
    { TokenType::LT, Precedence::LESSGREATER },
    { TokenType::GT, Precedence::LESSGREATER },
    { TokenType::PLUS, Precedence::SUM },
    { TokenType::MINUS, Precedence::SUM },
    { TokenType::DIVISION, Precedence::PRODUC },
    { TokenType::MULTIPLICATION, Precedence::PRODUC }
};

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
    ExpressionStatement* parse_expression_statements();
    Expression* parse_expression(Precedence);
    Block* parse_block();
    std::vector<Identifier*> parse_function_parameters();
    bool expected_token(const TokenType&);
    void advance_tokens();
    void expected_token_error(const TokenType&);
    InfixParseFns register_infix_fns();
    PrefixParseFns register_prefix_fns();
    Precedence current_precedence();
    Precedence peek_precedence();

public:
    explicit Parser(const Lexer& l);
    std::vector<Statement*> parse_program();
    std::vector<std::string>& errors();

private:
    PrefixParseFn parse_identifier = [&]() -> Expression* 
    {
        try{
            return new Identifier(Token(current_token), current_token.literal); 
        } catch(const std::bad_alloc& e)
        {
            errors_list.push_back("No se ha podido reservar espacio en memoria.");
            return nullptr;
        }
    };

    PrefixParseFn parse_integer = [&]() -> Expression*
    {
        try{
            return new Integer(current_token, std::stoi(current_token.literal));
        } catch(const std::invalid_argument& e) {
            std::string error = "No se ha podido parsear ";
            error.append(current_token.literal + " como entero.");
            errors_list.push_back(error);
            return nullptr;
        } catch(const std::bad_alloc& e) {
            errors_list.push_back("No se ha podido reservar espacio en memoria.");
            return nullptr;
        }
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
        try{
            return new Boolean(current_token, current_token.token_type == TokenType::_TRUE);
        } catch(const std::bad_alloc& e)
        {
            errors_list.push_back("No se ha podido reservar espacio en memoria.");
            return nullptr;
        }
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

    InfixParseFn parse_infix_expression = [&](Expression* left) -> Expression*
    {
        auto infix = std::make_unique<Infix>(current_token, left, current_token.literal);
        
        auto precedence = current_precedence();
        advance_tokens();
        infix->right = parse_expression(precedence);

        return infix.release();
    };
};

#endif // PARSER_H