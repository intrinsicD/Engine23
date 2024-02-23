//
// Created by alex on 23.02.24.
//

#ifndef ENGINE23_MESHVERTEXNORMALSAREAWEIGHTED_H
#define ENGINE23_MESHVERTEXNORMALSAREAWEIGHTED_H

#include "Mesh.h"

namespace Bcg {
    Eigen::Vector<double, 3> MeshVertexNormalAreaWeighted(const Mesh &mesh, const VertexHandle &v,
                                                          const Property<Eigen::Vector<double, 3>> &face_normals,
                                                          const Property<double> &face_areas);

    Property<Eigen::Vector<double, 3>> MeshVertexNormalsAreaWeighted(Mesh &mesh);
}

#endif //ENGINE23_MESHVERTEXNORMALSAREAWEIGHTED_H
