//
// Created by alex on 29.05.24.
//

#ifndef ENGINE23_VIEWMATRIX_H
#define ENGINE23_VIEWMATRIX_H

#include "Eigen/Core"

//TODO use this in the camera and also the Projection stuff

namespace Bcg {
    template<typename T>
    struct ViewParameters {
        Eigen::Vector<T, 3> position;
        Eigen::Vector<T, 3> right;
        Eigen::Vector<T, 3> up;

        // Returns the vector from the scene into the camera
        Eigen::Vector<T, 3> backward() const {
            return right.cross(up);
        }

        // Returns the vector from the camera into the scene
        Eigen::Vector<T, 3> forward() const {
            return -backward();
        }

        void set_forward(const Eigen::Vector<T, 3> &vector) {
            Eigen::Vector<T, 3> forward = vector.normalized(); // Normalize the forward vector
            right = forward.cross(up).normalized(); // Calculate and normalize the right vector
            up = right.cross(forward).normalized(); // Recalculate and normalize the up vector
        }

        void lookAt(const Eigen::Vector<T, 3> &target) {
            set_forward((target - position).normalized());
        }

        void rotate(const Eigen::Matrix<T, 3, 3> &R) {
            // Rotate the right and up vectors to maintain the orientation of the camera
            right = R * right;
            up = R * up;
        }

        void rotate_around_center(const Eigen::Matrix<T, 3, 3> &R,
                                  const Eigen::Vector<T, 3> &center = Eigen::Vector<T, 3>::Zeros()) {
            rotate(R);
            // Rotate the position of the camera around the center point
            position = R * (position - center) + center;
        }
    };

    template<typename T>
    Eigen::Matrix<T, 4, 4>
    ViewMatrix(const Eigen::Vector<T, 3> &position, const Eigen::Vector<T, 3> &right, const Eigen::Vector<T, 3> &up) {
        Eigen::Matrix<T, 4, 4> matrix = Eigen::Matrix<T, 4, 4>::Identity();
        Eigen::Vector<T, 3> forward = -right.cross(up);  // Forward direction vector

        // Set the rotation part of the matrix
        matrix.block<3, 1>(0, 0) = right;
        matrix.block<3, 1>(0, 1) = up;
        matrix.block<3, 1>(0, 2) = forward;

        // Set the translation part of the matrix
        matrix(0, 3) = -right.dot(position);
        matrix(1, 3) = -up.dot(position);
        matrix(2, 3) = -forward.dot(position);
        return matrix;
    }

    template<typename T>
    Eigen::Matrix<T, 4, 4> ViewMatrix(const ViewParameters<T> &parameters) {
        return ViewMatrix(parameters.position, parameters.right, parameters.up);
    }

    template<typename T>
    Eigen::Matrix<T, 4, 4> ViewMatrix(const Eigen::Matrix<T, 4, 4> &model) {
        return model.inverse();
    }

    template<typename T>
    ViewParameters<T> ViewParametersFromMatrix(const Eigen::Matrix<T, 4, 4> &view) {
        ViewParameters<T> params;
        params.right = view.template block<3, 1>(0, 0);      // Extract right vector
        params.up = view.template block<3, 1>(0, 1);         // Extract up vector
        params.position = -(params.right * view(0, 3)
                            + params.up * view(1, 3)
                            + view.template block<3, 1>(0, 2) * view(2, 3));
        return params;
    }
}

#endif //ENGINE23_VIEWMATRIX_H
