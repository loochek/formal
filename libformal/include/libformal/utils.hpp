#pragma once

#include <string_view>

namespace formal {
    std::string_view strip(const std::string_view& str);

    inline void hash_combine(std::size_t& seed) {}

    template <typename T, typename... Rest>
    inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        hash_combine(seed, rest...);
    }
}

#define MAKE_HASHABLE_FWD(type) \
    namespace std { \
        template<> struct hash<type> { \
            std::size_t operator()(const type &t) const; \
        }; \
    }

#define MAKE_HASHABLE(type, ...) \
    namespace std { \
        template<> struct hash<type> { \
            std::size_t operator()(const type &t) const { \
                std::size_t ret = 0; \
                formal::hash_combine(ret, __VA_ARGS__); \
                return ret; \
            } \
        }; \
    }
