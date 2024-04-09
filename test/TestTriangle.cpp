//
// Created by alex on 09.04.24.
//

#include <gtest/gtest.h>

#include "TriangleClosestPoint.h"

TEST(Triangle, ClosestPoint) {
    Bcg::Triangle<double, 3> triangle({0, 0, 0},
                                      {1, 0, 0},
                                      {0, 1, 0});

    Eigen::Vector<double, 3> expected_closest_point = triangle.points.row(0);
    Eigen::Vector<double, 3> closest_point = Bcg::ClosestPoint(triangle, expected_closest_point);
    EXPECT_TRUE(closest_point.isApprox(expected_closest_point));

    expected_closest_point = triangle.points.row(1);
    closest_point = Bcg::ClosestPoint(triangle, expected_closest_point);
    EXPECT_TRUE(closest_point.isApprox(expected_closest_point));

    expected_closest_point = triangle.points.row(2);
    closest_point = Bcg::ClosestPoint(triangle, expected_closest_point);
    EXPECT_TRUE(closest_point.isApprox(expected_closest_point));

    expected_closest_point = triangle.points.colwise().mean().transpose();
    Eigen::Vector<double, 3> normal = triangle.normal();
    Eigen::Vector<double, 3> expected_normal(0, 0, 1);
    EXPECT_TRUE(normal.isApprox(expected_normal));
    Eigen::Vector<double, 3> query_point = expected_closest_point + normal;
    closest_point = Bcg::ClosestPoint(triangle, query_point);
    EXPECT_TRUE(closest_point.isApprox(expected_closest_point));
}

#include "TriangleBarycentricCoordinates.h"

TEST(Triangle, BarycentricCoordinates){
    Eigen::Vector<double, 3> expected_p0 = {0, 0, 0};
    Eigen::Vector<double, 3> expected_p1 = {1, 0, 0};
    Eigen::Vector<double, 3> expected_p2 = {0, 1, 0};
    Bcg::Triangle<double, 3> triangle(expected_p0, expected_p1, expected_p2);

    Eigen::Vector<double, 3> expected_barycentric_coordinates(1.0, 0.0, 0.0);
    Eigen::Vector<double, 3> barycentric_coordinates = Bcg::ToBarycentricCoordinates(triangle, expected_p0);
    Eigen::Vector<double, 3> p0 = Bcg::FromBarycentricCoordinates(triangle, barycentric_coordinates);
    EXPECT_EQ(barycentric_coordinates, expected_barycentric_coordinates);
    EXPECT_EQ(p0, expected_p0);

    expected_barycentric_coordinates = Eigen::Vector<double, 3>(0.0, 1.0, 0.0);
    barycentric_coordinates = Bcg::ToBarycentricCoordinates(triangle, expected_p1);
    Eigen::Vector<double, 3> p1 = Bcg::FromBarycentricCoordinates(triangle, barycentric_coordinates);
    EXPECT_EQ(barycentric_coordinates, expected_barycentric_coordinates);
    EXPECT_EQ(p1, expected_p1);

    expected_barycentric_coordinates = Eigen::Vector<double, 3>(0.0, 0.0, 1.0);
    barycentric_coordinates = Bcg::ToBarycentricCoordinates(triangle, expected_p2);
    Eigen::Vector<double, 3> p2 = Bcg::FromBarycentricCoordinates(triangle, barycentric_coordinates);
    EXPECT_EQ(barycentric_coordinates, expected_barycentric_coordinates);
    EXPECT_EQ(p2, expected_p2);

    expected_barycentric_coordinates = Eigen::Vector<double, 3>(1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0);
    Eigen::Vector<double, 3> query_point = triangle.points.colwise().mean().transpose();
    barycentric_coordinates = Bcg::ToBarycentricCoordinates(triangle, query_point);
    Eigen::Vector<double, 3> query_point_reconstructed = Bcg::FromBarycentricCoordinates(triangle, barycentric_coordinates);
    EXPECT_TRUE(barycentric_coordinates.isApprox(expected_barycentric_coordinates));
    EXPECT_EQ(query_point_reconstructed, query_point);
}