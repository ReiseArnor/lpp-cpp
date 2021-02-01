#ifndef AST_H
#define AST_H
#include <ostream>
#include <string>
#include <vector>
#include "token.h"

class ASTNode
{
public:
    virtual std::string token_literal() const = 0;
    virtual std::string to_string() const = 0;
    virtual ~ASTNode(){}
};

class Statement : public ASTNode
{
public:
    Token token;
    Statement() = default;
    explicit Statement(const Token& t) : token(t) {}
    std::string token_literal() const override { return token.literal; }
};

class Expression : public ASTNode
{
public:
    Token token;
    Expression() = default;
    explicit Expression(const Token& t) : token(t) {}
    std::string token_literal() const override { return token.literal; }
    std::string to_string() const override { return token.literal; }
};

class Program : public ASTNode
{
public:
    std::vector<Statement*> statements;

    explicit Program(const std::vector<Statement*>& s) : statements(s) {}
    Program() = default;
    Program(const Program&) = default;
    Program& operator=(const Program&) = default;

    std::string token_literal() const override 
    {
        if(statements.size() > 0)
            return statements.at(0)->token_literal();
        return "";
    }

    std::string to_string() const override
    {
        std::string str;
        for(auto s : statements)
            str.append(s->to_string());
        return str;
    }

    ~Program()
    {
        for(auto s : statements)
            delete s;
    }
};

class Identifier : public Expression
{
public:
    std::string value;
    Identifier() = default;
    Identifier(const Token& t, const std::string& v) 
        : Expression(t), value(v) {}

    std::string to_string() const override
    {
        return value;
    }
};

class LetStatement : public Statement
{
public:
    Identifier name;
    Expression value;
    LetStatement() = default;
    explicit LetStatement(const Token& token) : Statement(token) {}
    explicit LetStatement(const Token& token, const Identifier& name, const Expression& value) 
        : Statement(token), name(name), value(value) {}

    std::string to_string() const override
    {
        return token_literal() + " " + name.to_string() + " = " + value.to_string() + ";";
    }
};

class ReturnStatement : public Statement
{
public:
    Expression return_value;
    ReturnStatement() = default;
    explicit ReturnStatement(const Token& t) : Statement(t) {}
    explicit ReturnStatement(const Token& t, const Expression& rv)
        : Statement(t), return_value(rv) {}

    std::string to_string() const override
    {
        return token_literal() + " " + return_value.to_string() + ";";
    }
};
#endif // AST_H