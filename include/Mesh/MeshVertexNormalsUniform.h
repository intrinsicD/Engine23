//
// Created by alex on 23.02.24.
//

#ifndef ENGINE23_MESHVERTEXNORMALSUNIFORM_H
#define ENGINE23_MESHVERTEXNORMALSUNIFORM_H

#include "Mesh.h"

namespace Bcg{
    Eigen::Vector<double, 3> MeshVertexNormalUniform(const Mesh &mesh, const VertexHandle &v, Property<Eigen::Vector<double, 3>> &face_normals);

    Property<Eigen::Vector<double, 3>> MeshVertexNormalsUniform(Mesh &mesh);
}

#endif //ENGINE23_MESHVERTEXNORMALSUNIFORM_H
