//
// Created by alex on 10.04.24.
//

#ifndef ENGINE23_SEGMENTCLOSESTPOINT_H
#define ENGINE23_SEGMENTCLOSESTPOINT_H

#include "Segment.h"

namespace Bcg{
    template<typename T, int N>
    Eigen::Vector<T, N> ClosestPoint(const Segment<T, N> &segment, const Eigen::Vector<T, N> &point) {
        Eigen::Vector<T, N> ab = segment.end - segment.start;
        T t = (point - segment.start).dot(ab) / ab.squaredNorm();
        return segment.point(std::clamp(t, T(0), T(1)));
    }


    template<typename T, int N>
    std::array<Eigen::Vector<T, N>, 2> ClosestPoints(const Segment<T, N> &segment1, const Segment<T, N> &segment2) {
        Eigen::Vector<T, N> ab = segment1.end - segment1.start;
        Eigen::Vector<T, N> cd = segment2.end - segment2.start;
        Eigen::Vector<T, N> ac = segment2.start - segment1.start;
        T e = ab.dot(ab);
        T f = cd.dot(cd);
        T g = cd.dot(ac);
        if (e <= std::numeric_limits<T>::epsilon() && f <= std::numeric_limits<T>::epsilon()) {
            return {segment1.start, segment2.start};
        }
        if (e <= std::numeric_limits<T>::epsilon()) {
            return {segment1.start, segment2.point(g / f)};
        }
        T h = ab.dot(ac);
        if (f <= std::numeric_limits<T>::epsilon()) {
            return {segment1.point(h / e), segment2.start};
        }
        T t = (g * e - h * f) / (e * f - g * g);
        T s = (t * g + h) / e;
        return {segment1.point(std::clamp(t, T(0), T(1))), segment2.point(std::clamp(s, T(0), T(1)))};
    }
}

#endif //ENGINE23_SEGMENTCLOSESTPOINT_H
