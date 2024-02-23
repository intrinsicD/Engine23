//
// Created by alex on 23.02.24.
//


#include "MeshFaceAreaVectors.h"
#include "MeshFaceNormals.h"
#include "Eigen/Geometry"

namespace Bcg {
    Eigen::Vector<double, 3>
    MeshFaceNormal(const Mesh &mesh, const FaceHandle &f, const Property<Eigen::Vector<double, 3>> &positions) {
        return MeshFaceAreaVector(mesh, f, positions).normalized();
    }

    Property<Eigen::Vector<double, 3>> MeshFaceNormals(Mesh &mesh) {
        auto positions = mesh.vertices.get<Eigen::Vector<double, 3 >>("v_position");
        auto face_normals = mesh.faces.get_or_add<Eigen::Vector<double, 3 >>("f_normal");

        for(const auto f: mesh.faces){
            face_normals[f] = MeshFaceNormal(mesh, f, positions);
        }

        face_normals.set_dirty();
        return face_normals;
    }
}