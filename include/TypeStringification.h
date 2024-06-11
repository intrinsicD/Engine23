//
// Created by alex on 11.06.24.
//

#ifndef ENGINE23_TYPESTRINGIFICATION_H
#define ENGINE23_TYPESTRINGIFICATION_H

#include <string>

namespace Bcg {
#define BCG_STRINGIFY(x) #x
#define BCG_GENERATE_TYPE_STRING(T) template<> std::string TypeName<T>::name = BCG_STRINGIFY(T);

    template<typename T>
    struct TypeName {
        static std::string name;
    };

    template<typename T>
    std::string TypeName<T>::name = "UNKNOWN_TYPE";

    //BCG_GENERATE_TYPE_STRING(int)
}

#endif //ENGINE23_TYPESTRINGIFICATION_H
