//
// Created by alex on 20.06.24.
//

#ifndef ENGINE23_FACENORMALS_H
#define ENGINE23_FACENORMALS_H

#include "glm/glm.hpp"
#include <vector>
#include "Mesh.h"

namespace Bcg {
    struct FaceNormals {
        void set_position_data(const void *data, unsigned int size, unsigned int element_bytes);

        void set_position_buffer(unsigned int id, unsigned int size, unsigned int element_bytes);

        void set_triangle_data(const void *data, unsigned int size, unsigned int element_bytes);

        void set_triangle_buffer(unsigned int id, unsigned int size, unsigned int element_bytes);

        void set_normal_buffer(unsigned int id, unsigned int size, unsigned int element_bytes);

        void compile_program();

        void compute();

        void destroy_all();

        void get_result(unsigned int buffer, unsigned int size_bytes, void *data);

        std::vector<glm::vec3> get_result();

        struct Buffer {
            unsigned int id = -1;
            unsigned int size = -1;
            unsigned int element_bytes = -1;
        } b_positions, b_triangles, b_normals;
        unsigned int program = -1;
    };

    class ComputeFaceNormals{
    public:
        explicit ComputeFaceNormals(Mesh &mesh) {
            positions = mesh.vertices.get<Eigen::Vector<double, 3>>("v_position");
            triangles = mesh.get_triangles();
            face_normals = mesh.faces.get_or_add<Eigen::Vector<double, 3>>("f_normal_compute");
        }

        void compute();

        Property<Eigen::Vector<double, 3>> positions;
        Property<Eigen::Vector<unsigned int, 3>> triangles;
        Property<Eigen::Vector<double, 3>> face_normals;
    };
}

#endif //ENGINE23_FACENORMALS_H
