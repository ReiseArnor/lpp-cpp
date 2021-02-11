#include "ast.h"
#include "object.h"
#include <typeinfo>
#include <vector>
using obj::Object;
using ast::ASTNode;
using ast::Program;
using ast::Statement;
using ast::ExpressionStatement;

static Object* evaluate_statements(const std::vector<Statement*>&);

static Object* evaluate(ASTNode* node)
{
    auto node_type = typeid(*node).name();

    if(node_type == typeid(Program).name())
    {
        auto new_node = dynamic_cast<Program*>(node);
        return evaluate_statements(new_node->statements);
    }
    else if(node_type == typeid(ExpressionStatement).name())
    {
        auto new_node = dynamic_cast<ExpressionStatement*>(node);
        return evaluate(new_node->expression);
    }
    else if(node_type == typeid(ast::Integer).name())
    {
        auto new_node = dynamic_cast<ast::Integer*>(node);
        return new obj::Integer(new_node->value);
    }

    return nullptr;
}

Object* evaluate_statements(const std::vector<Statement*>& statements)
{
    Object* result = nullptr;
    for(auto s : statements)
        result = evaluate(s);

    return result;
}