//
// Created by alex on 10.04.24.
//

#ifndef ENGINE23_SEGMENTCLOSESTPOINT_H
#define ENGINE23_SEGMENTCLOSESTPOINT_H

#include "Segment.h"

namespace Bcg {
    template<typename T, int N>
    Eigen::Vector<T, N> ClosestPoint(const Segment<T, N> &segment, const Eigen::Vector<T, N> &point) {
        Eigen::Vector<T, N> ab = segment.end - segment.start;
        T t = (point - segment.start).dot(ab) / ab.squaredNorm();
        return segment.point(std::clamp(t, T(0), T(1)));
    }

    template<typename T, int N>
    std::array<Eigen::Vector<T, N>, 2> ClosestPoints1(const Segment<T, N> &segment1, const Segment<T, N> &segment2) {
        Eigen::Vector<T, N> ab = segment1.end - segment1.start;
        Eigen::Vector<T, N> cd = segment2.end - segment2.start;
        Eigen::Vector<T, N> ac = segment2.start - segment1.start;

        T f = cd.dot(cd);
        T e = ab.dot(ab);
        T d = cd.dot(ab);
        T h = ac.dot(ab);
        T g = ac.dot(cd);
        T denom = d * d - f * e;

        T s, t;
        if (std::abs(denom) < std::numeric_limits<T>::epsilon()) {
            s = 0;
            t = (e * s - h) / d;
        } else {
            s = (g * d - f * h) / denom;
            t = (-h * d + e * g) / denom;
        }

        s = std::clamp(s, T(0), T(1));
        t = std::clamp(t, T(0), T(1));

        auto p_a = segment1.start + s * ab;
        auto p_b = segment2.start + t * cd;

        return {p_a, p_b};
    }

    template<typename T, int N>
    std::array<Eigen::Vector<T, N>, 2> ClosestPoints(const Segment<T, N> &segment1, const Segment<T, N> &segment2) {
        Eigen::Vector<T, N> ab = segment1.end - segment1.start;
        Eigen::Vector<T, N> cd = segment2.end - segment2.start;
        Eigen::Vector<T, N> ac = segment2.start - segment1.start;
        T f = cd.dot(cd);
        T e = ab.dot(ab);
        T d = cd.dot(ab);
        T g = ac.dot(cd);
        T denom = d * d - f * e;
        if (e <= std::numeric_limits<T>::epsilon() && f <= std::numeric_limits<T>::epsilon()) {
            return {segment1.start, segment2.start};
        }
        if (e <= std::numeric_limits<T>::epsilon()) {
            return {segment1.start, segment2.point(-g / f)};
        }
        T h = ac.dot(ab);
        if (f <= std::numeric_limits<T>::epsilon()) {
            return {segment1.point(h / e), segment2.start};
        }
        T s = (g * d - f * h) / denom;
        T t = (-h * d + e * g) / denom;
        return {segment1.point(std::clamp(s, T(0), T(1))), segment2.point(std::clamp(t, T(0), T(1)))};
    }
}

#endif //ENGINE23_SEGMENTCLOSESTPOINT_H
