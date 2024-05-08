//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_CAMERA_H
#define ENGINE23_CAMERA_H

#include "LookAtRH.h"
#include "OrthoRH_NO.h"
#include "PerspectiveRH_NO.h"
#include "DegreesToRadians.h"

namespace Bcg {
    template<typename T>
    class Camera {
    public:
        struct Projection {
            struct Perspective {
                T fovy_degrees = 45.0;
                T aspect = 4.0 / 3.0;
                T near;
                T far;
                bool dirty = false;

                Eigen::Matrix<T, 4, 4> get_matrix() const {
                    return PerspectiveRH_NO(DegreesToRadians(fovy_degrees), aspect, near, far);
                }
            } perspective_parameters;

            struct Orthographic {
                T left;
                T right;
                T bottom;
                T top;
                T near;
                T far;
                bool dirty = false;

                Eigen::Matrix<T, 4, 4> get_matrix() const {
                    return OrthoRH_NO(left, right, bottom, top, near, far);
                }
            } orthographic_parameters;
        } projection;

        struct ViewParameters {
            Eigen::Vector<T, 3> position = Eigen::Vector<T, 3>(0.0, 0.0, 3.0);
            Eigen::Vector<T, 3> front = Eigen::Vector<T, 3>(0.0, 0.0, -1.0);
            Eigen::Vector<T, 3> up = Eigen::Vector<T, 3>(0.0, 1.0, 0.0);
            Eigen::Vector<T, 3> right = Eigen::Vector<T, 3>(1.0, 0.0, 0.0);
            Eigen::Vector<T, 3> world_up = Eigen::Vector<T, 3>(0.0, 1.0, 0.0);
            bool dirty;

            Eigen::Matrix<T, 4, 4> get_matrix() const {
                return LookAtRH<T>(position, position + front, up);
            }
        } view;

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
            set_perspective_parameters({0.1, 100.0, 800.0 / 600.0, DegreesToRadians<T>(45.0)});
        }

        Eigen::Matrix<T, 4, 4> get_model() const {
            return get_view().inverse();
        }

        Eigen::Matrix<T, 4, 4> get_view() const {
            return view.get_matrix();
        }

        Eigen::Matrix<T, 4, 4> get_projection() const {
            if (is_orthographic) {
                return projection.orthographic_parameters.get_matrix();
            } else {
                return projection.perspective_parameters.get_matrix();
            }
        }

        void set_front(const Eigen::Vector<T, 3> &front) {
            view.front = front.normalized();
            view.right = view.front.cross(view.up).normalized();
            view.up = view.right.cross(view.front);
        }

        void set_target(const Eigen::Vector<T, 3> &target) {
            set_front(target - view.position);
        }

        void set_position(const Eigen::Vector<T, 3> &position) {
            view.position = position;
        }

        void set_worldup(const Eigen::Vector<T, 3> &world_up) {
            view.world_up = world_up;
            view.right = view.front.cross(view.world_up).normalized();
            view.up = view.right.cross(view.front);
        }

        void set_view_parameters(const ViewParameters &parameters) {
            view = parameters;
            view.right = view.front.cross(view.world_up).normalized();
            view.up = view.right.cross(view.front);
        }

        void set_perspective_parameters(const typename Projection::Perspective &parameters) {
            projection.perspective_parameters = parameters;
            is_orthographic = false;
        }

        void set_orthographic_parameters(const typename Projection::Orthographic &parameters) {
            projection.orthographic_parameters = parameters;
            is_orthographic = true;
        }

        friend std::ostream &operator<<(std::ostream &stream, const Camera<T> &camera) {
            stream << "Camera: TODO implement this!" << std::endl;
            //TODO implement Camera output stream as string
            return stream;
        }
    };
}

#endif //ENGINE23_CAMERA_H
