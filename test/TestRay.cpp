//
// Created by alex on 08.04.24.
//


#include <gtest/gtest.h>

#include "RayClosestPoint.h"
#include "VectorPerpendicular.h"

TEST(Ray, ClosestPoint) {
    Bcg::Ray<double, 3> ray({0, 0, 0}, {1, 1, 1});
    auto perp = Bcg::FindPerpendicular(ray.direction);
    Eigen::Vector<double, 3> expected_closest_point({0.5, 0.5, 0.5});
    Eigen::Vector<double, 3> point(expected_closest_point + perp);
    Eigen::Vector<double, 3> closest_point = Bcg::ClosestPoint(ray, point);
    EXPECT_EQ(closest_point, expected_closest_point);
}

TEST(Ray, ClosestPointBetweenTwoRays) {
    Bcg::Ray<double, 3> ray1({0, 0, -1}, {1, 0, 0});
    Bcg::Ray<double, 3> ray2({0, 0, 1}, {0, 1, 0});
    Eigen::Vector<double, 3> expected_closest_point1(ray1.origin);
    Eigen::Vector<double, 3> expected_closest_point2(ray2.origin);
    auto closest_points = Bcg::ClosestPoints(ray1, ray2);
    EXPECT_EQ(closest_points[0], expected_closest_point1);
    EXPECT_EQ(closest_points[1], expected_closest_point2);
}

#include "RayIntersectsAABB.h"

TEST(Ray, IntersectsAABB) {
    Bcg::Ray<double, 3> ray({0, 0, 0}, {1, 1, 1});
    Bcg::AABB<double, 3> aabb({0.5, 0.5, 0.5}, {1.5, 1.5, 1.5});
    EXPECT_TRUE(Bcg::Intersects(ray, aabb));
}

TEST(Ray, DoesNotIntersectAABB) {
    Bcg::AABB<double, 3> aabb({2, 2, 2}, {3, 3, 3});
    Eigen::Vector<double, 3> direction = (aabb.max - aabb.min).normalized();
    auto perp = Bcg::FindPerpendicular(direction);
    Bcg::Ray<double, 3> ray({0, 0, 0}, perp);
    EXPECT_FALSE(Bcg::Intersects(ray, aabb));
}

#include "RayIntersectsSphere.h"

TEST(Ray, IntersectsSphere) {
    Bcg::Ray<double, 3> ray({0, 0, 0}, {1, 1, 1});
    Bcg::Sphere<double, 3> sphere({0.5, 0.5, 0.5}, 0.5);
    EXPECT_TRUE(Bcg::Intersects(ray, sphere));
}

TEST(Ray, DoesNotIntersectSphere) {
    Bcg::Ray<double, 3> ray({0, 0, 0}, {1, 1, 1});
    Bcg::Sphere<double, 3> sphere({0, 1, 0}, 0.5);
    EXPECT_FALSE(Bcg::Intersects(ray, sphere));
}