//
// Created by alex on 26.02.24.
//

#include "Transform.h"
#include "Rotation3DAngleAxis.h"

namespace Bcg{

    Eigen::Vector<float, 3> Transform::get_position() const {
        return model.translation();
    }

    Eigen::Vector<float, 3> Transform::get_scale() const {
        auto linear = model.linear();
        return Eigen::Vector<float, 3>(linear.col(0).norm(), linear.col(1).norm(), linear.col(2).norm());
    }

    Eigen::Vector<float, 3> Transform::get_angles_axis() const {
        Rotation3DAngleAxis<float> angleAxis;
        angleAxis.from_matrix(model.rotation().matrix());
        return angleAxis.m_angle_axis;
    }

    Eigen::Vector<float, 3> Transform::get_x_axis() const {
        auto linear = model.linear();
        return linear.col(0);
    }

    Eigen::Vector<float, 3> Transform::get_y_axis() const {
        auto linear = model.linear();
        return linear.col(1);
    }

    Eigen::Vector<float, 3> Transform::get_z_axis() const {
        auto linear = model.linear();
        return linear.col(2);
    }

    void Transform::set_position(const Eigen::Vector<float, 3> &position) {
        model.translation() = position;
    }

    void Transform::set_scale(const Eigen::Vector<float, 3> &scale) {
        auto linear = model.linear();
        linear *= get_scale().asDiagonal().inverse() * scale.asDiagonal();
    }

    void Transform::set_rotation(const Eigen::Vector<float, 3> &axis, float angle) {
       auto scale = get_scale();
       Rotation3DAngleAxis<float> rot;
       rot.m_angle_axis = axis * angle;

       model.linear().setIdentity();
       model.linear() *= rot.matrix() * get_scale().asDiagonal();
    }
}