//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_TRIMESH_H
#define ENGINE23_TRIMESH_H

#include <vector>
#include "glm/glm.hpp"

namespace Bcg{
    struct Faces {
        std::vector<glm::uvec3> vertices;
        std::vector<glm::uvec3> normals;
        std::vector<glm::uvec3> texcoords;
    };

    struct Vertices {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec3> colors;
    };

    struct TriMesh {
        Vertices vertices;
        Faces faces;
    };
}

#endif //ENGINE23_TRIMESH_H
