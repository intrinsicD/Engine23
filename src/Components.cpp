//
// Created by alex on 05.12.23.
//

#include "TriMesh.h"
#include "ImGuiUtils.h"
#include "Engine.h"

namespace Bcg {

    void ComponentGui<TriMesh>::Show(entt::entity entity_id) {
        if (!Engine::State().all_of<TriMesh>(entity_id)) {
            return;
        }
        auto &mesh = Engine::State().get<TriMesh>(entity_id);
        Show("##TriMesh", mesh);
    }




    void ComponentGui<TriMesh>::Show(const char *label, TriMesh &mesh) {
        if(ImGui::CollapsingHeader("Vertices")) {
            ImGui::Indent();
            ComponentGui<Vertices>::Show("##TriMeshVertices", mesh.vertices);
            ImGui::Unindent();
        }
        if(ImGui::CollapsingHeader("Faces")) {
            ImGui::Indent();
            ComponentGui<Faces>::Show("##TriMeshFaces",mesh.faces);
            ImGui::Unindent();
        }
    }

    void ComponentGui<Vertices>::Show(const char *label, Vertices &vertices) {
        if(!vertices.positions.empty() && ImGui::CollapsingHeader("Positions")) {
            ComponentGui<std::vector<glm::vec3>>::Show("##VerticesPositions",vertices.positions);
        }
        if(!vertices.normals.empty() && ImGui::CollapsingHeader("Normals")) {
            ComponentGui<std::vector<glm::vec3>>::Show("##VerticesNormals", vertices.normals);
        }
        if(!vertices.colors.empty() && ImGui::CollapsingHeader("Colors")) {
            ComponentGui<std::vector<glm::vec3>>::Show("##VerticesColors",vertices.colors);
        }
    }

    void ComponentGui<Faces>::Show(const char *label, Faces &faces) {
        if(!faces.indices.empty() && ImGui::CollapsingHeader("Indices")) {
            ComponentGui<std::vector<glm::uvec3>>::Show("##FacesIndices", faces.indices);
        }
        if(!faces.normals.empty() && ImGui::CollapsingHeader("Normals")) {
            ComponentGui<std::vector<glm::uvec3>>::Show("##FacesNormals",faces.normals);
        }
        if(!faces.texcoords.empty() && ImGui::CollapsingHeader("Texcoords")) {
            ComponentGui<std::vector<glm::uvec3>>::Show("##FacesTexCoords", faces.texcoords);
        }
    }

    void ComponentGui<std::vector<glm::vec3>>::Show(const char *label,std::vector<glm::vec3> &vec) {
        //A comboBox with selectable items would be nice here
        static int current_item = 0;
        ImGuiUtils::Combo(label, &current_item, vec);
    }

    void ComponentGui<std::vector<glm::uvec3>>::Show(const char *label,std::vector<glm::uvec3> &vec) {
        //A comboBox with selectable items would be nice here
        static int current_item = 0;
        ImGuiUtils::Combo(label, &current_item, vec);
    }
}