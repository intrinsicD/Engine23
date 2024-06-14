//
// Created by alex on 13.06.24.
//

#ifndef ENGINE23_TYPENAME_H
#define ENGINE23_TYPENAME_H

#include <string>
#include <typeinfo>
#include <cxxabi.h>

namespace Bcg{
    template<typename T>
    struct TypeName {
        static std::string get() {
            const char* name = typeid(T).name();
            int status = 0;
            char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
            std::string result = (status == 0) ? demangled : name;
            //remove Namespace
            result = result.substr(result.rfind("::") + 2);
            free(demangled);
            return result;
        }

        static const std::string& name() {
            static const std::string value = get();
            return value;
        }
    };
}



#endif //ENGINE23_TYPENAME_H
