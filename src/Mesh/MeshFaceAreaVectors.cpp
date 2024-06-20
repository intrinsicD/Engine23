//
// Created by alex on 23.02.24.
//

#include "MeshFaceAreaVectors.h"
#include "Eigen/Geometry"

namespace Bcg {
    Eigen::Vector<double, 3>
    MeshFaceAreaVector(const Mesh &mesh, const FaceHandle &f, const Property<Eigen::Vector<double, 3>> &positions) {
        Eigen::Vector<double, 3> vector_area = Eigen::Vector<double, 3>::Zero();
        for (const auto h: mesh.get_halfedges(f)) {
            vector_area += positions[mesh.get_from_vertex(h)].cross(positions[mesh.get_to_vertex(h)]);
        }
        return vector_area / 2.0;
    }


    Property<Eigen::Vector<double, 3>> MeshFaceAreaVectors(Mesh &mesh) {
        auto positions = mesh.vertices.get<Eigen::Vector<double, 3 >>("v_position");
        auto face_area_vectors = mesh.faces.get_or_add<Eigen::Vector<double, 3 >>("f_area_vector");

        for (const auto f: mesh.faces) {
            face_area_vectors[f] = MeshFaceAreaVector(mesh, f, positions);
        }

        face_area_vectors.set_dirty();
        return face_area_vectors;
    }
}