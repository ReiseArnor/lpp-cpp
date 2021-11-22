#include "ast.h"
#include "object.h"
#include "builtin.h"
#include <cassert>
#include <cstddef>
#include <memory>
#include <string>
#include <typeinfo>
#include <vector>
#include "utils.h"
#include "cleaner.h"

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
using obj::Environment;
using ast::LetStatement;
using ast::Identifier;
using ast::Node;

static const char* WRONG_ARGS = "Cantidad errónea de argumentos para la función cerca de la línea %d, se esperaban %d pero se obtuvo %d";
static const char* NOT_A_FUNCTION = "No es una function: %s cerca de la línea %d";
static const char* TYPE_MISMATCH = "Discrepancia de tipos: %s %s %s cerca de la línea %d";
static const char* UNKNOWN_PREFIX_OPERATION = "Operador desconocido: %s%s cerca de la línea %d";
static const char* UNKNOWN_INFIX_OPERATION = "Operador desconocido: %s %s %s cerca de la línea %d";
static const char* UNKNOWN_IDENTIFIER = "Identificador sin definir: \"%s\" cerca de la línea %d";

const static auto TRUE = std::make_unique<obj::Boolean>(true);
const static auto FALSE = std::make_unique<obj::Boolean>(false);
const static auto _NULL = std::make_unique<obj::Null>();

static Object* evaluate_program(Program*, Environment*);
static Object* to_boolean_object(bool);
static Object* evaluate_prefix_expression(const std::string&, Object*, const int);
static Object* evaluate_infix_expression(const std::string&, Object*, Object*, const int);
static Object* evaluate_if_expression(If*, Environment*);
static Object* evaluate_block_statements(Block*, Environment*);
static Object* evaluate_identifier(Identifier*, Environment*, const int);
static std::vector<Object*> evaluate_expression(const std::vector<Expression*>&, Environment*);
static Object* apply_function(Object*, const std::vector<Object*>&, const int);

static Object* evaluate(ASTNode* node, Environment* env)
{
    auto node_type = node->type();

    switch (node_type) {

        case Node::Program:
            return evaluate_program(dynamic_cast<Program*>(node), env);

        case Node::ExpressionStatement:
            {
                auto cast_exp_st = dynamic_cast<ExpressionStatement*>(node);
                return evaluate(cast_exp_st->expression, env);
            }

        case Node::Integer:
            {
                auto cast_int = dynamic_cast<ast::Integer*>(node);
                auto integer = new obj::Integer(cast_int->value);
                cleaner.push_back(integer);
                return integer;
            }

        case Node::Boolean:
            {
                auto cast_bool = dynamic_cast<ast::Boolean*>(node);
                return to_boolean_object(cast_bool->value);
            }

        case Node::Prefix:
            {
                auto cast_prefix = dynamic_cast<Prefix*>(node);
                assert(cast_prefix != nullptr);
                auto right = evaluate(cast_prefix->right, env);
                assert(right != nullptr);
                return evaluate_prefix_expression(cast_prefix->operatr, right, cast_prefix->token.line);
            }

        case Node::Infix:
            {
                auto cast_infix = dynamic_cast<Infix*>(node);
                assert(cast_infix->left && cast_infix->right);
                auto left = evaluate(cast_infix->left, env);
                auto right = evaluate(cast_infix->right, env);
                assert(left && right);
                return evaluate_infix_expression(cast_infix->operatr, left, right, cast_infix->token.line);
            }

        case Node::Block:
            {
                auto cast_block = dynamic_cast<Block*>(node);
                return evaluate_block_statements(cast_block, env);
            }

        case Node::If:
            {
                auto cast_if = dynamic_cast<ast::If*>(node);
                return evaluate_if_expression(cast_if, env);
            }

        case Node::ReturnStatement:
            {
                auto cast_rtn_st = dynamic_cast<ReturnStatement*>(node);
                assert(cast_rtn_st->return_value);
                auto value = evaluate(cast_rtn_st->return_value, env);
                assert(value);
                auto return_val = new obj::Return(value);
                cleaner.push_back(return_val);
                return return_val;
            }

        case Node::LetStatement:
            {
                auto cast_let_st = dynamic_cast<LetStatement*>(node);
                assert(cast_let_st->value);
                auto value = evaluate(cast_let_st->value, env);
                assert(cast_let_st->name);
                env->set_item(cast_let_st->name->value, value);
                return value;
            }

        case Node::Identifier:
            {
                auto cast_ident = dynamic_cast<Identifier*>(node);
                assert(cast_ident);
                return evaluate_identifier(cast_ident, env, cast_ident->token.line);
            }

        case Node::Function:
            {
                auto cast_func = dynamic_cast<ast::Function*>(node);
                assert(cast_func);
                auto func = new obj::Function(cast_func->parameters, cast_func->body, env);
                cleaner.push_back(func);
                return func;
            }

        case Node::Call:
            {
                auto cast_call = dynamic_cast<ast::Call*>(node);
                auto function = evaluate(cast_call->function, env);
                auto args = evaluate_expression(cast_call->arguments, env);
                return apply_function(function, args, cast_call->token.line);
            }

        case Node::StringLiteral:
            {
                auto cast_str_lit = dynamic_cast<ast::StringLiteral*>(node);
                auto str = new obj::String(cast_str_lit->value);
                cleaner.push_back(str);
                return str;
            }

        default:
            return nullptr;
    }
}

static Environment* extend_function_environment(obj::Function* fn, const std::vector<Object*>& args, const int line)
{
    if(fn->parameters.size() != args.size())
    {
        auto error = new Error{ format(
                    WRONG_ARGS,
                    line,
                    fn->parameters.size(),
                    args.size()
                    ) };
        eval_errors.push_back(error);
        return nullptr;
    }

    auto env = new Environment(fn->env);
    environments.push_back(env);

    for(std::size_t i = 0; i < fn->parameters.size(); i++)
        env->set_item(fn->parameters.at(i)->value, args.at(i));

    return env;
}

static Object* unwrap_return_value(Object* obj)
{
    if(typeid(*obj) == typeid(obj::Return))
        return static_cast<obj::Return*>(obj)->value;
    return obj;
}

Object* apply_function(Object* fn, const std::vector<Object*>& args, const int line)
{
    if(typeid(*fn) == typeid(obj::Function))
    {
        auto function = static_cast<obj::Function*>(fn);
        auto extended_environment = extend_function_environment(function, args, line);
        if(!extended_environment)
            return eval_errors.at(eval_errors.size() - 1UL);

        auto evaluated = evaluate(function->body, extended_environment);
        return unwrap_return_value(evaluated);
    }
    else if(typeid(*fn) == typeid(obj::Builtin))
    {
        auto function = static_cast<obj::Builtin*>(fn);
        return function->fn(args, line);
    }

    auto error = new Error{ format(
                    NOT_A_FUNCTION,
                    fn->type_string().c_str(),
                    line
                    ) };
    eval_errors.push_back(error);
    return error;
}

Object* evaluate_program(Program* program, Environment* env)
{
    Object* result = nullptr;
    for(auto s : program->statements)
    {
        result = evaluate(s, env);
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

Object* evaluate_if_expression(If* if_expression, Environment* env)
{
    assert(if_expression->condition);
    auto condicion = evaluate(if_expression->condition, env);
    assert(condicion);

    if(is_truthy(condicion))
    {
        assert(if_expression->consequence);
        return evaluate(if_expression->consequence, env);
    }
    else if (if_expression->alternative)
        return evaluate(if_expression->alternative, env);
    else
        return _NULL.get();
}

Object* evaluate_block_statements(Block* block, Environment* env)
{
    Object* result = nullptr;

    for (auto statement : block->statements)
    {
        result = evaluate(statement, env);

        if((result && result->type() == ObjectType::RETURN)
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
    {
        auto error = new Error{ format(
                    UNKNOWN_PREFIX_OPERATION,
                    "-",
                    right->type_string().c_str(),
                    line
                    ) };
        eval_errors.push_back(error);
        return error;
    }

    auto cast_right = dynamic_cast<obj::Integer*>(right);
    auto integer = new obj::Integer(-cast_right->value);
    cleaner.push_back(integer);
    return integer;
}

Object* evaluate_prefix_expression(const std::string& operatr, Object* right, const int line)
{
    if(operatr == "!")
        return evaluate_bang_operator_expression(right);
    else if(operatr == "-")
        return evaluate_minus_operator_expression(right, line);
    else
    {
        auto error = new Error{ format(
                    UNKNOWN_PREFIX_OPERATION,
                    operatr.c_str(),
                    right->type_string().c_str(),
                    line
                    ) };
        eval_errors.push_back(error);
        return error;
    }

}

static Object* evaluate_integer_infix_expression(const std::string& operatr, Object* left, Object* right, const int line)
{
    auto left_value = static_cast<obj::Integer*>(left)->value;
    auto right_value = static_cast<obj::Integer*>(right)->value;

    if (operatr == "+")
    {
        auto integer = new obj::Integer(left_value + right_value);
        cleaner.push_back(integer);
        return integer;
    }
    else if (operatr == "-")
    {
        auto integer = new obj::Integer(left_value - right_value);
        cleaner.push_back(integer);
        return integer;
    }
    else if (operatr == "*")
    {
        auto integer = new obj::Integer(left_value * right_value);
        cleaner.push_back(integer);
        return integer;
    }
    else if (operatr == "/")
    {
        auto integer = new obj::Integer(left_value / right_value);
        cleaner.push_back(integer);
        return integer;
    }
    else if (operatr == "<")
        return to_boolean_object(left_value < right_value);
    else if (operatr == ">")
        return to_boolean_object(left_value > right_value);
    else if (operatr == "==")
        return to_boolean_object(left_value == right_value);
    else if (operatr == "!=")
        return to_boolean_object(left_value != right_value);
    else
    {
        auto error = new Error{ format(
                    UNKNOWN_INFIX_OPERATION,
                    left->type_string().c_str(),
                    operatr.c_str(),
                    right->type_string().c_str(),
                    line
                    ) };
        eval_errors.push_back(error);
        return error;
    }

}

static Object* evaluate_string_infix_expression(const std::string& operatr, Object* left, Object* right, const int line)
{
    auto left_value = static_cast<obj::String*>(left)->value;
    auto right_value = static_cast<obj::String*>(right)->value;

    if(operatr == "+")
    {
        auto str = new obj::String(left_value + right_value);
        cleaner.push_back(str);
        return str;
    }
    else if(operatr == "==")
        return to_boolean_object(left_value == right_value);
    else if(operatr == "!=")
        return to_boolean_object(left_value != right_value);

    auto error = new Error{ format(
                UNKNOWN_INFIX_OPERATION,
                left->type_string().c_str(),
                operatr.c_str(),
                right->type_string().c_str(),
                line
                ) };
    eval_errors.push_back(error);
    return error;
}

Object* evaluate_infix_expression(const std::string& operatr, Object* left, Object* right, const int line)
{
    if(left->type() == ObjectType::INTEGER && right->type() == ObjectType::INTEGER)
        return evaluate_integer_infix_expression(operatr, left, right, line);
    else if(left->type() == ObjectType::STRING && right->type() == ObjectType::STRING)
        return evaluate_string_infix_expression(operatr, left, right, line);
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
    {
        auto error = new Error{ format(
                    TYPE_MISMATCH,
                    left->type_string().c_str(),
                    operatr.c_str(),
                    right->type_string().c_str(),
                    line
                    ) };
        eval_errors.push_back(error);
        return error;
    }


    auto error = new Error{ format(
                UNKNOWN_INFIX_OPERATION,
                left->type_string().c_str(),
                operatr.c_str(),
                right->type_string().c_str(),
                line
                ) };
    eval_errors.push_back(error);
    return error;
}

Object* evaluate_identifier(Identifier* ident, Environment* env, const int line)
{
    if(env->item_exist(ident->value))
        return env->get_item(ident->value);
    else if(BUILTINS.find(ident->value) != BUILTINS.end())
        return &BUILTINS.at(ident->value);
    else
    {
        auto error = new Error{ format(
                    UNKNOWN_IDENTIFIER,
                    ident->value.c_str(),
                    line) };
        eval_errors.push_back(error);
        return error;
    }
}

std::vector<Object*> evaluate_expression(const std::vector<Expression*>& expressions, Environment* env)
{
    auto result = std::vector<Object*>();
    for(auto exp : expressions)
    {
        auto evaluated = evaluate(exp, env);
        if(evaluated)
            result.push_back(evaluated);
    }
    return result;
}

Object* to_boolean_object(bool value)
{
    return value ? TRUE.get() : FALSE.get();
}
