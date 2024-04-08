//
// Created by alex on 08.04.24.
//

#include <gtest/gtest.h>

#include "SphereClosestPoint.h"
#include "Time.h"

TEST(Spheret, ClosestPoint) {
    Bcg::Sphere<double, 3> sphere({0, 0, 0}, 1);
    Eigen::Vector<double, 3> point(1, 0, 0);
    Eigen::Vector<double, 3> closest_point = Bcg::ClosestPoint(sphere, point);
    Eigen::Vector<double, 3> expected_closest_point = {1, 0, 0};

    EXPECT_EQ(closest_point, expected_closest_point);
}

#include "SphereContainsAABB.h"

TEST(Sphere, ContainsAABB) {
    Bcg::Sphere<double, 3> sphere({0, 0, 0}, 1);
    Bcg::AABB<double, 3> aabb({-1, -1, -1}, {1, 1, 1});
    EXPECT_TRUE(Bcg::Contains(sphere, aabb));
}

TEST(Sphere, DoesNotContainAABB) {
    Bcg::Sphere<double, 3> sphere({0, 0, 0}, 1);
    Bcg::AABB<double, 3> aabb({1, 1, 1}, {2, 2, 2});
    EXPECT_FALSE(Bcg::Contains(sphere, aabb));
}

#include "SphereContainsPoint.h"

TEST(Sphere, ContainsPoint) {
    Bcg::Sphere<double, 3> sphere({0, 0, 0}, 1);
    Eigen::Vector<double, 3> point(0.5, 0.5, 0.5);
    EXPECT_TRUE(Bcg::Contains(sphere, point));
}

TEST(Sphere, DoesNotContainPoint) {
    Bcg::Sphere<double, 3> sphere({0, 0, 0}, 1);
    Eigen::Vector<double, 3> point(1, 1, 1);
    EXPECT_FALSE(Bcg::Contains(sphere, point));
}

#include "SphereContainsSphere.h"

TEST(Sphere, ContainsSphere) {
    Bcg::Sphere<double, 3> sphere1({0, 0, 0}, 2);
    Bcg::Sphere<double, 3> sphere2({0, 0, 0}, 1);
    EXPECT_TRUE(Bcg::Contains(sphere1, sphere2));
}

TEST(Sphere, DoesNotContainSphere) {
    Bcg::Sphere<double, 3> sphere1({0, 0, 0}, 1);
    Bcg::Sphere<double, 3> sphere2({0, 0, 0}, 2);
    EXPECT_FALSE(Bcg::Contains(sphere1, sphere2));
}

#include "SphereIntersectsAABB.h"

TEST(Sphere, IntersectsAABB) {
    Bcg::Sphere<double, 3> sphere({0, 0, 0}, 1);
    Bcg::AABB<double, 3> aabb({0.5, 0.5, 0.5}, {1.5, 1.5, 1.5});
    EXPECT_TRUE(Bcg::Intersects(sphere, aabb));
}

TEST(Sphere, DoesNotIntersectAABB) {
    Bcg::Sphere<double, 3> sphere({0, 0, 0}, 1);
    Bcg::AABB<double, 3> aabb({2, 2, 2}, {3, 3, 3});
    EXPECT_FALSE(Bcg::Intersects(sphere, aabb));
}

#include "SphereIntersectsLine.h"

TEST(Sphere, IntersectsLine) {
    Bcg::Sphere<double, 3> sphere({0, 0, 0}, 1);
    Bcg::Line<double, 3> line({0.5, 0.5, 0.5}, {1, 1, 1});
    EXPECT_TRUE(Bcg::Intersects(sphere, line));
}

TEST(Sphere, DoesNotIntersectLine) {
    Bcg::Sphere<double, 3> sphere({2, 0, 0}, 1);
    Bcg::Line<double, 3> line({2, 2, 2}, {3, 3, 3});
    EXPECT_FALSE(Bcg::Intersects(sphere, line));
}

#include "SphereIntersectsSphere.h"

TEST(Sphere, IntersectsSphere) {
    Bcg::Sphere<double, 3> sphere1({0, 0, 0}, 1);
    Bcg::Sphere<double, 3> sphere2({1, 0, 0}, 1);
    EXPECT_TRUE(Bcg::Intersects(sphere1, sphere2));
}

TEST(Sphere, DoesNotIntersectSphere) {
    Bcg::Sphere<double, 3> sphere1({0, 0, 0}, 1);
    Bcg::Sphere<double, 3> sphere2({3, 0, 0}, 1);
    EXPECT_FALSE(Bcg::Intersects(sphere1, sphere2));
}

#include "SphereIterativeFit.h"
#include "SphereSamplingFibunacciLattice.h"

TEST(Sphere, IterativeFit) {
    Bcg::Time::Point start = Bcg::Time::Point::Now();
    auto expected_sphere = Bcg::Sphere<double, 3>({0.5, 0.3, -2.6}, 1.2);
    Eigen::Matrix<double, -1, 3> points = Bcg::SampleFibunacciLattice(expected_sphere, 1000);

    Bcg::Sphere<double, 3> sphere = Bcg::IterativeFit(points, 100);
    Bcg::Time::Point end = Bcg::Time::Point::Now();
    std::cout << "IterativeFit: " << end.duration(start) << std::endl;
    EXPECT_EQ(sphere.center, expected_sphere.center);
    EXPECT_EQ(sphere.radius, expected_sphere.radius);
}

#include "SphereLeastSquaresFit.h"

TEST(Sphere, SphereLeastSquaresFitSvd) {
    Bcg::Time::Point start = Bcg::Time::Point::Now();
    auto expected_sphere = Bcg::Sphere<double, 3>({0.5, 0.3, -2.6}, 1.2);
    Eigen::Matrix<double, -1, 3> points = Bcg::SampleFibunacciLattice(expected_sphere, 1000);

    Bcg::Sphere<double, 3> sphere = Bcg::SphereLeastSquaresFitSvd(points);
    Bcg::Time::Point end = Bcg::Time::Point::Now();
    std::cout << "SphereLeastSquaresFitSvd: " << end.duration(start) << std::endl;
    EXPECT_EQ(sphere.center, expected_sphere.center);
    EXPECT_EQ(sphere.radius, expected_sphere.radius);
}

TEST(Sphere, SphereLeastSquaresFitFast) {
    Bcg::Time::Point start = Bcg::Time::Point::Now();
    auto expected_sphere = Bcg::Sphere<double, 3>({0.5, 0.3, -2.6}, 1.2);
    Eigen::Matrix<double, -1, 3> points = Bcg::SampleFibunacciLattice(expected_sphere, 1000);

    Bcg::Sphere<double, 3> sphere = Bcg::SphereLeastSquaresFitFast(points);
    Bcg::Time::Point end = Bcg::Time::Point::Now();
    std::cout << "SphereLeastSquaresFitFast: " << end.duration(start) << std::endl;
    EXPECT_EQ(sphere.center, expected_sphere.center);
    EXPECT_EQ(sphere.radius, expected_sphere.radius);
}

