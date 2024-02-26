//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_TRANSFORM_H
#define ENGINE23_TRANSFORM_H

#include "glm/glm.hpp"
#include "components/ComponentGui.h"

namespace Bcg{
    struct Transform {
        glm::mat4 model = glm::mat4(1.0f);

        glm::vec3 get_position() const;

        glm::vec3 get_scale() const;

        glm::quat get_rotation() const;

        glm::vec3 get_euler_angles() const;

        glm::vec3 get_angles_axis() const;

        glm::vec3 get_x_axis() const;

        glm::vec3 get_y_axis() const;

        glm::vec3 get_z_axis() const;

        void set_position(const glm::vec3 &position);

        void set_scale(const glm::vec3 &scale);

        void set_rotation(const glm::quat &rotation);

        void set_rotation(const glm::vec3 &axis, float angle);

        void set_rotation(const glm::vec3 &euler_angles);

        void set_rotation(float pitch, float yaw, float roll);
    };

}

#endif //ENGINE23_TRANSFORM_H
