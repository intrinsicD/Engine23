//
// Created by alex on 10.04.24.
//

#ifndef ENGINE23_SEGMENT_H
#define ENGINE23_SEGMENT_H

#include "Eigen/Core"

namespace Bcg{
    template<typename T, int N>
    class Segment {
        public:
        Eigen::Vector<T, N> start;
        Eigen::Vector<T, N> end;

        Segment() : start(Eigen::Vector<T, N>::Zero()), end(Eigen::Vector<T, N>::Unit(0)) {}

        Segment(const Eigen::Vector<T, N> &start, const Eigen::Vector<T, N> &end) : start(start), end(end) {}

        Eigen::Vector<T, N> point(T t) const {
            return start + t * (end - start);
        }

        bool operator==(const Segment &segment) const {
            return start == segment.start && end == segment.end;
        }

        bool operator!=(const Segment &segment) const {
            return !operator==(segment);
        }

        bool operator<(const Segment &segment) const {
            if (start < segment.start) {
                return true;
            }

            if (start > segment.start) {
                return false;
            }

            return end < segment.end;
        }

        bool operator<=(const Segment &segment) const {
            return !segment.operator<(*this);
        }

        bool operator>(const Segment &segment) const {
            return segment.operator<(*this);
        }

        bool operator>=(const Segment &segment) const {
            return !operator<(segment);
        }
    };
}

#endif //ENGINE23_SEGMENT_H
