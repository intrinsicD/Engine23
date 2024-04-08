//
// Created by alex on 08.04.24.
//


#include <gtest/gtest.h>

#include "PlaneClosestPoint.h"

TEST(Plane, ClosestPoint) {
    Bcg::Plane<double, 3> plane({0, 0, 0}, {1, 1, 1});
    Eigen::Vector<double, 3> point({0.5, 0.5, 0.5});
    Eigen::Vector<double, 3> expected_closest_point({0.5, 0.5, 0.5});
    Eigen::Vector<double, 3> closest_point = Bcg::PlaneClosestPoint(plane, point);
    EXPECT_EQ(closest_point, expected_closest_point);
}

#include "PlaneIntersectsAABB.h"

TEST(Plane, IntersectsAABB) {
    Bcg::Plane<double, 3> plane({0, 0, 0}, {1, 1, 1});
    Bcg::AABB<double, 3> aabb({0.5, 0.5, 0.5}, {1.5, 1.5, 1.5});
    EXPECT_TRUE(Bcg::Intersects(plane, aabb));
}

TEST(Plane, DoesNotIntersectAABB) {
    Bcg::AABB<double, 3> aabb({2, 2, 2}, {3, 3, 3});
    Bcg::Plane<double, 3> plane({1, 1, 1}, {0, 0, 0});
    EXPECT_FALSE(Bcg::Intersects(plane, aabb));
}

#include "PlaneIntersectsLine.h"
#include "VectorPerpendicular.h"

TEST(Plane, IntersectsLine) {
    Bcg::Plane<double, 3> plane({1, 1, 1}, {0, 0, 0});
    Bcg::Line<double, 3> line({0.5, 0.5, 0.5}, {1, 1, 1});
    EXPECT_TRUE(Bcg::Intersects(plane, line));
}

TEST(Plane, DoesNotIntersectLine) {
    Bcg::Plane<double, 3> plane({1, 1, 1}, {0, 0, 0});
    auto perp = Bcg::FindPerpendicular(plane.normal);
    Bcg::Line<double, 3> line({0, 0, 0}, perp);
    EXPECT_FALSE(Bcg::Intersects(plane, line));
}

#include "PlaneIntersectsPlane.h"

TEST(Plane, IntersectsPlane) {
    Bcg::Plane<double, 3> plane1({0, 0, 0}, {1, 1, 1});
    Bcg::Plane<double, 3> plane2({0.5, 0.5, 0.5}, {1, 1, 1});
    EXPECT_TRUE(Bcg::Intersects(plane1, plane2));
}

TEST(Plane, DoesNotIntersectPlane) {
    Bcg::Plane<double, 3> plane1({1, 1, 1}, {0, 0, 0});
    Bcg::Plane<double, 3> plane2({1, 1, 1}, {0.1, 0.1, 0.1});
    EXPECT_FALSE(Bcg::Intersects(plane1, plane2));
}

#include "PlaneIntersectsSphere.h"

TEST(Plane, IntersectsSphere) {
    Bcg::Plane<double, 3> plane({0, 0, 0}, {1, 1, 1});
    Bcg::Sphere<double, 3> sphere({0.5, 0.5, 0.5}, 0.5);
    EXPECT_TRUE(Bcg::Intersects(plane, sphere));
}

TEST(Plane, DoesNotIntersectSphere) {
    Bcg::Plane<double, 3> plane({1, 1, 1}, {0, 0, 0});
    Bcg::Sphere<double, 3> sphere({0, 1, 0}, 0.5);
    EXPECT_FALSE(Bcg::Intersects(plane, sphere));
}

#include "PlaneLeastSquaresFit.h"

TEST(Plane, LeastSquaresFit) {
    Eigen::Matrix<double, 3, 3> points;
    points << 0, 0, 0,
              1, 0, 0,
              0, 1, 0;

    Bcg::Plane<double, 3> plane = Bcg::PlaneLeastSquaresFit(points);
    Eigen::Vector<double, 3> expected_normal({0, 0, 1});
    EXPECT_EQ(plane.normal, expected_normal);
    Eigen::Vector<double, 3> point = plane.point();
    Eigen::Vector<double, 3> expected_point({0, 0, 0});
    EXPECT_EQ(point, expected_point);
}