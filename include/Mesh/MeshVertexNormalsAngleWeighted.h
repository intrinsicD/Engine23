//
// Created by alex on 23.02.24.
//

#ifndef ENGINE23_MESHVERTEXNORMALSANGLEWEIGHTED_H
#define ENGINE23_MESHVERTEXNORMALSANGLEWEIGHTED_H

#include "Mesh.h"

namespace Bcg {
    Eigen::Vector<double, 3> MeshVertexNormalAngleWeighted(const Mesh &mesh, const VertexHandle &v,
                                                          const Property<Eigen::Vector<double, 3>> &face_normals,
                                                          const Property<Eigen::Vector<double, 3>> &positions);

    Property<Eigen::Vector<double, 3>> MeshVertexNormalsAngleWeighted(Mesh &mesh);
}

#endif //ENGINE23_MESHVERTEXNORMALSANGLEWEIGHTED_H
