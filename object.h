#ifndef OBJECT_H
#define OBJECT_H
#include <string>

namespace obj 
{
enum class ObjectType
{
    BOOLEAN,
    INTEGER,
    _NULL
};

class Object
{
public:
    virtual ObjectType type() = 0;
    virtual std::string inspect() = 0; 
    virtual ~Object(){}
};

class Integer : public Object
{
public:
    int value;
    explicit Integer(int v) : value(v) {}
    ObjectType type() override { return ObjectType::INTEGER; }
    std::string inspect() override { return std::to_string(value); }
};

class Boolean : public Object
{
public:
    bool value;
    explicit Boolean(bool v) : value(v) {}
    ObjectType type() override { return ObjectType::BOOLEAN; }
    std::string inspect() override { return value ? "verdadero" : "falso"; }
};

class Null : public Object
{
public:
    ObjectType type() override { return ObjectType::_NULL;}
    std::string inspect() override { return "nulo"; }
};

} // namespace obj
#endif // OBJECT_H