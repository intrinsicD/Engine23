//
// Created by alex on 20.02.24.
//

#ifndef ENGINE23_FILESYSTEMSTL_H
#define ENGINE23_FILESYSTEMSTL_H

#if  __cplusplus < 201703L
#include <experimental/filesystem>

namespace Bcg {
    namespace Fs = std::experimental::filesystem;
}
#else

#include <filesystem>

namespace Bcg {
    namespace Fs = std::filesystem;
}
#endif

#endif //ENGINE23_FILESYSTEMSTL_H
