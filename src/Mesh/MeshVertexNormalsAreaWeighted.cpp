//
// Created by alex on 23.02.24.
//
#include "MeshVertexNormalsAreaWeighted.h"
#include "MeshFaceNormals.h"
#include "MeshFaceAreas.h"

namespace Bcg {
    Eigen::Vector<double, 3>
    MeshVertexNormalAreaWeighted(const Mesh &mesh, const VertexHandle &v, const Property<Eigen::Vector<double, 3>> &face_normals, const Property<double> &face_areas) {
        Eigen::Vector<double, 3> normal = Eigen::Vector<double, 3>::Zero();

        for (auto f: mesh.get_faces(v)) {
            normal += face_normals[f] * face_areas[f];
        }

        return normal.normalized();
    }

    Property<Eigen::Vector<double, 3>> MeshVertexNormalsAreaWeighted(Mesh &mesh) {
        auto normals = mesh.vertices.get_or_add<Eigen::Vector<double, 3>>("v_normal");
        auto face_normals = mesh.faces.get<Eigen::Vector<double, 3>>("f_normal");
        auto face_areas = mesh.faces.get<double>("f_area");
        if (!face_normals) {
            face_normals = MeshFaceNormals(mesh);
        }
        if(!face_areas){
            face_areas = MeshFaceAreas(mesh);
        }
        for (auto v: mesh.vertices) {
            normals[v] = MeshVertexNormalAreaWeighted(mesh, v, face_normals, face_areas);
        }

        normals.set_dirty();
        return normals;
    }
}