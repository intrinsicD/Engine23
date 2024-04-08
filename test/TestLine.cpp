//
// Created by alex on 08.04.24.
//


#include <gtest/gtest.h>

#include "LineClosestPoint.h"
#include "VectorPerpendicular.h"

TEST(Line, ClosestPoint) {
    Bcg::Line<double, 3> line({0, 0, 0}, {1, 1, 1});
    auto perp = Bcg::FindPerpendicular(line.direction);
    Eigen::Vector<double, 3> expected_closest_point({0.5, 0.5, 0.5});
    Eigen::Vector<double, 3> point(expected_closest_point + perp);
    Eigen::Vector<double, 3> closest_point = Bcg::LineClosestPoint(line, point);
    EXPECT_EQ(closest_point, expected_closest_point);
}

#include "LineIntersectsAABB.h"

TEST(Line, IntersectsAABB) {
    Bcg::Line<double, 3> line({0, 0, 0}, {1, 1, 1});
    Bcg::AABB<double, 3> aabb({0.5, 0.5, 0.5}, {1.5, 1.5, 1.5});
    EXPECT_TRUE(Bcg::Intersects(line, aabb));
}

TEST(Line, DoesNotIntersectAABB) {
    Bcg::AABB<double, 3> aabb({2, 2, 2}, {3, 3, 3});
    Eigen::Vector<double, 3> direction = (aabb.max - aabb.min).normalized();
    auto perp = Bcg::FindPerpendicular(direction);
    Bcg::Line<double, 3> line({0, 0, 0}, perp);
    EXPECT_FALSE(Bcg::Intersects(line, aabb));
}

#include "LineIntersectsSphere.h"

TEST(Line, IntersectsSphere) {
    Bcg::Line<double, 3> line({0, 0, 0}, {1, 1, 1});
    Bcg::Sphere<double, 3> sphere({0.5, 0.5, 0.5}, 0.5);
    EXPECT_TRUE(Bcg::Intersects(line, sphere));
}

TEST(Line, DoesNotIntersectSphere) {
    Bcg::Line<double, 3> line({0, 0, 0}, {1, 1, 1});
    Bcg::Sphere<double, 3> sphere({0, 1, 0}, 0.5);
    EXPECT_FALSE(Bcg::Intersects(line, sphere));
}