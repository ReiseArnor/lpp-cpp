#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <algorithm>
#include <vector>
#include <cstdarg>

template<class T>
struct NameValuePair {
    using value_type = T;
    const T value;
    const char* const name;
};

template<class Mapping, class V>
static std::string getNameForValue(Mapping a, V value) {
    auto pos = std::find_if(std::begin(a), std::end(a), [&value](const typename Mapping::value_type& t){
        return (t.value == value);
    });
    return pos->name;
}

[[maybe_unused]] static std::string format(const char* str, ...)
{
    va_list args;
    va_start (args, str);
    size_t len = std::vsnprintf(NULL, 0, str, args);
    va_end (args);
    std::vector<char> vec(len + 1);
    va_start (args, str);
    std::vsnprintf(&vec[0], len + 1, str, args);
    va_end (args);
    return &vec[0];
}

#endif // UTILS_H
