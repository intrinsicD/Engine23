//
// Created by alex on 23.02.24.
//

#include "MeshVertexNormalsAreaAngleHybrid.h"
#include "MeshFaceNormals.h"
#include "MeshFaceAreas.h"
#include "SafeAngle.h"

namespace Bcg {
    Eigen::Vector<double, 3> MeshVertexNormalAreaAngleHybrid(const Mesh &mesh, const VertexHandle &v,
                                                             const Property<Eigen::Vector<double, 3>> &face_normals,
                                                             const Property<double> &face_areas,
                                                             const Property<Eigen::Vector<double, 3>> &positions) {
        Eigen::Vector<double, 3> normal = Eigen::Vector<double, 3>::Zero();

        for (const auto h: mesh.Graph::get_halfedges(v)) {
            auto hr = mesh.rotate_ccw(h);

            Eigen::Vector<double, 3> a = positions[mesh.get_to_vertex(h)] - positions[v];
            Eigen::Vector<double, 3> b = positions[mesh.get_to_vertex(hr)] - positions[v];
            auto f = mesh.get_face(h);
            normal += face_normals[f] * SafeAngle(a, b) * face_areas[f];
        }
        return normal.normalized();
    }

    Property<Eigen::Vector<double, 3>> MeshVertexNormalsAreaAngleHybrid(Mesh &mesh) {
        auto normals = mesh.vertices.get_or_add<Eigen::Vector<double, 3>>("v_normal");
        auto positions = mesh.vertices.get<Eigen::Vector<double, 3>>("v_position");
        auto face_normals = mesh.faces.get<Eigen::Vector<double, 3>>("f_normal");
        auto face_areas = mesh.faces.get<double>("f_area");

        if (!face_normals) {
            face_normals = MeshFaceNormals(mesh);
        }

        if (!face_areas) {
            face_areas = MeshFaceAreas(mesh);
        }

        for (const auto v: mesh.vertices) {
            normals[v] = MeshVertexNormalAreaAngleHybrid(mesh, v, face_normals, face_areas, positions);
        }

        normals.set_dirty();
        return normals;
    }
}