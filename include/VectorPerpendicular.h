//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_VECTORPERPENDICULAR_H
#define ENGINE23_VECTORPERPENDICULAR_H

#include "Eigen/Core"

namespace Bcg {
    template<typename T, int N>
    Eigen::Vector<T, N> FindPerpendicular(const Eigen::Vector<T, N> &v) {
        // Find a non-zero element in v. We will swap this with a zero (or just negate it)
        // to construct a vector that is guaranteed to be not parallel to v.
        Eigen::Vector<T, N> result = Eigen::Vector<T, N>::Zero();

        for (int i = 0; i < N; ++i) {
            if (v[i] != 0) {
                // To ensure robustness, check for the case where changing one element
                // wouldn't be enough to ensure orthogonality (e.g., [1, 1, 0]).
                if (i + 1 < N) {
                    // Swap the current element with the next one, then negate.
                    result[i] = v[i + 1];
                    result[i + 1] = -v[i];
                } else {
                    // For the last element, just negate the previous one if possible.
                    result[i] = -v[i - 1];
                    result[i - 1] = v[i];
                }
                break;
            }
        }

        return result;
    }
}

#endif //ENGINE23_VECTORPERPENDICULAR_H
