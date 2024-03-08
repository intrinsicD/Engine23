//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_TRANSFORM_H
#define ENGINE23_TRANSFORM_H

#include "Eigen/Geometry"
#include "components/ComponentGui.h"

namespace Bcg{
    struct Transform {
        Eigen::Transform<float, 3, Eigen::Affine> model = Eigen::Transform<float, 3, Eigen::Affine>::Identity();

        Eigen::Vector<float, 3> get_position() const;

        Eigen::Vector<float, 3> get_scale() const;

        Eigen::Vector<float, 3> get_angles_axis() const;

        Eigen::Vector<float, 3> get_x_axis() const;

        Eigen::Vector<float, 3> get_y_axis() const;

        Eigen::Vector<float, 3> get_z_axis() const;

        void set_position(const Eigen::Vector<float, 3> &position);

        void set_scale(const Eigen::Vector<float, 3> &scale);

        void set_rotation(const Eigen::Vector<float, 3> &axis, float angle);
    };

}

#endif //ENGINE23_TRANSFORM_H
