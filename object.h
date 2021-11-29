#ifndef OBJECT_H
#define OBJECT_H
#include <cstddef>
#include <map>
#include <string>
#include <string_view>
#include <vector>
#include "ast.h"
#include "parser.h"
#include "token.h"
#include "utils.h"
#include <functional>

using ast::Identifier;
using ast::Block;

namespace obj
{
enum class ObjectType
{
    BOOLEAN,
    INTEGER,
    _NULL,
    RETURN,
    ERROR,
    FUNCTION,
    STRING,
    BUILTIN
};

static constexpr std::array<const NameValuePair<ObjectType>, 8> objects_enums_string {{
    {ObjectType::BOOLEAN, "BOOLEAN"},
    {ObjectType::INTEGER, "INTEGER"},
    {ObjectType::_NULL, "NULL"},
    {ObjectType::RETURN, "RETURN"},
    {ObjectType::ERROR, "ERROR"},
    {ObjectType::FUNCTION, "FUNCTION"},
    {ObjectType::STRING, "STRING"},
    {ObjectType::BUILTIN, "BUILTIN"}
}};

class Object
{
public:
    virtual ObjectType type() const = 0;
    virtual std::string inspect() const = 0;
    virtual std::string_view type_string() const = 0;
    virtual ~Object(){}
};

class Integer : public Object
{
public:
    const std::size_t value;
    explicit Integer(const std::size_t v) : value(v) {}
    ObjectType type() const override { return ObjectType::INTEGER; }
    std::string inspect() const override { return std::to_string(value); }
    std::string_view type_string() const override { return getNameForValue(objects_enums_string, ObjectType::INTEGER); }
};

class Boolean : public Object
{
public:
    const bool value;
    explicit Boolean(bool v) : value(v) {}
    ObjectType type() const override { return ObjectType::BOOLEAN; }
    std::string inspect() const override { return value ? "verdadero" : "falso"; }
    std::string_view type_string() const override { return getNameForValue(objects_enums_string, ObjectType::BOOLEAN); }
};

class Null : public Object
{
public:
    ObjectType type() const override { return ObjectType::_NULL;}
    std::string inspect() const override { return "nulo"; }
    std::string_view type_string() const override { return getNameForValue(objects_enums_string, ObjectType::_NULL); }
};

class Return : public Object
{
public:
    Object* value;
    explicit Return(Object* v) : value(v) {}
    ObjectType type() const override { return ObjectType::RETURN; }
    std::string inspect() const override { return value->inspect(); }
    std::string_view type_string() const override { return getNameForValue(objects_enums_string, ObjectType::RETURN); }
};

class Error : public Object
{
public:
    const std::string message;
    explicit Error(const std::string& msg) : message(msg) {}
    ObjectType type() const override { return ObjectType::ERROR; }
    std::string inspect() const override { return message; }
    std::string_view type_string() const override { return getNameForValue(objects_enums_string, ObjectType::ERROR); }
};

class Environment
{
    std::map<std::string, Object*> store;
    Environment* outer = nullptr;
public:
    Environment() = default;
    explicit Environment(Environment* outer) : outer(outer) { }
    void set_item(const std::string& key, Object* value) { store[key] = value; }
    void del_item(const std::string& key){ store.erase(key); }

    Object* get_item(const std::string& key)
    {
        if(store[key])
            return store[key];
        else
            return outer->get_item(key);
    }

    bool item_exist(const std::string& key)
    {
        if(store.find(key) != store.end())
            return true;
        else if(outer && outer->item_exist(key))
            return true;
        return false;
    }
};

class Function : public Object
{
public:
    std::vector<Identifier*> parameters;
    Block* body;
    Environment* env;
    Function(const std::vector<Identifier*>& params, Block* b, Environment* env )
        : parameters(params), body(b), env(env) {}
    ObjectType type() const override { return ObjectType::FUNCTION; }
    std::string_view type_string() const override { return getNameForValue(objects_enums_string, ObjectType::FUNCTION); }
    std::string inspect() const override
    {
       return "Función";
    }
};

class String : public Object
{
public:
    const std::string value;
    explicit String(const std::string& v) : value(v) {}
    ObjectType type() const override { return ObjectType::STRING; }
    std::string inspect() const override { return value; }
    std::string_view type_string() const override { return getNameForValue(objects_enums_string, ObjectType::STRING); }
};

using BuiltinFunction = std::function<Object*(const std::vector<Object*>&, const int)>;

class Builtin : public Object
{
public:
    const BuiltinFunction& fn;
    explicit Builtin(const BuiltinFunction& builtin_fn) : fn(builtin_fn) {}
    ObjectType type() const override { return ObjectType::BUILTIN; }
    std::string inspect() const override { return "builtin function"; }
    std::string_view type_string() const override { return getNameForValue(objects_enums_string, ObjectType::BUILTIN); }
};

} // namespace obj
#endif // OBJECT_H
