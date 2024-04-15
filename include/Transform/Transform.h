//
// Created by alex on 03.01.24.
//

#ifndef ENGINE23_TRANSFORM_H
#define ENGINE23_TRANSFORM_H

#include "Eigen/Geometry"
#include "ComponentGui.h"
#include "Rotation3DAngleAxis.h"

namespace Bcg {
    template<typename T>
    class Transform {
    public:
        Eigen::Transform<T, 3, Eigen::Affine> model = Eigen::Transform<T, 3, Eigen::Affine>::Identity();

        Eigen::Vector<T, 3> get_position() const{
            return model.translation();
        }

        Eigen::Vector<T, 3> get_scale() const{
            return model.linear().colwise().norm();
        }

        Eigen::Vector<T, 3> get_angles_axis() const{
            Rotation3DAngleAxis<T> angleAxis;
            angleAxis.from_matrix(model.rotation().matrix());
            return angleAxis.m_angle_axis;
        }

        Eigen::Vector<T, 3> get_x_axis() const{
            return model.linear().col(0).normalized();
        }

        Eigen::Vector<T, 3> get_y_axis() const{
            return model.linear().col(1).normalized();
        }

        Eigen::Vector<T, 3> get_z_axis() const{
            return model.linear().col(2).normalized();
        }

        void set_position(const Eigen::Vector<T, 3> &position){
            model.translation() = position;

        }

        void set_scale(const Eigen::Vector<T, 3> &scale){
            auto linear = model.linear();
            linear *= get_scale().asDiagonal().inverse() * scale.asDiagonal();
        }

        void set_rotation(const Eigen::Vector<T, 3> &axis, T angle){
            auto scale = get_scale();
            Rotation3DAngleAxis<T> rot;
            rot.m_angle_axis = axis * angle;

            model.linear().setIdentity();
            model.linear() *= rot.matrix() * get_scale().asDiagonal();
        }
    };
}

#endif //ENGINE23_TRANSFORM_H
