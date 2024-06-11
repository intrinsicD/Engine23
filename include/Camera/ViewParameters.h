//
// Created by alex on 11.06.24.
//

#ifndef ENGINE23_VIEWPARAMETERS_H
#define ENGINE23_VIEWPARAMETERS_H

#include "LookAtRH.h"

namespace Bcg {
    template<typename T>
    struct ViewParameters {
        Eigen::Vector<T, 3> position = Eigen::Vector<T, 3>(0.0, 0.0, 3.0);
        Eigen::Vector<T, 3> up = Eigen::Vector<T, 3>(0.0, 1.0, 0.0);
        Eigen::Vector<T, 3> right = Eigen::Vector<T, 3>(1.0, 0.0, 0.0);
        Eigen::Vector<T, 3> world_up = Eigen::Vector<T, 3>(0.0, 1.0, 0.0);
        Eigen::Vector<T, 3> target = Eigen::Vector<T, 3>(0.0, 0.0, 0.0);
        bool dirty;

        Eigen::Matrix<T, 4, 4> get_matrix() const {
            return LookAtRH<T>(position, position + get_front(), up);
        }

        void set_position(const Eigen::Vector<T, 3> &position) {
            this->position = position;
            dirty = true;
        }

        void set_worldup(const Eigen::Vector<T, 3> &world_up) {
            Eigen::Vector<T, 3> front = get_front();

            this->world_up = world_up;
            right = front.cross(this->world_up).normalized();
            up = right.cross(front);
            dirty = true;
        }

        void set_target(const Eigen::Vector<T, 3> &target) {
            this->target = target;
            set_front(target - position);
        }

        void set_front(const Eigen::Vector<T, 3> &front) {
            Eigen::Vector<T, 3> front_normalized = front.normalized();
            right = front_normalized.cross(world_up).normalized();
            up = right.cross(front_normalized).normalized();
            dirty = true;
        }

        //front points into the screen
        Eigen::Vector<T, 3> get_front() const {
            return -right.cross(up);
        }
    };
}

#endif //ENGINE23_VIEWPARAMETERS_H
