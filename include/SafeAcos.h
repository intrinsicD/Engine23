//
// Created by alex on 23.02.24.
//

#ifndef ENGINE23_SAFEACOS_H
#define ENGINE23_SAFEACOS_H

#include <cmath>

namespace Bcg {
    template<typename T>
    T SafeAcos(T value) {
        return std::acos((value > -1 ? value : -1) < 1 ? value : 1);
    }
}

#endif //ENGINE23_SAFEACOS_H
