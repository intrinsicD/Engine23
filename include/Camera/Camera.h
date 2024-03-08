//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_CAMERA_H
#define ENGINE23_CAMERA_H

#include "Eigen/Core"

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
            Eigen::Vector<float, 3> position = Eigen::Vector<float, 3>(0.0f, 0.0f, 3.0f);
            Eigen::Vector<float, 3> front = Eigen::Vector<float, 3>(0.0f, 0.0f, -1.0f);
            Eigen::Vector<float, 3> up = Eigen::Vector<float, 3>(0.0f, 1.0f, 0.0f);
            Eigen::Vector<float, 3> right = Eigen::Vector<float, 3>(1.0f, 0.0f, 0.0f);
            Eigen::Vector<float, 3> world_up = Eigen::Vector<float, 3>(0.0f, 1.0f, 0.0f);
            bool dirty;
        }view_parameters;

        struct Sensitivity{
            float zoom = 5.0f;
            float move = 5.0f;
            float rotate = 5.0f;
            float drag = 0.01f;
        }sensitivity;

        struct ArcBallParameters {
            bool last_point_ok = false;
            Eigen::Vector<float, 3> target = Eigen::Vector<float, 3>(0.0f, 0.0f, 0.0f);
            Eigen::Vector<float, 2> last_point_2d = Eigen::Vector<float, 2>(0.0f, 0.0f);
            Eigen::Vector<float, 3> last_point_3d = Eigen::Vector<float, 3>(0.0f, 0.0f, 0.0f);
        }arc_ball_parameters;

        bool is_orthographic = true;

        Camera();

        Eigen::Matrix<float, 4, 4> get_model() const;

        Eigen::Matrix<float, 4, 4> get_view() const;

        Eigen::Matrix<float, 4, 4> get_projection() const;

        void set_front(const Eigen::Vector<float, 3> &front);

        void set_target(const Eigen::Vector<float, 3> &target);

        void set_position(const Eigen::Vector<float, 3> &position);

        void set_worldup(const Eigen::Vector<float, 3> &world_up);

        void set_view_parameters(const ViewParameters &parameters);

        void set_perspective_parameters(const ProjectionParameters::Perspective &parameters);

        void set_orthographic_parameters(const ProjectionParameters::Orthographic &parameters);
    };
}

#endif //ENGINE23_CAMERA_H
