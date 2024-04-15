//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_CAMERA_H
#define ENGINE23_CAMERA_H

#include "LookAtRH.h"
#include "OrthoRH_NO.h"
#include "PerspectiveRH_NO.h"

namespace Bcg {

    template<typename T>
    T FovyRadians(T fovy_degrees) {
        return fovy_degrees * std::numbers::pi / T(360.0);
    }

    template<typename T>
    struct Camera {
        struct ProjectionParameters {
            struct Perspective {
                T fovy_degrees = 45.0;
                T aspect = 4.0 / 3.0;
                T near;
                T far;
                bool dirty;
            } perspective_parameters;
            struct Orthographic {
                T left;
                T right;
                T bottom;
                T top;
                T near;
                T far;
                bool dirty;
            } orthographic_parameters;
        } projection_parameters;
        struct ViewParameters {
            Eigen::Vector<T, 3> position = Eigen::Vector<T, 3>(0.0, 0.0, 3.0);
            Eigen::Vector<T, 3> front = Eigen::Vector<T, 3>(0.0, 0.0, -1.0);
            Eigen::Vector<T, 3> up = Eigen::Vector<T, 3>(0.0, 1.0, 0.0);
            Eigen::Vector<T, 3> right = Eigen::Vector<T, 3>(1.0, 0.0, 0.0);
            Eigen::Vector<T, 3> world_up = Eigen::Vector<T, 3>(0.0, 1.0, 0.0);
            bool dirty;
        } view_parameters;

        struct Sensitivity {
            T zoom = 5.0;
            T move = 5.0;
            T rotate = 2.0;
            T drag = 0.01;
        } sensitivity;

        struct ArcBallParameters {
            bool last_point_ok = false;
            Eigen::Vector<T, 3> target = Eigen::Vector<T, 3>(0.0, 0.0, 0.0);
            Eigen::Vector<T, 2> last_point_2d = Eigen::Vector<T, 2>(0.0, 0.0);
            Eigen::Vector<T, 3> last_point_3d = Eigen::Vector<T, 3>(0.0, 0.0, 0.0);
        } arc_ball_parameters;

        bool is_orthographic = true;

        Camera() : is_orthographic(false) {
            set_worldup({0.0, 1.0, 0.0});
            set_position({0.0, 0.0, 3.0});
            set_target({0.0, 0.0, 0.0});
            set_perspective_parameters({0.1, 100.0, 800.0 / 600.0, FovyRadians<T>(45.0)});
        }

        Eigen::Matrix<T, 4, 4> get_model() const {
            return get_view().inverse();
        }

        Eigen::Matrix<T, 4, 4> get_view() const {
            return LookAtRH<T>(view_parameters.position,
                               view_parameters.position + view_parameters.front,
                            view_parameters.up);
        }

        Eigen::Matrix<T, 4, 4> get_projection() const {
            if (is_orthographic) {
                return OrthoRH_NO(projection_parameters.orthographic_parameters.left,
                                  projection_parameters.orthographic_parameters.right,
                                  projection_parameters.orthographic_parameters.bottom,
                                  projection_parameters.orthographic_parameters.top,
                                  projection_parameters.orthographic_parameters.near,
                                  projection_parameters.orthographic_parameters.far);
            } else {
                return PerspectiveRH_NO(FovyRadians(projection_parameters.perspective_parameters.fovy_degrees),
                                        projection_parameters.perspective_parameters.aspect,
                                        projection_parameters.perspective_parameters.near,
                                        projection_parameters.perspective_parameters.far);
            }
        }

        void set_front(const Eigen::Vector<T, 3> &front) {
            view_parameters.front = front.normalized();
            view_parameters.right = view_parameters.front.cross(view_parameters.up).normalized();
            view_parameters.up = view_parameters.right.cross(view_parameters.front);
        }

        void set_target(const Eigen::Vector<T, 3> &target) {
            set_front(target - view_parameters.position);
        }

        void set_position(const Eigen::Vector<T, 3> &position) {
            view_parameters.position = position;
        }

        void set_worldup(const Eigen::Vector<T, 3> &world_up) {
            view_parameters.world_up = world_up;
            view_parameters.right = view_parameters.front.cross(view_parameters.world_up).normalized();
            view_parameters.up = view_parameters.right.cross(view_parameters.front);
        }

        void set_view_parameters(const ViewParameters &parameters) {
            view_parameters = parameters;
            view_parameters.right = view_parameters.front.cross(view_parameters.world_up).normalized();
            view_parameters.up = view_parameters.right.cross(view_parameters.front);
        }

        void set_perspective_parameters(const typename ProjectionParameters::Perspective &parameters) {
            projection_parameters.perspective_parameters = parameters;
            is_orthographic = false;
        }

        void set_orthographic_parameters(const typename ProjectionParameters::Orthographic &parameters) {
            projection_parameters.orthographic_parameters = parameters;
            is_orthographic = true;
        }
    };
}

#endif //ENGINE23_CAMERA_H
