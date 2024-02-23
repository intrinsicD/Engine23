//
// Created by alex on 23.02.24.
//

#ifndef ENGINE23_MESHVERTEXNORMALSAREAANGLEHYBRID_H
#define ENGINE23_MESHVERTEXNORMALSAREAANGLEHYBRID_H

#include "Mesh.h"

namespace Bcg {
    Eigen::Vector<double, 3> MeshVertexNormalAreaAngleHybrid(const Mesh &mesh, const VertexHandle &v,
                                                             const Property<Eigen::Vector<double, 3>> &face_normals,
                                                             const Property<double> &face_areas,
                                                             const Property<Eigen::Vector<double, 3>> &positions);

    Property<Eigen::Vector<double, 3>> MeshVertexNormalsAreaAngleHybrid(Mesh &mesh);
}

#endif //ENGINE23_MESHVERTEXNORMALSAREAANGLEHYBRID_H
