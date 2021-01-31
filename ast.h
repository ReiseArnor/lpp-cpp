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

    friend std::ostream& operator<<(std::ostream& out, const Program& p)
    {
        for(auto s : p.statements)
            out << s->token_literal() << "\n";
        return out;
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

    friend std::ostream& operator<<(std::ostream& out, const Identifier& e)
    {
        return out << e.value;
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

    friend std::ostream& operator<<(std::ostream& out, const LetStatement& ls)
    {
        return out << ls.token_literal() << " " << ls.name << " = " << ls.value.token_literal() << ";\n";
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

    friend std::ostream& operator<<(std::ostream& out, const ReturnStatement& rs)
    {
        out << rs.token_literal() << " " << rs.return_value.token_literal();
        return out;
    }
};
#endif // AST_H