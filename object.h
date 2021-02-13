#ifndef OBJECT_H
#define OBJECT_H
#include <string>
#include "token.h"
namespace obj 
{
enum class ObjectType
{
    BOOLEAN,
    INTEGER,
    _NULL,
    RETURN,
    ERROR
};

const std::array<const NameValuePair<ObjectType>, 5> objects_enums_string {{
    {ObjectType::BOOLEAN, "BOOLEAN"},
    {ObjectType::INTEGER, "INTEGER"},
    {ObjectType::_NULL, "NULL"},
    {ObjectType::RETURN, "RETURN"},
    {ObjectType::ERROR, "ERROR"}
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

} // namespace obj
#endif // OBJECT_H