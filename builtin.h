#ifndef BUILTIN_H
#define BUILTIN_H
#include "object.h"
#include "utils.h"
#include "cleaner.h"
#include <map>
#include <string>
#include <string_view>
#include <vector>
#include <fmt/format.h>

using obj::Builtin;
using obj::BuiltinFunction;
using obj::Object;
using obj::Error;

static constexpr std::string_view UNSUPPORTED_ARGUMENT_TYPE = "Argumento para longitud sin soporte, se recibió {} cerca de la línea {}";
static constexpr std::string_view WRONG_ARGS_BUILTIN_FN = "Número incorrecto de argumentos para {}, se recibieron {}, se esperaba 1, cerca de la línea {}";

static const BuiltinFunction longitud = [](const std::vector<Object*>& args, const int line) -> Object*
{
    if(args.size() != 1)
    {
        auto error = new Error{
            fmt::format(WRONG_ARGS_BUILTIN_FN,
                        "longitud",
                        args.size(),
                        line
        )};
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

    auto error = new Error{
        fmt::format(UNSUPPORTED_ARGUMENT_TYPE,
                    args.at(0)->type_string(),
                    line
    )};
    eval_errors.push_back(error);
    return error;
};

static std::map<std::string_view, Builtin> BUILTINS {
    {"longitud", Builtin(longitud)}
};

#endif // BUILTIN_H
