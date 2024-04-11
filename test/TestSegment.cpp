//
// Created by alex on 10.04.24.
//

#include <gtest/gtest.h>

#include "SegmentClosestPoint.h"

TEST(Segment, ClosestPoint) {
    Bcg::Segment<double, 2> segment({0, 0}, {1, 1});
    Eigen::Vector2d point = {0, 1};
    Eigen::Vector2d closest = Bcg::ClosestPoint(segment, point);
    Eigen::Vector2d expected = {0.5, 0.5};
    EXPECT_EQ(closest, expected);
}

TEST(Segment, ClosestPoints) {
    Bcg::Segment<double, 2> segment1({0, 0}, {1, 1});
    Bcg::Segment<double, 2> segment2({0, 1}, {1, 0});
    auto [closest1, closest2] = Bcg::ClosestPoints(segment1, segment2);
    Eigen::Vector2d expected1 = {0.5, 0.5};
    Eigen::Vector2d expected2 = {0.5, 0.5};
    EXPECT_EQ(closest1, expected1);
    EXPECT_EQ(closest2, expected2);
}

TEST(Segment, ClosestPoints1) {
    Bcg::Segment<double, 2> segment1({0, 0}, {0, 0});
    Bcg::Segment<double, 2> segment2({0, 1}, {1, 0});
    auto [closest1, closest2] = Bcg::ClosestPoints(segment1, segment2);
    Eigen::Vector2d expected1 = {0, 0};
    Eigen::Vector2d expected2 = {0.5, 0.5};
    EXPECT_EQ(closest1, expected1);
    EXPECT_EQ(closest2, expected2);
}

TEST(Segment, ClosestPoints2) {
    Bcg::Segment<double, 2> segment1({0, 0}, {1, 1});
    Bcg::Segment<double, 2> segment2({0, 1}, {0, 1});
    auto [closest1, closest2] = Bcg::ClosestPoints(segment1, segment2);
    Eigen::Vector2d expected1 = {0.5, 0.5};
    Eigen::Vector2d expected2 = {0, 1};
    EXPECT_EQ(closest1, expected1);
    EXPECT_EQ(closest2, expected2);
}

#include "SegmentIntersectsAABB.h"

TEST(Segment, IntersectsAABB) {
    Bcg::Segment<double, 3> segment({0, 0, 0}, {1, 1, 1});
    Bcg::AABB<double, 3> aabb({0.5, 0.5, 0.5}, {1.5, 1.5, 1.5});
    EXPECT_TRUE(Bcg::Intersects(segment, aabb));
}

TEST(Segment, DoesNotIntersectAABB) {
    Bcg::Segment<double, 3> segment({0, 0, 0}, {1, 1, 1});
    Bcg::AABB<double, 3> aabb({1.5, 1.5, 1.5}, {2.5, 2.5, 2.5});
    EXPECT_FALSE(Bcg::Intersects(segment, aabb));
}

#include "SegmentIntersectsSphere.h"

TEST(Segment, IntersectsSphere) {
    Bcg::Segment<double, 3> segment({0, 0, 0}, {1, 1, 1});
    Bcg::Sphere<double, 3> sphere({0.5, 0.5, 0.5}, 1);
    EXPECT_TRUE(Bcg::Intersects(segment, sphere));
}

TEST(Segment, DoesNotIntersectSphere) {
    Bcg::Segment<double, 3> segment({0, 0, 0}, {1, 1, 1});
    Bcg::Sphere<double, 3> sphere({1.5, 1.5, 1.5}, 1);
    EXPECT_FALSE(Bcg::Intersects(segment, sphere));
}

#include "SegmentIntersectsTriangle.h"

TEST(Segment, IntersectsTriangle) {
    Bcg::Segment<double, 3> segment({0, 0, 0}, {1, 1, 1});
    Bcg::Triangle<double, 3> triangle({0.5, 0.5, 0.5}, {1.5, 0.5, 0.5}, {0.5, 1.5, 0.5});
    EXPECT_TRUE(Bcg::Intersects(segment, triangle));
}

TEST(Segment, DoesNotIntersectTriangle) {
    Bcg::Segment<double, 3> segment({0, 0, 0}, {1, 1, 1});
    Bcg::Triangle<double, 3> triangle({1.5, 1.5, 1.5}, {2.5, 1.5, 1.5}, {1.5, 2.5, 1.5});
    EXPECT_FALSE(Bcg::Intersects(segment, triangle));
}