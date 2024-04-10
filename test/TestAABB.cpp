//
// Created by alex on 08.04.24.
//

#include <gtest/gtest.h>

#include "AABBClosestPoint.h"

TEST(AABB, ClosestPoint) {
    Bcg::AABB<double, 3> aabb({0, 0, 0}, {1, 1, 1});
    Eigen::Vector<double, 3> point({2, 2, 2});
    Eigen::Vector<double, 3> closest_point = Bcg::ClosestPoint(aabb, point);
    Eigen::Vector<double, 3> expected_closest_point({1, 1, 1});
    EXPECT_EQ(closest_point, expected_closest_point);
}


#include "AABBContainsAABB.h"

TEST(AABB, ContainsAABB) {
    Bcg::AABB<double, 3> aabb1({0, 0, 0}, {1, 1, 1});
    Bcg::AABB<double, 3> aabb2({0.25, 0.25, 0.25}, {0.75, 0.75, 0.75});
    EXPECT_TRUE(Bcg::Contains(aabb1, aabb2));
}

TEST(AABB, DoesNotContainAABB) {
    Bcg::AABB<double, 3> aabb1({0, 0, 0}, {1, 1, 1});
    Bcg::AABB<double, 3> aabb2({0.25, 0.25, 0.25}, {1.25, 1.25, 1.25});
    EXPECT_FALSE(Bcg::Contains(aabb1, aabb2));
}

#include "AABBContainsPoint.h"

TEST(AABB, ContainsPoint) {
    Bcg::AABB<double, 3> aabb({0, 0, 0}, {1, 1, 1});
    Eigen::Vector<double, 3> point({0.5, 0.5, 0.5});
    EXPECT_TRUE(Bcg::Contains(aabb, point));
}

TEST(AABB, DoesNotContainPoint) {
    Bcg::AABB<double, 3> aabb({0, 0, 0}, {1, 1, 1});
    Eigen::Vector<double, 3> point({2, 2, 2});
    EXPECT_FALSE(Bcg::Contains(aabb, point));
}

#include "AABBIntersectsAABB.h"

TEST(AABB, IntersectsAABB) {
    Bcg::AABB<double, 3> aabb1({0, 0, 0}, {1, 1, 1});
    Bcg::AABB<double, 3> aabb2({0.5, 0.5, 0.5}, {1.5, 1.5, 1.5});
    EXPECT_TRUE(Bcg::Intersects(aabb1, aabb2));
}

TEST(AABB, DoesNotIntersectAABB) {
    Bcg::AABB<double, 3> aabb1({0, 0, 0}, {1, 1, 1});
    Bcg::AABB<double, 3> aabb2({2, 2, 2}, {3, 3, 3});
    EXPECT_FALSE(Bcg::Intersects(aabb1, aabb2));
}

#include "AABBIntersectsRay.h"

TEST(AABB, IntersectsRay) {
    Bcg::AABB<double, 3> aabb({0, 0, 0}, {1, 1, 1});
    Bcg::Ray<double, 3> ray({-1, -1, -1}, {1, 1, 1});
    EXPECT_TRUE(Bcg::Intersects(aabb, ray));
}

TEST(AABB, DoesNotIntersectRay) {
    Bcg::AABB<double, 3> aabb({0, 0, 0}, {1, 1, 1});
    Bcg::Ray<double, 3> ray({2, 2, 2}, {1, 0, 0});
    EXPECT_FALSE(Bcg::Intersects(aabb, ray));
}

#include "AABBIntersectsPlane.h"

TEST(AABB, IntersectsPlane) {
    Bcg::AABB<double, 3> aabb({0, 0, 0}, {1, 1, 1});
    Bcg::Plane<double, 3> plane({1, 0, 0}, {0.5, 0.5, 0.5});
    EXPECT_TRUE(Bcg::Intersects(aabb, plane));
}

TEST(AABB, DoesNotIntersectPlane) {
    Bcg::AABB<double, 3> aabb({0, 0, 0}, {1, 1, 1});
    Bcg::Plane<double, 3> plane({1, 0, 0}, {-0.5, -0.5, -0.5});
    EXPECT_FALSE(Bcg::Intersects(aabb, plane));
}

#include "AABBIntersectsSphere.h"

TEST(AABB, IntersectsSphere) {
    Bcg::AABB<double, 3> aabb({0, 0, 0}, {1, 1, 1});
    Bcg::Sphere<double, 3> sphere({0.5, 0.5, 0.5}, 0.5);
    EXPECT_TRUE(Bcg::Intersects(aabb, sphere));
}

TEST(AABB, DoesNotIntersectSphere) {
    Bcg::AABB<double, 3> aabb({0, 0, 0}, {1, 1, 1});
    Bcg::Sphere<double, 3> sphere({2, 2, 2}, 0.5);
    EXPECT_FALSE(Bcg::Intersects(aabb, sphere));
}

#include "AABBMortonSubdivide.h"

TEST(AABB, MortonSubdivide) {
    Bcg::AABB<double, 3> aabb({0, 0, 0}, {1, 1, 1});
    auto aabbs = Bcg::Subdivide(aabb);
    std::array<Bcg::AABB<double, 3>, 8> expected_aabbs = {
            Bcg::AABB<double, 3>({0, 0, 0}, {0.5, 0.5, 0.5}),
            Bcg::AABB<double, 3>({0.5, 0, 0}, {1, 0.5, 0.5}),
            Bcg::AABB<double, 3>({0, 0.5, 0}, {0.5, 1, 0.5}),
            Bcg::AABB<double, 3>({0.5, 0.5, 0}, {1, 1, 0.5}),
            Bcg::AABB<double, 3>({0, 0, 0.5}, {0.5, 0.5, 1}),
            Bcg::AABB<double, 3>({0.5, 0, 0.5}, {1, 0.5, 1}),
            Bcg::AABB<double, 3>({0, 0.5, 0.5}, {0.5, 1, 1}),
            Bcg::AABB<double, 3>({0.5, 0.5, 0.5}, {1, 1, 1})
    };
    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(aabbs[i], expected_aabbs[i]);
    }
}

TEST(AABB, MortonChildIndex) {
    Bcg::AABB<double, 3> aabb({0, 0, 0}, {1, 1, 1});
    Eigen::Vector<double, 3> point({0.5, 0.5, 0.5});
    size_t index = Bcg::MortonChildIndex(aabb, point);
    EXPECT_EQ(index, 0);
}

TEST(AABB, MortonChildCenter) {
    Eigen::Vector<double, 3> center({0, 0, 0});
    Eigen::Vector<double, 3> halfSize({0.5, 0.5, 0.5});
    Eigen::Vector<double, 3> child_center = Bcg::MortonChildCenter(center, halfSize, 0);
    Eigen::Vector<double, 3> expected_child_center({-0.25, -0.25, -0.25});
    EXPECT_EQ(child_center, expected_child_center);
}

TEST(AABB, MortonChildAABB) {
    Bcg::AABB<double, 3> aabb({0, 0, 0}, {1, 1, 1});
    Bcg::AABB<double, 3> child_aabb = Bcg::MortonChildAABB(aabb, 0);
    Bcg::AABB<double, 3> expected_child_aabb({0, 0, 0}, {0.5, 0.5, 0.5});
    EXPECT_EQ(child_aabb, expected_child_aabb);
}

#include "AABBSurfaceArea.h"

TEST(AABB, SurfaceArea) {
    Bcg::AABB<double, 3> aabb({0, 0, 0}, {1, 1, 1});
    double surface_area = Bcg::SurfaceArea(aabb);
    EXPECT_EQ(surface_area, 6);
}

#include "AABBVolume.h"

TEST(AABB, Volume) {
    Bcg::AABB<double, 3> aabb({0, 0, 0}, {1, 1, 1});
    double volume = Bcg::Volume(aabb);
    EXPECT_EQ(volume, 1);
}