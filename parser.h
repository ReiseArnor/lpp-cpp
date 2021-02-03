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

using PrefixParseFn = std::function<Expression*()>;
using InfixParseFn = std::function<Expression*(Expression)>;

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
    bool expected_token(const TokenType&);
    void advance_tokens();
    void expected_token_error(const TokenType&);
    InfixParseFns register_infix_fns();
    PrefixParseFns register_prefix_fns();

public:
    explicit Parser(const Lexer& l);
    Program parse_program();
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
};

#endif // PARSER_H