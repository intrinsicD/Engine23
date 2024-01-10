//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_TRIMESH_H
#define ENGINE23_TRIMESH_H

#include <vector>
#include "ComponentGui.h"
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


    template<>
    struct ComponentGui<TriMesh> {
        static void Show(entt::entity entity_id);

        static void Show(const char *label, TriMesh &mesh);
    };

    template<>
    struct ComponentGui<Vertices> {
        static void Show(const char *label, Vertices &vertices);
    };

    template<>
    struct ComponentGui<Faces> {
        static void Show(const char *label, Faces &faces);
    };

    template<>
    struct ComponentGui<std::vector<glm::vec3>> {
        static void Show(const char *label, std::vector<glm::vec3> &vec);
    };

    template<>
    struct ComponentGui<std::vector<glm::uvec3>> {
        static void Show(const char *label, std::vector<glm::uvec3> &vec);
    };
}

#endif //ENGINE23_TRIMESH_H
