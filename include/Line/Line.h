//
// Created by alex on 08.04.24.
//

#ifndef ENGINE23_LINE_H
#define ENGINE23_LINE_H

#include "Eigen/Core"

namespace Bcg {
    template<typename T, int N>
    class Line {
    public:
        Eigen::Vector<T, N> origin;
        Eigen::Vector<T, N> direction;

        Line() : origin(Eigen::Vector<T, N>::Zero()), direction(Eigen::Vector<T, N>::Unit(0)) {}

        Line(const Eigen::Vector<T, N> &origin, const Eigen::Vector<T, N> &direction) : origin(origin),
                                                                                        direction(
                                                                                                direction.normalized()) {}

        Eigen::Vector<T, N> point(double t) const {
            return origin + t * direction;
        }

        bool operator==(const Line &line) const {
            return origin.isApprox(line.origin) && direction.isApprox(line.direction);
        }

        bool operator!=(const Line &line) const {
            return !operator==(line);
        }

        bool operator<(const Line &line) const {
            if (origin < line.origin) {
                return true;
            }

            if (origin > line.origin) {
                return false;
            }

            return direction < line.direction;
        }

        bool operator<=(const Line &line) const {
            return !line.operator<(*this);
        }

        bool operator>(const Line &line) const {
            return line.operator<(*this);
        }
    };
}

#endif //ENGINE23_LINE_H
