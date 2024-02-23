//
// Created by alex on 23.02.24.
//

#ifndef ENGINE23_MESHFACENORMALS_H
#define ENGINE23_MESHFACENORMALS_H

#include "Mesh.h"

namespace Bcg{
    Eigen::Vector<double, 3> MeshFaceNormal(const Mesh &mesh, const FaceHandle &f, const Property<Eigen::Vector<double, 3>> &positions);

    Property<Eigen::Vector<double, 3>> MeshFaceNormals(Mesh &mesh);
}

#endif //ENGINE23_MESHFACENORMALS_H
