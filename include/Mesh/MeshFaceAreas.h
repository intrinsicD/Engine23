//
// Created by alex on 23.02.24.
//

#ifndef ENGINE23_MESHFACEAREAS_H
#define ENGINE23_MESHFACEAREAS_H

#include "Mesh.h"

namespace Bcg {
    double MeshFaceArea(const Mesh &mesh, const FaceHandle &f, const Property<Eigen::Vector<double, 3>> &positions);

    Property<double> MeshFaceAreas(Mesh &mesh);
}

#endif //ENGINE23_MESHFACEAREAS_H
