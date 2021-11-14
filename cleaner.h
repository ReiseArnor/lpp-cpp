#ifndef CLEANER_H
#define CLEANER_H
#include "object.h"
#include <cstddef>

template<class T>
class Cleaner
{
    std::vector<T*> store;
public:
    Cleaner() = default;
    void push_back(T* obj) { store.push_back(obj); }
    T* at(const std::size_t index) const { return store.at(index); }
    std::size_t size() const { return store.size(); }
    ~Cleaner()
    {
        for(auto obj : store)
            delete obj;
    }
};

static auto eval_errors = Cleaner<obj::Object>();
static auto cleaner = Cleaner<obj::Object>();
static auto environments = Cleaner<obj::Environment>();

#endif // CLEANER_H
