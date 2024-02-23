//
// Created by alex on 23.02.24.
//

#ifndef ENGINE23_MESHFACEAREAVECTORS_H
#define ENGINE23_MESHFACEAREAVECTORS_H

#include "Mesh.h"

namespace Bcg{
    Eigen::Vector<double, 3> MeshFaceAreaVector(const Mesh &mesh, const FaceHandle &f, const Property<Eigen::Vector<double, 3>> &positions);

    Property<Eigen::Vector<double, 3>> MeshFaceAreaVectors(Mesh &mesh);
}

#endif //ENGINE23_MESHFACEAREAVECTORS_H
