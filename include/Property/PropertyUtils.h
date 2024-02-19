//
// Created by alex on 19.02.24.
//

#ifndef ENGINE23_PROPERTYUTILS_H
#define ENGINE23_PROPERTYUTILS_H

#include "IsEigenType.h"

namespace Bcg{
    template<typename T, std::enable_if_t<!IsEigenType<T>, size_t> = 0>
    inline std::stringstream &Print(std::stringstream &ss, const T &value) {
        ss << value;
        return ss;
    }

    template<typename T>
    inline std::stringstream &Print(std::stringstream &ss, const std::vector<T> &values) {
        for (const auto &item: values) {
            ss << item << " ";
        }
        ss << "\n";
        return ss;
    }

/*    template<typename T, std::enable_if_t<IsFundamentalType<T>, size_t> = 0>
    inline std::stringstream &Print(std::stringstream &ss, const T &value) {
        ss << value;
        return ss;
    }*/

    template<typename T, std::enable_if_t<IsEigenType<T>, size_t> = 0>
    inline std::stringstream &Print(std::stringstream &ss, const Eigen::EigenBase<T> &value) {
        ss << value.derived().transpose();
        return ss;
    }
}
#endif //ENGINE23_PROPERTYUTILS_H
