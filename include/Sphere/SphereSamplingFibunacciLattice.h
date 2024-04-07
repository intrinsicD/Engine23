//
// Created by alex on 07.04.24.
//

#ifndef ENGINE23_SPHERESAMPLINGFIBUNACCILATTICE_H
#define ENGINE23_SPHERESAMPLINGFIBUNACCILATTICE_H

#include "Sphere.h"

namespace Bcg {
    enum FibunacciLattice {
        FLNAIVE,
        FLFIRST,
        FLSECOND,
        FLTHIRD,
        FLOFFSET
    };

    template<typename T>
    Eigen::Vector<T, 3> LatticePoint(size_t i, size_t num_samples, T golden_ratio, T TWOPI, T index_offset,
                                     T sample_count_offset, const Sphere<T, 3> &sphere) {
        T x = (i + index_offset) / T(num_samples + sample_count_offset);
        T y = T(i) / golden_ratio;
        T phi = std::acos(1.0 - 2.0 * x);
        T theta = TWOPI * y;
        Eigen::Vector<T, 3> point = {std::cos(theta) * std::sin(phi), std::sin(theta) * std::sin(phi),
                                     std::cos(phi)};
        return point * sphere.radius + sphere.center;
    }

    //http://extremelearning.com.au/evenly-distributing-points-on-a-sphere/
    template<typename T>
    std::vector<Eigen::Vector<T, 3>> SampleFibunacciLattice(const Sphere<T, 3> &sphere, size_t num_samples,
                                                            FibunacciLattice type = FLTHIRD) {
        //http://extremelearning.com.au/evenly-distributing-points-on-a-sphere/
        std::vector<Eigen::Vector<T, 3>> points;
        points.reserve(num_samples);
        T golden_ratio = (1.0 + std::sqrt(5.0)) / 2.0;
        T TWOPI = 2 * M_PI;
        T epsilon = 0.36;
        size_t start_index = 0;
        size_t end_index = num_samples;
        T offset = 0.0;
        T sample_count_offset = 0.0;
        switch (type) {
            default:
            case FLNAIVE: {
                offset = 0.0;
                sample_count_offset = 0.0;
                break;
            }
            case FLFIRST: {
                offset = 0.5;
                sample_count_offset = 0.0;
                break;
            }
            case FLSECOND: {
                offset = 1.5;
                sample_count_offset = 2 * offset;
                break;
            }
            case FLTHIRD: {
                offset = 3.5;
                sample_count_offset = 2 * offset;
                start_index = 1;
                end_index = num_samples - 1;
                break;
            }
            case FLOFFSET: {
                offset = epsilon;
                sample_count_offset = 2 * offset - 1;
                start_index = 1;
                end_index = num_samples - 1;
                break;
            }
        }

        if (type == FLTHIRD || type == FLOFFSET) {
            points.emplace_back(Eigen::Vector<T, 3>(0, 0, 1) * sphere.radius + sphere.center);
        }

        for (size_t i = start_index; i < end_index; ++i) {
            points.emplace_back(
                    LatticePoint(i, num_samples, golden_ratio, TWOPI, offset, sample_count_offset, sphere));
        }
        if (type == FLTHIRD || type == FLOFFSET) {
            points.emplace_back(Eigen::Vector<T, 3>(0, 0, -1) * sphere.radius + sphere.center);
        }
        return points;
    }
}

#endif //ENGINE23_SPHERESAMPLINGFIBUNACCILATTICE_H
