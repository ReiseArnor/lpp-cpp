#include "ast.h"
#include "object.h"
#include <cassert>
#include <memory>
#include <string>
#include <typeinfo>
#include <vector>
using obj::Object;
using ast::ASTNode;
using ast::Program;
using ast::Statement;
using ast::ExpressionStatement;
using ast::Prefix;

const static auto TRUE = std::make_unique<obj::Boolean>(true);
const static auto FALSE = std::make_unique<obj::Boolean>(false);
const static auto _NULL = std::make_unique<obj::Null>();

static Object* evaluate_statements(const std::vector<Statement*>&);
static Object* to_boolean_object(bool);
static Object* evaluate_prefix_expression(const std::string&, Object*);

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
    else if(node_type == typeid(Prefix).name())
    {
        auto new_node = dynamic_cast<Prefix*>(node);
        assert(new_node != nullptr);

        auto right = evaluate(new_node->right);
        assert(right != nullptr);

        return evaluate_prefix_expression(new_node->operatr, right);
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

static Object* evaluate_bang_operator_expression(Object* right)
{
    if(right == TRUE.get())
        return FALSE.get();
    else if(right == FALSE.get())
        return TRUE.get();
    else if(right == _NULL.get())
        return TRUE.get();
    else
        return FALSE.get();
}

static Object* evaluate_minus_operator_expression(Object* right)
{
    if(typeid(*right).name() != typeid(obj::Integer).name())
        return nullptr;
    auto cast_right = dynamic_cast<obj::Integer*>(right);

    return new obj::Integer(-cast_right->value);
}

Object* evaluate_prefix_expression(const std::string& operatr, Object* right)
{
    if(operatr == "!")
        return evaluate_bang_operator_expression(right);
    else if(operatr == "-")
        return evaluate_minus_operator_expression(right);
    else
        return _NULL.get();
}

Object* to_boolean_object(bool value)
{
    return value ? TRUE.get() : FALSE.get();
}