#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <algorithm>
#include <cstdarg>
#include <array>
#include <string_view>

template<class T>
struct NameValuePair {
    using value_type = T;
    const T value;
    const char* const name;
};

template<class Mapping, class V>
static std::string_view getNameForValue(Mapping a, V value) {
    auto pos = std::find_if(std::begin(a), std::end(a), [&value](const typename Mapping::value_type& t){
        return (t.value == value);
    });
    return pos->name;
}

template <typename Key, typename Value, std::size_t Size>
struct Map {
    std::array<std::pair<Key, Value>, Size> data;

    [[nodiscard]] constexpr Value at(const Key &key) const {
        const auto itr =
        std::find_if(begin(data), end(data),
                     [&key](const auto &v) { return v.first == key; });
        return itr->second;
    };

    [[nodiscard]] constexpr bool find(const Key &key) const {
        const auto itr =
            std::find_if(begin(data), end(data),
                [&key](const auto &v) { return v.first == key; });
        if (itr != end(data)) {
            return true;
        }

        return false;
  };
};
#endif // UTILS_H
