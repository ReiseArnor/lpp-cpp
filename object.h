#ifndef OBJECT_H
#define OBJECT_H
#include <string>

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
    int value;
public:
    explicit Integer(int v) : value(v) {}
    ObjectType type() override { return ObjectType::INTEGER; }
    std::string inspect() override { return std::to_string(value); }
};

class Boolean : public Object
{
    bool value;
public:
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

#endif // OBJECT_H