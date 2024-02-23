//
// Created by alex on 23.02.24.
//

#include "MeshVertexNormalsUniform.h"
#include "MeshFaceNormals.h"

namespace Bcg {
    Eigen::Vector<double, 3>
    MeshVertexNormalUniform(const Mesh &mesh, const VertexHandle &v, Property<Eigen::Vector<double, 3>> &face_normals) {
        Eigen::Vector<double, 3> normal = Eigen::Vector<double, 3>::Zero();

        for (auto f: mesh.get_faces(v)) {
            normal += face_normals[f];
        }

        return normal.normalized();
    }


    Property<Eigen::Vector<double, 3>> MeshVertexNormalsUniform(Mesh &mesh) {
        auto normals = mesh.vertices.get_or_add<Eigen::Vector<double, 3>>("v_normal");
        auto face_normals = mesh.faces.get<Eigen::Vector<double, 3>>("f_normal");
        if (!face_normals) {
            face_normals = MeshFaceNormals(mesh);
        }
        for (auto v: mesh.vertices) {
            normals[v] = MeshVertexNormalUniform(mesh, v, face_normals);
        }

        normals.set_dirty();
        return normals;
    }
}