//
// Created by alex on 22.03.24.
//

#ifndef ENGINE23_STLUTILS_H
#define ENGINE23_STLUTILS_H

#include <vector>
#include <algorithm>

namespace Bcg {

    template<typename First, typename Second>
    inline std::vector<std::pair<First, Second>> Zip(const std::vector<First> &first,
                                                     const std::vector<Second> &second) {

        size_t min_size = std::min(first.size(), second.size());
        std::vector<std::pair<First, Second>> container(min_size);

        for (size_t i = 0; i < min_size; ++i) {
            container[i] = std::make_pair(first[i], second[i]);
        }
        return container;
    }

    template<typename First, typename Second>
    inline void Unzip(const std::vector<std::pair<First, Second>> &container,
                      std::vector<First> *first = nullptr,
                      std::vector<Second> *second = nullptr) {
        if (first != nullptr) {
            first->clear();
            first->reserve(container.size());
        }

        if (second != nullptr) {
            second->clear();
            second->reserve(container.size());
        }

        if (first == nullptr && second == nullptr) return;

        for (const auto &item: container) {
            if (first != nullptr) {
                first->emplace_back(item.first);
            }
            if (second != nullptr) {
                second->emplace_back(item.second);
            }
        }
    }

    template<typename First, typename Second>
    inline void SortByFirst(std::vector<First> &first, std::vector<Second> &second, bool descending = false) {
        auto container = Zip(first, second);
        if (descending) {
            std::sort(container.begin(), container.end(), [](const std::pair<First, Second> &lhs,
                                                             const std::pair<First, Second> &rhs) {
                return lhs.first > rhs.first;
            });
        } else {
            std::sort(container.begin(), container.end(), [](const std::pair<First, Second> &lhs,
                                                             const std::pair<First, Second> &rhs) {
                return lhs.first < rhs.first;
            });
        }
        Unzip<First, Second>(container, &first, &second);
    }

// Ranges object to support Python-like iteration. Use with `range()`.
    template<typename T>
    struct RangeHelper {
        struct Iterator {
            T pos = 0;

            Iterator &operator++() {
                ++pos;
                return *this;
            }

            bool operator!=(const Iterator &other) const { return pos != other.pos; }

            T operator*() const { return pos; }
        };

        RangeHelper(T min, T max) : begin_(min), end_(max) {}

        T begin_ = 0, end_ = 0;

        Iterator begin() const { return {begin_}; }

        Iterator end() const { return {end_}; }
    };

// Python `range()` equivalent. Construct an object to iterate over a sequence.
    template<typename T>
    inline auto Range(T min, T max) {
        return RangeHelper<T>{min, max};
    }


// Enumerate object to support Python-like enumeration. Use with `enumerate()`.
    template<typename T>
    struct EnumerateHelper {
        struct Iterator {
            T *data = nullptr;
            int64_t pos = 0;

            Iterator &operator++() {
                ++pos;
                return *this;
            }

            bool operator!=(const Iterator &other) const { return pos != other.pos; }

            std::pair<int64_t, T> operator*() const { return {pos, *(data + pos)}; }
        };

        T *data = nullptr;
        int64_t size = 0;

        Iterator begin() const { return {data, 0}; }

        Iterator end() const { return {data, size}; }
    };

// Python `enumerate()` equivalent. Construct an object that iteraterates over a
// sequence of elements and numbers them.
    template<typename T>
    inline auto Enumerate(const std::vector<T> &vals) {
        return EnumerateHelper<const T>{vals.data(), (int64_t) vals.size()};
    }

    template<typename T>
    inline auto Enumerate(std::vector<T> &vals) {
        return EnumerateHelper<T>{vals.data(), (int64_t) vals.size()};
    }

// Vector append and concatenation
    template<typename T>
    inline std::vector<T> &Append(std::vector<T> &a, const std::vector<T> &b) {
        a.insert(a.end(), b.begin(), b.end());
        return a;
    }

    template<typename T>
    inline std::vector<T> &Append(std::vector<T> &a, const T &b) {
        a.push_back(b);
        return a;
    }

    template<typename T>
    inline std::vector<T> Join(const std::vector<T> &a, const std::vector<T> &b) {
        auto c = a;
        return Append(c, b);
    }

    template<typename T>
    inline std::vector<T> Join(const std::vector<T> &a, const T &b) {
        auto c = a;
        return Append(c, b);
    }

}
#endif //ENGINE23_STLUTILS_H
