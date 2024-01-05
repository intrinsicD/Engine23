//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_CAMERA_H
#define ENGINE23_CAMERA_H

#include "glm/glm.hpp"

namespace Bcg{
    struct Camera {
        struct ProjectionParameters {
            struct Perspective {
                float fovy_degrees = 45.0f;
                float aspect = 4.0f / 3.0f;
                float near;
                float far;
                bool dirty;
            } perspective_parameters;
            struct Orthographic {
                float left;
                float right;
                float bottom;
                float top;
                float near;
                float far;
                bool dirty;
            } orthographic_parameters;
        } projection_parameters;
        struct ViewParameters{
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
            glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
            glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
            bool dirty;
        }view_parameters;

        struct Sensitivity{
            float zoom = 5.0f;
            float move = 5.0f;
            float rotate = 5.0f;
            float drag = 0.01f;
        }sensitivity;

        bool is_orthographic = true;

        Camera();

        glm::mat4 get_model() const;

        glm::mat4 get_view() const;

        glm::mat4 get_projection() const;

        void set_front(const glm::vec3 &front);

        void set_target(const glm::vec3 &target);

        void set_position(const glm::vec3 &position);

        void set_worldup(const glm::vec3 &world_up);

        void set_view_parameters(const ViewParameters &parameters);

        void set_perspective_parameters(const ProjectionParameters::Perspective &parameters);

        void set_orthographic_parameters(const ProjectionParameters::Orthographic &parameters);
    };
}

#endif //ENGINE23_CAMERA_H
