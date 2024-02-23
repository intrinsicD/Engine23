//
// Created by alex on 23.02.24.
//
#include "MeshVertexNormalsAngleWeighted.h"
#include "MeshFaceNormals.h"
#include "SafeAngle.h"

namespace Bcg {
    Eigen::Vector<double, 3> MeshVertexNormalAngleWeighted(const Mesh &mesh, const VertexHandle &v,
                                                           const Property<Eigen::Vector<double, 3>> &face_normals,
                                                           const Property<Eigen::Vector<double, 3>> &positions) {
        Eigen::Vector<double, 3> normal = Eigen::Vector<double, 3>::Zero();

        for (const auto h: mesh.Graph::get_halfedges(v)) {
            auto hr = mesh.rotate_ccw(h);

            Eigen::Vector<double, 3> a = positions[mesh.get_to_vertex(h)] - positions[v];
            Eigen::Vector<double, 3> b = positions[mesh.get_to_vertex(hr)] - positions[v];

            normal += face_normals[mesh.get_face(h)] * SafeAngle(a, b);
        }
        return normal.normalized();
    }

    Property<Eigen::Vector<double, 3>> MeshVertexNormalsAngleWeighted(Mesh &mesh) {
        auto normals = mesh.vertices.get_or_add<Eigen::Vector<double, 3>>("v_normal");
        auto positions = mesh.vertices.get<Eigen::Vector<double, 3>>("v_position");
        auto face_normals = mesh.faces.get<Eigen::Vector<double, 3>>("f_normal");

        if (!face_normals) {
            face_normals = MeshFaceNormals(mesh);
        }

        for (const auto v: mesh.vertices) {
            normals[v] = MeshVertexNormalAngleWeighted(mesh, v, face_normals, positions);
        }

        normals.set_dirty();
        return normals;
    }
}