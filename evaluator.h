#include "ast.h"
#include "object.h"
#include <memory>
#include <typeinfo>
#include <vector>
using obj::Object;
using ast::ASTNode;
using ast::Program;
using ast::Statement;
using ast::ExpressionStatement;

const static auto TRUE = std::make_unique<obj::Boolean>(true);
const static auto FALSE = std::make_unique<obj::Boolean>(false);
const static auto _NULL = std::make_unique<obj::Null>;

static Object* evaluate_statements(const std::vector<Statement*>&);
static Object* to_boolean_object(bool);

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
    else if(node_type == typeid(ast::Boolean).name())
    {
        auto new_node = dynamic_cast<ast::Boolean*>(node);
        return to_boolean_object(new_node->value);
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

Object* to_boolean_object(bool value)
{
    return value ? TRUE.get() : FALSE.get();
}