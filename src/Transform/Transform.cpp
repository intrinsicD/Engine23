//
// Created by alex on 26.02.24.
//

#include "Transform.h"
#include "glm/gtx/euler_angles.hpp"

namespace Bcg{

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
}