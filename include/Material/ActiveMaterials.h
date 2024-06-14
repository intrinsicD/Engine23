//
// Created by alex on 14.06.24.
//

#ifndef ENGINE23_ACTIVEMATERIALS_H
#define ENGINE23_ACTIVEMATERIALS_H

#include <vector>

namespace Bcg {
    struct ActiveMaterials {
        unsigned int mesh_material_idx = -1;
        unsigned int graph_material_idx = -1;
        unsigned int point_cloud_material_idx = -1;
        std::vector<unsigned int> vertex_vector_field_indices;
        std::vector<unsigned int> edge_vector_field_indices;
        std::vector<unsigned int> face_vector_field_indices;

        friend std::ostream &operator<<(std::ostream &stream, const ActiveMaterials &active_materials) {
            stream << "mesh_material_idx: " << active_materials.mesh_material_idx << ", "
                   << "graph_material_idx: " << active_materials.graph_material_idx << ", "
                   << "point_cloud_material_idx: " << active_materials.point_cloud_material_idx
                   << ", "
                   << "vertex_vector_field_indices: [";
            for (const auto &index: active_materials.vertex_vector_field_indices) {
                stream << index << ", ";
            }
            stream << "], edge_vector_field_indices: [";
            for (const auto &index: active_materials.edge_vector_field_indices) {
                stream << index << ", ";
            }
            stream << "], face_vector_field_indices: [";
            for (const auto &index: active_materials.face_vector_field_indices) {
                stream << index << ", ";
            }
            stream << "]";
            return stream;
        }
    };
}

#endif //ENGINE23_ACTIVEMATERIALS_H
