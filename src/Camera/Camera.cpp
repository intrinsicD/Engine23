//
// Created by alex on 26.02.24.
//

#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"
#include "LookAtRH.h"
#include "OrthoRH_NO.h"
#include "PerspectiveRH_NO.h"
#include <numbers>

namespace Bcg {

    inline double FovyRadians(double fovy_degrees) {
        return fovy_degrees * std::numbers::pi / double(360.0);
    }

    Camera::Camera() : is_orthographic(false) {
        set_worldup(Eigen::Vector<float, 3>(0.0f, 1.0f, 0.0f));
        set_position(Eigen::Vector<float, 3>(0.0f, 0.0f, 3.0f));
        set_target(Eigen::Vector<float, 3>(0.0f, 0.0f, 0.0f));
        set_perspective_parameters({0.1f, 100.0f, 800.0f / 600.0f, glm::radians(45.0f)});
    }

    Eigen::Matrix<float, 4, 4> Camera::get_model() const {
        return get_view().inverse();
    }

    Eigen::Matrix<float, 4, 4> Camera::get_view() const {
        return LookAtRH<float>(view_parameters.position, view_parameters.position + view_parameters.front,
                        view_parameters.up);
    }

    Eigen::Matrix<float, 4, 4> Camera::get_projection() const {
        if (is_orthographic) {
            return OrthoRH_NO(projection_parameters.orthographic_parameters.left,
                              projection_parameters.orthographic_parameters.right,
                              projection_parameters.orthographic_parameters.bottom,
                              projection_parameters.orthographic_parameters.top,
                              projection_parameters.orthographic_parameters.near,
                              projection_parameters.orthographic_parameters.far);
        } else {
            float fovy_radians = FovyRadians(projection_parameters.perspective_parameters.fovy_degrees);
            return PerspectiveRH_NO(fovy_radians,
                                    projection_parameters.perspective_parameters.aspect,
                                    projection_parameters.perspective_parameters.near,
                                    projection_parameters.perspective_parameters.far);
        }
    }

    void Camera::set_front(const Eigen::Vector<float, 3> &front) {
        view_parameters.front = front.normalized();
        view_parameters.right = view_parameters.front.cross(view_parameters.up).normalized();
        view_parameters.up = view_parameters.right.cross(view_parameters.front);
    }

    void Camera::set_target(const Eigen::Vector<float, 3> &target) {
        set_front(target - view_parameters.position);
    }

    void Camera::set_position(const Eigen::Vector<float, 3> &position) {
        view_parameters.position = position;
    }

    void Camera::set_worldup(const Eigen::Vector<float, 3> &world_up) {
        view_parameters.world_up = world_up;
        view_parameters.right = view_parameters.front.cross(view_parameters.world_up).normalized();
        view_parameters.up = view_parameters.right.cross(view_parameters.front);
    }

    void Camera::set_view_parameters(const ViewParameters &parameters) {
        view_parameters = parameters;
        view_parameters.right = view_parameters.front.cross(view_parameters.world_up).normalized();
        view_parameters.up = view_parameters.right.cross(view_parameters.front);
    }

    void Camera::set_perspective_parameters(const ProjectionParameters::Perspective &perspective_parameters) {
        projection_parameters.perspective_parameters = perspective_parameters;
        is_orthographic = false;
    }

    void Camera::set_orthographic_parameters(const ProjectionParameters::Orthographic &orthographic_parameters) {
        projection_parameters.orthographic_parameters = orthographic_parameters;
        is_orthographic = true;
    }
}