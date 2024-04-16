//
// Created by alex on 16.04.24.
//

#ifndef ENGINE23_DEGREESTORADIANS_H
#define ENGINE23_DEGREESTORADIANS_H

#include <numbers>

namespace Bcg{
    template<typename T>
    T DegreesToRadians(T fovy_degrees) {
        return fovy_degrees * std::numbers::pi / T(360.0);
    }
}

#endif //ENGINE23_DEGREESTORADIANS_H
