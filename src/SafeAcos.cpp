//
// Created by alex on 23.02.24.
//

#include "SafeAcos.h"
#include <cmath>
#include <algorithm>

namespace Bcg{
    double SafeAcos(double value) {
        return std::acos(std::clamp(value, -1.0, 1.0));
    }
}