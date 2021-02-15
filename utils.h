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
std::string getNameForValue(Mapping a, V value) {
    auto pos = std::find_if(std::begin(a), std::end(a), [&value](const typename Mapping::value_type& t){
        return (t.value == value);
    });
    return pos->name;
}

static std::string format(const char* format, ...)
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

template<class T>
class Cleaner
{
    std::vector<T*> store;
public:
    Cleaner() = default;
    void push_back(T* obj) { store.push_back(obj); }
    T* at(const int index) const { return store.at(index); }
    int size() const { return store.size(); }
    ~Cleaner()
    {
        for(auto obj : store)
            delete obj;
    }
};

#endif // UTILS_H