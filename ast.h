#ifndef AST_H
#define AST_H
#include <cstddef>
#include <string>
#include <vector>
#include "token.h"

namespace ast {
enum class Node {
    AssignStatement,
    Block,
    Boolean,
    Call,
    Expression,
    ExpressionStatement,
    Function,
    Identifier,
    If,
    Infix,
    Integer,
    LetStatement,
    Null,
    Prefix,
    Program,
    ReturnStatement,
    Statement,
    StringLiteral
};

class ASTNode
{
public:
    virtual std::string token_literal() const = 0;
    virtual std::string to_string() const = 0;
    virtual Node type() const = 0;
    virtual ~ASTNode(){}
};

class Statement : public ASTNode
{
public:
    Token token;
    Statement() = default;
    explicit Statement(const Token& t) : token(t) {}
    std::string token_literal() const override { return token.literal; }
    Node type() const override { return Node::Statement; }
};

class Expression : public ASTNode
{
public:
    Token token;
    Expression() = default;
    explicit Expression(const Token& t) : token(t) {}
    std::string token_literal() const override { return token.literal; }
    std::string to_string() const override { return token.literal; }
    Node type() const override { return Node::Expression; }
};

class Program : public ASTNode
{
    Program() = delete;
    Program(const Program&) = delete;
    Program& operator=(const Program&) = delete;

public:
    std::vector<Statement*> statements;

    explicit Program(const std::vector<Statement*>& s) : statements(s) {}
    Node type() const override { return Node::Program; }

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
    const std::string value;
    Identifier() = default;
    Identifier(const Token& t, const std::string& v)
        : Expression(t), value(v) {}
    Node type() const override { return Node::Identifier; }

    std::string to_string() const override
    {
        return value;
    }
};

class LetStatement : public Statement
{
public:
    Identifier* name;
    Expression* value;
    LetStatement() = default;
    explicit LetStatement(const Token& token) : Statement(token), name(nullptr), value(nullptr) {}
    explicit LetStatement(const Token& token, Identifier* name, Expression* value)
        : Statement(token), name(name), value(value) {}
    Node type() const override { return Node::LetStatement; }

    std::string to_string() const override
    {
        return token_literal() + " " + name->to_string() + " = " + value->to_string() + ";";
    }

    ~LetStatement()
    {
        if(name)
            delete name;
        if(value)
            delete value;
    }
};

class ReturnStatement : public Statement
{
public:
    Expression* return_value;
    ReturnStatement() = default;
    explicit ReturnStatement(const Token& t) : Statement(t), return_value(nullptr) {}
    explicit ReturnStatement(const Token& t, Expression* rv)
        : Statement(t), return_value(rv) {}
    Node type() const override { return Node::ReturnStatement; }

    std::string to_string() const override
    {
        return token_literal() + " " + return_value->to_string() + ";";
    }

    ~ReturnStatement()
    {
        if(return_value)
            delete return_value;
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
    Node type() const override { return Node::ExpressionStatement; }

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
    const std::size_t value;
    explicit Integer(const Token& t) : Expression(t), value(0) {}
    Integer(const Token& t, const std::size_t v)
        : Expression(t), value(v) {}
    Node type() const override { return Node::Integer; }

    std::string to_string() const override
    {
        return std::to_string(value);
    }
};

class Prefix : public Expression
{
public:
    const std::string operatr;
    Expression* right;
    Prefix(const Token& t, const std::string& op)
        : Expression(t), operatr(op), right(nullptr) {}
    Prefix(const Token& t, const std::string& op, Expression* e)
        : Expression(t), operatr(op), right(e) {}
    Node type() const override { return Node::Prefix; }

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
    const std::string operatr;
    Infix(const Token& t, Expression* l, const std::string& op)
        : Expression(t), right(nullptr), left(l), operatr(op) {}
    Infix(const Token& t, Expression* l, const std::string& op, Expression* r)
        : Expression(t), right(r), left(l), operatr(op) {}
    Node type() const override { return Node::Infix; }

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
    const bool value;
    explicit Boolean(const Token& t) : Expression(t), value(false) {}
    Boolean(const Token& t, const bool v) : Expression(t), value(v) {}
    Node type() const override { return Node::Boolean; }

    std::string to_string() const override
    {
        return token_literal();
    }
};

class Block : public Statement
{
public:
    std::vector<Statement*> statements;
    Block(const Token& t, const std::vector<Statement*>& vs)
        : Statement(t), statements(vs) {}
    Node type() const override { return Node::Block; }

    std::string to_string() const override
    {
        std::string out;
        for(auto s : statements)
            out.append(s->to_string());
        return out;
    }

    ~Block()
    {
        for(auto s : statements)
            delete s;
    }
};

class If : public Expression
{
public:
    Expression* condition;
    Block* consequence;
    Block* alternative;
    explicit If(const Token& t)
        : Expression(t), condition(nullptr), consequence(nullptr), alternative(nullptr) {}
    If(const Token& t, Expression* cond, Block* cons)
        : Expression(t), condition(cond), consequence(cons), alternative(nullptr) {}
    If(const Token& t, Expression* cond, Block* cons, Block* alt)
        : Expression(t), condition(cond), consequence(cons), alternative(alt) {}
    Node type() const override { return Node::If; }

    std::string to_string() const override
    {
        std::string out = "si " + condition->to_string() + " " + consequence->to_string();
        if(alternative)
            out.append(" si_no" + alternative->to_string());
        return out;
    }

    ~If()
    {
        if(condition)
            delete condition;
        if(consequence)
            delete consequence;
        if(alternative)
            delete alternative;
    }
};

class Function : public Expression
{
public:
    std::vector<Identifier*> parameters;
    Block* body;
    explicit Function(const Token& t, const std::vector<Identifier*>& p = {})
        : Expression(t), parameters(p), body(nullptr) {}
    Function(const Token& t, const std::vector<Identifier*>& p, Block* b)
        :   Expression(t), parameters(p), body(b) {}
    Node type() const override { return Node::Function; }

    std::string to_string() const override
    {
        std::string params;
        for(auto s : parameters)
            params.append(s->to_string() + ", ");
        params.erase(params.size() - 2, 2);
        return token_literal() + "(" + params + ")" + "{" + body->to_string() + "}";
    }

    ~Function()
    {
        delete body;
        for(auto p : parameters)
            delete p;
    }
};

class Call : public Expression
{
public:
    Expression* function;
    std::vector<Expression*> arguments;
    Call(const Token& t, Expression* f)
        : Expression(t), function(f) {}
    Call(const Token& t, Expression* f, const std::vector<Expression*>& args)
        : Expression(t), function(f), arguments(args) {}
    Node type() const override { return Node::Call; }

    std::string to_string() const override
    {
        std::string args;
        for(auto arg : arguments)
            args.append(arg->to_string() + ", ");
        args.erase(args.size() - 2, 2);
        return function->to_string() + "(" + args + ")";
    }

    ~Call()
    {
        delete function;
        for(auto arg : arguments)
            delete arg;
    }
};

class StringLiteral : public Expression
{
public:
    const std::string value;
    StringLiteral(const Token& t, const std::string& val)
        : Expression(t), value(val) {}
    Node type() const override { return Node::StringLiteral; }
    std::string to_string() const override
    {
        return Expression::to_string();
    }
};

class Null : public Expression
{
public:
    explicit Null(const Token& t) : Expression(t) {}
    Node type() const override { return Node::Null; }

    std::string to_string() const override
    {
        return token_literal();
    }
};

class AssignStatement : public Statement
{
public:
    Identifier* name;
    Expression* value;
    AssignStatement(const Token& t, Identifier* i, Expression* e)
        : Statement(t), name(i), value(e) {}
    Node type() const override { return Node::AssignStatement; }

    std::string to_string() const override
    {
        return name->to_string() + " = " + value->to_string();
    }
};

class Programs_Guard
{
    std::vector<Program*> programs;
public:
    Programs_Guard() = default;
    void push_back(Program* p) { programs.push_back(p); }

    Program* new_program(const std::vector<Statement*>& statements)
    {
        auto prog = new Program(statements);
        programs.push_back(prog);
        return prog;
    }

    ~Programs_Guard()
    {
        for(auto p : programs)
            delete p;
    }
};

} // namespace ast
#endif // AST_H
