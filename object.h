#ifndef OBJECT_H
#define OBJECT_H
#include <map>
#include <string>
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

const std::array<const NameValuePair<ObjectType>, 8> objects_enums_string {{
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
    virtual ObjectType type() = 0;
    virtual std::string inspect() = 0;
    virtual std::string type_string() = 0; 
    virtual ~Object(){}
};

class Integer : public Object
{
public:
    int value;
    explicit Integer(int v) : value(v) {}
    ObjectType type() override { return ObjectType::INTEGER; }
    std::string inspect() override { return std::to_string(value); }
    std::string type_string() override { return getNameForValue(objects_enums_string, ObjectType::INTEGER); }
};

class Boolean : public Object
{
public:
    bool value;
    explicit Boolean(bool v) : value(v) {}
    ObjectType type() override { return ObjectType::BOOLEAN; }
    std::string inspect() override { return value ? "verdadero" : "falso"; }
    std::string type_string() override { return getNameForValue(objects_enums_string, ObjectType::BOOLEAN); }
};

class Null : public Object
{
public:
    ObjectType type() override { return ObjectType::_NULL;}
    std::string inspect() override { return "nulo"; }
    std::string type_string() override { return getNameForValue(objects_enums_string, ObjectType::_NULL); }
};

class Return : public Object
{
public:
    Object* value;
    explicit Return(Object* v) : value(v) {}
    ObjectType type() override { return ObjectType::RETURN; }
    std::string inspect() override { return value->inspect(); }
    std::string type_string() override { return getNameForValue(objects_enums_string, ObjectType::RETURN); }
};

class Error : public Object
{
public:
    const std::string message;
    explicit Error(const std::string& msg) : message(msg) {}
    ObjectType type() override { return ObjectType::ERROR; }
    std::string inspect() override { return message; }
    std::string type_string() override { return getNameForValue(objects_enums_string, ObjectType::ERROR); }
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

static const char* FUNCTION_NAME = "Función: %p";
class Function : public Object
{
public:
    std::vector<Identifier*> parameters;
    Block* body;
    Environment* env;
    Function(const std::vector<Identifier*>& params, Block* b, Environment* env )
        : parameters(params), body(b), env(env) {}
    ObjectType type() override { return ObjectType::FUNCTION; }
    std::string type_string() override { return getNameForValue(objects_enums_string, ObjectType::FUNCTION); }
    std::string inspect() override 
    {
       return format(FUNCTION_NAME, static_cast<void*>(this));
    }
};

class String : public Object
{
public:
    std::string value;
    explicit String(const std::string& v) : value(v) {}
    ObjectType type() override { return ObjectType::STRING; }
    std::string inspect() override { return value; }
    std::string type_string() override { return getNameForValue(objects_enums_string, ObjectType::STRING); }
};

using BuiltinFunction = std::function<Object*(const std::vector<Object*>&, const int)>;

class Builtin : public Object
{
public:
    const BuiltinFunction& fn;
    explicit Builtin(const BuiltinFunction& builtin_fn) : fn(builtin_fn) {}
    ObjectType type() override { return ObjectType::BUILTIN; }
    std::string inspect() override { return "builtin function"; }
    std::string type_string() override { return getNameForValue(objects_enums_string, ObjectType::BUILTIN); }
};

} // namespace obj
#endif // OBJECT_H