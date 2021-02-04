#ifndef AST_H
#define AST_H
#include <cstddef>
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
    Program() = delete;
    Program(const Program&) = delete;
    Program& operator=(const Program&) = delete;

public:
    std::vector<Statement*> statements;

    explicit Program(const std::vector<Statement*>& s) : statements(s) {}

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

    bool operator==(const Program& other_program) const
    {
        if(to_string() == other_program.to_string())   
            return true;
        return false;
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
    Identifier* name;
    Expression value;
    LetStatement() = default;
    explicit LetStatement(const Token& token) : Statement(token), name(nullptr) {}
    explicit LetStatement(const Token& token, Identifier* name, const Expression& value) 
        : Statement(token), name(name), value(value) {}

    std::string to_string() const override
    {
        return token_literal() + " " + name->to_string() + " = " + value.to_string() + ";";
    }

    ~LetStatement()
    {
        if(name)
            delete name;
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

class ExpressionStatement : public Statement
{
public:
    Expression* expression;
    ExpressionStatement() = default;
    explicit ExpressionStatement(const Token& t) 
        : Statement(t), expression(nullptr){}
    explicit ExpressionStatement(const Token& t, Expression* e)
        : Statement(t), expression(e) {}

    std::string to_string() const override
    {
        return expression->to_string();
    }

    ~ExpressionStatement()
    {
        if(expression)
            delete expression;
    }
};

class Integer : public Expression
{
public:
    int value;
    explicit Integer(const Token& t) : Expression(t), value(0) {}
    Integer(const Token& t, const int v) 
        : Expression(t), value(v) {}

    std::string to_string() const override
    {
        return std::to_string(value);
    }
};

class Prefix : public Expression
{
public:
    std::string operatr;
    Expression* right;
    Prefix(const Token& t, const std::string& op)
        : Expression(t), operatr(op), right(nullptr) {}
    Prefix(const Token& t, const std::string& op, Expression* e)
        : Expression(t), operatr(op), right(e) {} 

    std::string to_string() const override
    {
        return "(" + operatr + right->to_string() + ")";
    }

    ~Prefix()
    {
        if(right)
            delete right;
    }
};

class Infix : public Expression
{
public:
    Expression* right;
    Expression* left;
    std::string operatr;
    Infix(const Token& t, Expression* l, const std::string& op)
        : Expression(t), left(l), operatr(op), right(nullptr) {}
    Infix(const Token& t, Expression* l, const std::string& op, Expression* r)
        : Expression(t), left(l), operatr(op), right(r) {}

    std::string to_string() const override
    {
        return "(" + left->to_string() + " " + operatr + " " + right->to_string() + ")";
    }

    ~Infix()
    {
        if(right)
            delete right;
        delete left;
    }
};

class Boolean : public Expression
{
public:
    bool value;
    Boolean(const Token& t) : Expression(t) {}
    Boolean(const Token& t, const bool v) : Expression(t), value(v) {}

    std::string to_string() const override
    {
        return token_literal();
    }
};
#endif // AST_H