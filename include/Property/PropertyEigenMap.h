//
// Created by alex on 22.02.24.
//

#ifndef ENGINE23_PROPERTYEIGENMAP_H
#define ENGINE23_PROPERTYEIGENMAP_H

#include "Property.h"
#include "IsEigenType.h"

namespace Bcg {
    template<typename T, std::enable_if_t<std::is_fundamental_v<T>, size_t> = 0>
    inline Eigen::Map<Eigen::Matrix<T, -1, 1>> Map(Property<T> &p) {
        return Eigen::Map<Eigen::Matrix<T, -1, 1 >>(&p[0], p.get_size(), 1);
    }

    template<typename T, std::enable_if_t<std::is_fundamental_v<T>, size_t> = 0>
    inline Eigen::Map<const Eigen::Matrix<T, -1, 1>> MapConst(const Property<T> &p) {
        return Eigen::Map<const Eigen::Matrix<T, -1, 1>>(&p[0], p.get_size(), 1);
    }

    template<typename T, std::enable_if_t<IsEigenType<T>, size_t> = 0>
    inline Eigen::Map<Eigen::Matrix<typename T::Scalar, -1, -1>, Eigen::Unaligned, Eigen::Stride<-1, -1>>
    Map(Property<T> &p) {
        return Eigen::Map<Eigen::Matrix<typename T::Scalar, -1, -1>, Eigen::Unaligned, Eigen::Stride<-1, -1 >>(
                p[0].data(),
                p.get_size(),
                p.get_dims(),
                Eigen::Stride<-1, -1>(1, p[0].size()));
    }

    template<typename T, std::enable_if_t<IsEigenType<T>, size_t> = 0>
    inline Eigen::Map<const Eigen::Matrix<typename T::Scalar, -1, -1>, Eigen::Unaligned, Eigen::Stride<-1, -1>>
    MapConst(const Property<T> &p) {
        return Eigen::Map<const Eigen::Matrix<typename T::Scalar, -1, -1>, Eigen::Unaligned, Eigen::Stride<-1, -1>>
                (
                        p[0].data(),
                        p.get_size(),
                        p.get_dims(),
                        Eigen::Stride<-1, -1>(1, p[0].size()));
    }
}


#endif //ENGINE23_PROPERTYEIGENMAP_H
