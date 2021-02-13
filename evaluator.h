#include "ast.h"
#include "object.h"
#include <cassert>
#include <memory>
#include <string>
#include <typeinfo>
#include <vector>
#include <cstdarg>
using obj::Object;
using ast::ASTNode;
using ast::Program;
using ast::ExpressionStatement;
using ast::Prefix;
using ast::Infix;
using obj::ObjectType;
using ast::Block;
using ast::If;
using ast::ReturnStatement;
using obj::Error;

static std::string format_error(const char* format, ...)
{
    va_list args;
    va_start (args, format);
    size_t len = std::vsnprintf(NULL, 0, format, args);
    va_end (args);
    std::vector<char> vec(len + 1);
    va_start (args, format);
    std::vsnprintf(&vec[0], len + 1, format, args);
    va_end (args);
    return &vec[0];
}

static const char* TYPE_MISMATCH = "Discrepancia de tipos: %s %s %s cerca de la línea %d";
static const char* UNKNOWN_PREFIX_OPERATION = "Operador desconocido: %s%s cerca de la línea %d";
static const char* UNKNOWN_INFIX_OPERATION = "Operador desconocido: %s %s %s cerca de la línea %d";

const static auto TRUE = std::make_unique<obj::Boolean>(true);
const static auto FALSE = std::make_unique<obj::Boolean>(false);
const static auto _NULL = std::make_unique<obj::Null>();

static Object* evaluate_program(Program*);
static Object* to_boolean_object(bool);
static Object* evaluate_prefix_expression(const std::string&, Object*, const int);
static Object* evaluate_infix_expression(const std::string&, Object*, Object*, const int);
static Object* evaluate_if_expression(If*);
static Object* evaluate_block_statements(Block*);

static Object* evaluate(ASTNode* node)
{
    auto node_type = typeid(*node).name();

    if(node_type == typeid(Program).name())
    {
        return evaluate_program(dynamic_cast<Program*>(node));
    }
    else if(node_type == typeid(ExpressionStatement).name())
    {
        auto cast_node = dynamic_cast<ExpressionStatement*>(node);
        return evaluate(cast_node->expression);
    }
    else if(node_type == typeid(ast::Integer).name())
    {
        auto cast_node = dynamic_cast<ast::Integer*>(node);
        return new obj::Integer(cast_node->value);
    }
    else if(node_type == typeid(ast::Boolean).name())
    {
        auto cast_node = dynamic_cast<ast::Boolean*>(node);
        return to_boolean_object(cast_node->value);
    }
    else if(node_type == typeid(Prefix).name())
    {
        auto cast_node = dynamic_cast<Prefix*>(node);
        assert(cast_node != nullptr);

        auto right = evaluate(cast_node->right);
        assert(right != nullptr);

        return evaluate_prefix_expression(cast_node->operatr, right, cast_node->token.line);
    }
    else if (node_type == typeid(Infix).name())
    {
        auto cast_node = dynamic_cast<Infix*>(node);
        assert(cast_node->left && cast_node->right);

        auto left = evaluate(cast_node->left);
        auto right = evaluate(cast_node->right);

        assert(left && right);
        return evaluate_infix_expression(cast_node->operatr, left, right, cast_node->token.line);
    }
    else if (node_type == typeid(ast::Block).name())
    {
        auto cast_node = dynamic_cast<Block*>(node);
        return evaluate_block_statements(cast_node);
    }
    else if (node_type == typeid(ast::If).name()) 
    {
        auto cast_node = dynamic_cast<ast::If*>(node);
        return evaluate_if_expression(cast_node);
    }
    else if(node_type == typeid(ReturnStatement).name())
    {
        auto cast_node = dynamic_cast<ReturnStatement*>(node);

        assert(cast_node->return_value);
        auto value = evaluate(cast_node->return_value);
        assert(value);

        return new obj::Return(value);
    }

    return nullptr;
}

Object* evaluate_program(Program* program)
{
    Object* result = nullptr;
    for(auto s : program->statements)
    {
        result = evaluate(s);
        if(typeid(*result) == typeid(obj::Return))
        {
            auto cast_result = static_cast<obj::Return*>(result);
            return cast_result->value;
        }
        else if (typeid(*result) == typeid(obj::Error)) 
            return result;
    }

    return result;
}

static bool is_truthy(Object* obj)
{
    if(obj == _NULL.get())
        return false;
    else if(obj == TRUE.get())
        return true;
    else if(obj == FALSE.get())
        return false;
    else
        return true;
}

Object* evaluate_if_expression(If* if_expression)
{
    assert(if_expression->condition);
    auto condicion = evaluate(if_expression->condition);
    assert(condicion);

    if(is_truthy(condicion))
    {
        assert(if_expression->consequence);
        return evaluate(if_expression->consequence);
    }
    else if (if_expression->alternative)
        return evaluate(if_expression->alternative);
    else
        return _NULL.get();
}

Object* evaluate_block_statements(Block* block)
{
    Object* result = nullptr;

    for (auto statement : block->statements)
    {
        result = evaluate(statement);

        if(result && result->type() == ObjectType::RETURN
            || result->type() == ObjectType::ERROR)
            return result;
    }

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

static Object* evaluate_minus_operator_expression(Object* right, const int line)
{
    if(typeid(*right).name() != typeid(obj::Integer).name())
        return new Error{ format_error(
            UNKNOWN_PREFIX_OPERATION,
            "-",
            right->type_string().c_str(),
            line
            ) };
    auto cast_right = dynamic_cast<obj::Integer*>(right);

    return new obj::Integer(-cast_right->value);
}

Object* evaluate_prefix_expression(const std::string& operatr, Object* right, const int line)
{
    if(operatr == "!")
        return evaluate_bang_operator_expression(right);
    else if(operatr == "-")
        return evaluate_minus_operator_expression(right, line);
    else
        return new Error{ format_error(
                UNKNOWN_PREFIX_OPERATION, 
                operatr.c_str(), 
                right->type_string().c_str(),
                line
                ) };
}

static Object* evaluate_integer_infix_expression(const std::string& operatr, Object* left, Object* right, const int line)
{
    auto left_value = static_cast<obj::Integer*>(left)->value;
    auto right_value = static_cast<obj::Integer*>(right)->value;

    if (operatr == "+")
        return new obj::Integer(left_value + right_value);
    else if (operatr == "-")
        return new obj::Integer(left_value - right_value);
    else if (operatr == "*")
        return new obj::Integer(left_value * right_value);
    else if (operatr == "/")
        return new obj::Integer(left_value / right_value);
    else if (operatr == "<")
        return to_boolean_object(left_value < right_value);
    else if (operatr == ">")
        return to_boolean_object(left_value > right_value);
    else if (operatr == "==")
        return to_boolean_object(left_value == right_value);
    else if (operatr == "!=")
        return to_boolean_object(left_value != right_value);
    else
        return new Error{ format_error(
                UNKNOWN_INFIX_OPERATION,
                left->type_string().c_str(),
                operatr.c_str(),
                right->type_string().c_str(),
                line
                ) };
}

Object* evaluate_infix_expression(const std::string& operatr, Object* left, Object* right, const int line)
{
    if(left->type() == ObjectType::INTEGER && right->type() == ObjectType::INTEGER)
        return evaluate_integer_infix_expression(operatr, left, right, line);
    else if(operatr == "==")
        return to_boolean_object(
                dynamic_cast<obj::Boolean*>(left)->value == 
                dynamic_cast<obj::Boolean*>(right)->value
                );
    else if(operatr == "!=")
        return to_boolean_object(
                dynamic_cast<obj::Boolean*>(left)->value !=
                dynamic_cast<obj::Boolean*>(right)->value
                );
    else if(left->type() != right->type())
        return new Error{ format_error(
                TYPE_MISMATCH,
                left->type_string().c_str(),
                operatr.c_str(),
                right->type_string().c_str(),
                line
                ) };

    return new Error{ format_error(
            UNKNOWN_INFIX_OPERATION,
            left->type_string().c_str(),
            operatr.c_str(),
            right->type_string().c_str(),
            line
            ) };
}

Object* to_boolean_object(bool value)
{
    return value ? TRUE.get() : FALSE.get();
}