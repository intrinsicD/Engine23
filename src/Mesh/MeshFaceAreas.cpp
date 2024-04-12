//
// Created by alex on 23.02.24.
//

#include "MeshFaceAreas.h"
#include "MeshFaceAreaVectors.h"
#include "TriangleArea.h"

namespace Bcg {
    double MeshFaceArea(const Mesh &mesh, const FaceHandle &f, const Property<Eigen::Vector<double, 3>> &positions) {
        std::vector<Eigen::Vector<double, 3>> P;
        for (const auto &v: mesh.get_vertices(f)) {
            P.emplace_back(positions[v]);
        }
        double area = std::numeric_limits<double>::epsilon();
        if (P.size() == 3) {
            area = std::max(area, AreaFromMetric((P[1] - P[0]).norm(), (P[2] - P[1]).norm(), (P[0] - P[2]).norm()));
        }
        return std::max(area, MeshFaceAreaVector(mesh, f, positions).norm());
    }

    Property<double> MeshFaceAreas(Mesh &mesh) {
        auto areas = mesh.faces.get_or_add<double>("f_area");
        auto positions = mesh.vertices.get<Eigen::Vector<double, 3>>("v_position");

        for (const auto f: mesh.faces) {
            areas[f] = MeshFaceArea(mesh, f, positions);
        }

        areas.set_dirty();
        return areas;
    }

}