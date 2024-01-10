//
// Created by alex on 05.12.23.
//

#include <filesystem>
#include <iostream>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "components/FileWatcher.h"
#include "components/Camera.h"
#include "components/Transform.h"
#include "components/AABB.h"
#include "components/EntityName.h"
#include "components/TriMesh.h"
#include "ImGuiUtils.h"
#include "Engine.h"

namespace Bcg {
    //------------------------------------------------------------------------------------------------------------------
    // FileWatcher
    //------------------------------------------------------------------------------------------------------------------

    inline double FovyRadians(double fovy_degrees) {
        return fovy_degrees * std::numbers::pi / double(360.0);
    }

    void FileWatcher::add(std::string filepath, std::function<void()> callback) {
        last_write_times[filepath] = std::filesystem::last_write_time(filepath);
        items[filepath].emplace_back(callback);
    }

    void FileWatcher::remove(std::string filepath) {
        items.erase(filepath);
    }

    void FileWatcher::clear() {
        items.clear();
    }

    void FileWatcher::check() {
        for (auto &[filepath, callbacks]: items) {
            auto &last_write_time = last_write_times[filepath];
            auto current_write_time = std::filesystem::last_write_time(filepath);
            //compare write times and call callbacks if necessary
            if ((current_write_time - last_write_time).count()) {
                last_write_time = current_write_time;
                for (auto &callback: callbacks) {
                    callback();
                }
            }
        }
    }

    glm::vec3 Transform::get_position() const {
        return glm::vec3(model[3]);
    }

    glm::vec3 Transform::get_scale() const {
        return glm::vec3(glm::length(glm::vec3(model[0])), glm::length(glm::vec3(model[1])),
                         glm::length(glm::vec3(model[2])));
    }

    glm::quat Transform::get_rotation() const {
        return glm::quat_cast(model);
    }

    glm::vec3 Transform::get_euler_angles() const {
        return glm::eulerAngles(get_rotation());
    }

    glm::vec3 Transform::get_angles_axis() const {
        return glm::axis(get_rotation());
    }

    glm::vec3 Transform::get_x_axis() const {
        return glm::vec3(model[0]);
    }

    glm::vec3 Transform::get_y_axis() const {
        return glm::vec3(model[1]);
    }

    glm::vec3 Transform::get_z_axis() const {
        return glm::vec3(model[2]);
    }

    void Transform::set_position(const glm::vec3 &position) {
        model[3] = glm::vec4(position, 1.0f);
    }

    void Transform::set_scale(const glm::vec3 &scale) {
        model[0] = glm::vec4(glm::normalize(glm::vec3(model[0])) * scale.x, 0.0f);
        model[1] = glm::vec4(glm::normalize(glm::vec3(model[1])) * scale.y, 0.0f);
        model[2] = glm::vec4(glm::normalize(glm::vec3(model[2])) * scale.z, 0.0f);
    }

    void Transform::set_rotation(const glm::quat &rotation) {
        model = glm::mat4_cast(rotation);
    }

    void Transform::set_rotation(const glm::vec3 &axis, float angle) {
        model = glm::rotate(glm::mat4(1.0f), angle, axis);
    }

    void Transform::set_rotation(const glm::vec3 &euler_angles) {
        model = glm::mat4_cast(glm::quat(euler_angles));
    }

    void Transform::set_rotation(float pitch, float yaw, float roll) {
        model = glm::mat4_cast(glm::quat(glm::vec3(pitch, yaw, roll)));
    }

    void ComponentGui<Transform>::Show(entt::entity entity_id) {
        if (!Engine::State().all_of<Transform>(entity_id)) {
            return;
        }
        auto &transform = Engine::State().get<Transform>(entity_id);
        ImGui::Text("position: (%f, %f, %f)", transform.get_position().x, transform.get_position().y,
                    transform.get_position().z);
        ImGui::Text("scale:    (%f, %f, %f)", transform.get_scale().x, transform.get_scale().y,
                    transform.get_scale().z);
        ImGui::Text("rotation: (%f, %f, %f)", transform.get_euler_angles().x, transform.get_euler_angles().y,
                    transform.get_euler_angles().z);

        ImGui::Separator();

        ImGui::Text("model matrix: %f %f %f %f\n"
                    "              %f %f %f %f\n"
                    "              %f %f %f %f\n"
                    "              %f %f %f %f\n", transform.model[0][0], transform.model[0][1], transform.model[0][2],
                    transform.model[0][3],
                    transform.model[1][0], transform.model[1][1], transform.model[1][2], transform.model[1][3],
                    transform.model[2][0], transform.model[2][1], transform.model[2][2], transform.model[2][3],
                    transform.model[3][0], transform.model[3][1], transform.model[3][2], transform.model[3][3]);
    }

    Camera::Camera() : is_orthographic(false) {
        set_worldup(glm::vec3(0.0f, 1.0f, 0.0f));
        set_position(glm::vec3(0.0f, 0.0f, 3.0f));
        set_target(glm::vec3(0.0f, 0.0f, 0.0f));
        set_perspective_parameters({0.1f, 100.0f, 800.0f / 600.0f, glm::radians(45.0f)});
    }

    glm::mat4 Camera::get_model() const {
        return glm::inverse(get_view());
    }

    glm::mat4 Camera::get_view() const {
        return glm::lookAt(view_parameters.position, view_parameters.position + view_parameters.front,
                           view_parameters.up);
    }

    glm::mat4 Camera::get_projection() const {
        if (is_orthographic) {
            return glm::ortho(projection_parameters.orthographic_parameters.left,
                              projection_parameters.orthographic_parameters.right,
                              projection_parameters.orthographic_parameters.bottom,
                              projection_parameters.orthographic_parameters.top,
                              projection_parameters.orthographic_parameters.near,
                              projection_parameters.orthographic_parameters.far);
        } else {
            float fovy_radians = FovyRadians(projection_parameters.perspective_parameters.fovy_degrees);
            return glm::perspective(fovy_radians,
                                    projection_parameters.perspective_parameters.aspect,
                                    projection_parameters.perspective_parameters.near,
                                    projection_parameters.perspective_parameters.far);
        }
    }

    void Camera::set_front(const glm::vec3 &front) {
        view_parameters.front = glm::normalize(front);
        view_parameters.right = glm::normalize(glm::cross(view_parameters.front, view_parameters.up));
        view_parameters.up = glm::normalize(glm::cross(view_parameters.right, view_parameters.front));
    }

    void Camera::set_target(const glm::vec3 &target) {
        set_front(target - view_parameters.position);
    }

    void Camera::set_position(const glm::vec3 &position) {
        view_parameters.position = position;
    }

    void Camera::set_worldup(const glm::vec3 &world_up) {
        view_parameters.world_up = world_up;
        view_parameters.right = glm::normalize(glm::cross(view_parameters.front, view_parameters.world_up));
        view_parameters.up = glm::normalize(glm::cross(view_parameters.right, view_parameters.front));
    }

    void Camera::set_view_parameters(const ViewParameters &parameters) {
        view_parameters = parameters;
        view_parameters.right = glm::normalize(glm::cross(view_parameters.front, view_parameters.world_up));
        view_parameters.up = glm::normalize(glm::cross(view_parameters.right, view_parameters.front));
    }

    void Camera::set_perspective_parameters(const ProjectionParameters::Perspective &perspective_parameters) {
        projection_parameters.perspective_parameters = perspective_parameters;
        is_orthographic = false;
    }

    void Camera::set_orthographic_parameters(const ProjectionParameters::Orthographic &orthographic_parameters) {
        projection_parameters.orthographic_parameters = orthographic_parameters;
        is_orthographic = true;
    }

    void AABB::grow(const glm::vec3 &point) {
        min = glm::min(min, point);
        max = glm::max(max, point);
    }

    void AABB::merge(const AABB &aabb) {
        min = glm::min(min, aabb.min);
        max = glm::max(max, aabb.max);
    }

    glm::vec3 AABB::get_center() const {
        return (min + max) / 2.0f;
    }

    glm::vec3 AABB::get_extent() const {
        return max - min;
    }

    void AABB::set(const glm::vec3 &center, const glm::vec3 &extent) {
        min = center - extent / 2.0f;
        max = center + extent / 2.0f;
    }

    void ComponentGui<AABB>::Show(entt::entity entity_id) {
        if (!Engine::State().all_of<AABB>(entity_id)) {
            return;
        }
        auto &aabb = Engine::State().get<AABB>(entity_id);
        ImGui::Text("min: (%f, %f, %f)", aabb.min.x, aabb.min.y, aabb.min.z);
        ImGui::Text("max: (%f, %f, %f)", aabb.max.x, aabb.max.y, aabb.max.z);
    }

    void ComponentGui<EntityName>::Show(entt::entity entity_id) {
        if (!Engine::State().all_of<EntityName>(entity_id)) {
            return;
        }
        auto &name = Engine::State().get<EntityName>(entity_id);
        ImGui::Text("name: %s", name.c_str());
    }

    void ComponentGui<TriMesh>::Show(entt::entity entity_id) {
        if (!Engine::State().all_of<TriMesh>(entity_id)) {
            return;
        }
        auto &mesh = Engine::State().get<TriMesh>(entity_id);
        Show("##TriMesh", mesh);
    }

    void ComponentGui<TriMesh>::Show(const char *label, TriMesh &mesh) {
        if(ImGui::CollapsingHeader("Vertices")) {
            ComponentGui<Vertices>::Show("##TriMeshVertices", mesh.vertices);
        }
        if(ImGui::CollapsingHeader("Faces")) {
            ComponentGui<Faces>::Show("##TriMeshFaces",mesh.faces);
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
        if(!faces.vertices.empty() && ImGui::CollapsingHeader("Vertices")) {
            ComponentGui<std::vector<glm::uvec3>>::Show("##FacesVertices", faces.vertices);
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