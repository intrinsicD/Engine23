//
// Created by alex on 19.02.24.
//

#ifndef ENGINE23_HANDLEBASE_H
#define ENGINE23_HANDLEBASE_H

#include <cstddef>

namespace Bcg {
    template<typename Derived>
    struct HandleBase {
        HandleBase() = default;

        virtual ~HandleBase() = default;

        HandleBase(const size_t idx) : idx(idx) {}

        size_t idx = size_t(-1);

        [[nodiscard]] bool is_valid() const {
            return idx != size_t(-1);
        }

        inline operator size_t() {
            return idx;
        }

        inline operator size_t() const {
            return idx;
        }

        template<typename T>
        inline Derived operator+(T a){
            return idx + a;
        }

        template<typename T>
        inline Derived operator-(T a){
            return idx - a;
        }

        inline void operator++() {
            ++idx;
        }

        inline void operator--() {
            --idx;
        }
    };
}

#endif //ENGINE23_HANDLEBASE_H
