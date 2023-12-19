//
// Created by alex on 05.12.23.
//

#include "Components.h"
#include <filesystem>
#include <iostream>
#include "fmt/core.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

namespace Bcg {
    //------------------------------------------------------------------------------------------------------------------
    // FileWatcher
    //------------------------------------------------------------------------------------------------------------------

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

    Camera::Camera() : model(glm::mat4(1.0f)), is_orthographic(false) {
        set(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0f, 0.0f));
        set_perspective_parameters({0.1f, 100.0f, 800.0f / 600.0f, glm::radians(45.0f)});
    }

    glm::mat4 Camera::get_view() const {
        return glm::inverse(model);
    }

    glm::mat4 Camera::get_projection() const{
        if(is_orthographic){
            return glm::ortho(projection_parameters.orthographic_parameters.left, projection_parameters.orthographic_parameters.right,
                              projection_parameters.orthographic_parameters.bottom, projection_parameters.orthographic_parameters.top,
                              projection_parameters.orthographic_parameters.near, projection_parameters.orthographic_parameters.far);
        }else{
            return glm::perspective(projection_parameters.perspective_parameters.fovy, projection_parameters.perspective_parameters.aspect,
                                    projection_parameters.perspective_parameters.near, projection_parameters.perspective_parameters.far);
        }
    }

    glm::vec3 Camera::get_forward() const {
        //get direction vector from view_matrix
        return glm::vec3(-model[2]);
    }

    glm::vec3 Camera::get_direction() const {
        //get direction vector from view_matrix
        return glm::vec3(model[2]);
    }

    glm::vec3 Camera::get_up() const {
        //get up vector from view_matrix
        return glm::vec3(model[1]);
    }

    glm::vec3 Camera::get_right() const {
        //get right vector from view_matrix
        return glm::vec3(model[0]);
    }

    glm::vec3 Camera::get_position() const {
        //get position from view_matrix
        return glm::vec3(model[3]);
    }

    void Camera::set_model(const glm::mat4 &model_) {
        model = model_;
    }

    void Camera::set(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up){
        model = glm::inverse(glm::lookAt(position, target, up));
    }

    Camera::ProjectionParameters::Perspective Camera::get_perspective_parameters() const {
        return projection_parameters.perspective_parameters;
    }

    void Camera::set_perspective_parameters(const ProjectionParameters::Perspective &perspective_parameters) {
        projection_parameters.perspective_parameters = perspective_parameters;
        is_orthographic = false;
    }

    Camera::ProjectionParameters::Orthographic Camera::get_orthographic_parameters() const {
        return projection_parameters.orthographic_parameters;
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
}