#ifndef BUILTIN_H
#define BUILTIN_H
#include "object.h"
#include "utils.h"
#include "cleaner.h"
#include <map>
#include <string>
#include <vector>
using obj::Builtin;
using obj::BuiltinFunction;
using obj::Object;
using obj::Error;

static const char* UNSUPPORTED_ARGUMENT_TYPE = "Argumento para longitud sin soporte, se recibió %s cerca de la línea %d";
static const char* WRONG_ARGS_BUILTIN_FN = "Número incorrecto de argumentos para %s, se recibieron %d, se esperaba 1, cerca de la línea %d";

static BuiltinFunction longitud = [](const std::vector<Object*>& args, const int line) -> Object* 
{
    if(args.size() != 1)
    {
        auto error = new Error{ format(
                    WRONG_ARGS_BUILTIN_FN,
                    "longitud",
                    args.size(),
                    line) };
        eval_errors.push_back(error);
        return error;
    }

    auto argument = dynamic_cast<obj::String*>(args.at(0));

    if(argument)
    {
        if(argument->value.empty())
        {
            auto integer = new obj::Integer(0);
            cleaner.push_back(integer);
            return integer;
        }
        auto integer = new obj::Integer(argument->value.size());
        cleaner.push_back(integer);
        return integer;
    }
    
    auto error = new Error{ format(
                UNSUPPORTED_ARGUMENT_TYPE,
                args.at(0)->type_string().c_str(),
                line) };
    eval_errors.push_back(error);
    return error;
};

static std::map<std::string, Builtin> BUILTINS {
    {"longitud", Builtin(longitud)}
};

#endif // BUILTIN_H
