//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_CAMERA_H
#define ENGINE23_CAMERA_H

#include "LookAtRH.h"
#include "OrthoRH_NO.h"
#include "PerspectiveRH_NO.h"
#include "DegreesToRadians.h"
#include "PerspectiveProjection.h"
#include "OrthographicProjection.h"
#include "ViewParameters.h"

namespace Bcg {
    template<typename T>
    class Camera {
    public:
        struct Projection {
            PerspectiveProjection<T> perspective_parameters;
            OrthographicProjection<T> orthographic_parameters;
        } projection;

        ViewParameters<T> view;

        struct Sensitivity {
            T zoom = 5.0;
            T move = 5.0;
            T rotate = 2.0;
            T drag = 0.01;
        } sensitivity;

        struct ArcBallParameters {
            bool last_point_ok = false;
            Eigen::Vector<T, 2> last_point_2d = Eigen::Vector<T, 2>(0.0, 0.0);
            Eigen::Vector<T, 3> last_point_3d = Eigen::Vector<T, 3>(0.0, 0.0, 0.0);
        } arc_ball_parameters;

        bool is_orthographic = true;

        Camera() : is_orthographic(false) {
            view.set_worldup({0.0, 1.0, 0.0});
            view.set_position({0.0, 0.0, 3.0});
            view.set_target({0.0, 0.0, 0.0});
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

        Eigen::Matrix<T, 4, 4> get_view_projection() const {
            return get_projection() * get_view();
        }

        void set_view_parameters(const ViewParameters<T> &parameters) {
            view = parameters;
            Eigen::Vector<T, 3> front = view.get_front();
            view.right = front.cross(view.world_up).normalized();
            view.up = view.right.cross(front);
        }

        void set_perspective_parameters(const PerspectiveProjection<T> &parameters) {
            projection.perspective_parameters = parameters;
            is_orthographic = false;
        }

        void set_orthographic_parameters(const OrthographicProjection<T> &parameters) {
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
