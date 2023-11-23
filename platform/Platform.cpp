//
// Created by alex on 23.11.23.
//

#include "Platform.h"
#include <iostream>

namespace Bcg {
    Platform::Platform() {
        // Check Operating System
#ifdef WINDOWS
        os = "Windows";
#elif defined(MACOS)
        os = "macOS";
#elif defined(LINUX)
        os = "Linux";
#else
        std::cout << "Unknown Operating System" << std::endl;
        os = "Unknown";
#endif
        std::cout << "Running on " << os << std::endl;
        // Check Architecture
#ifdef ARCH_64
        arch = "64-bit";
#elif defined(ARCH_32)
        arch = "32-bit";
#else
        arch = "Unknown";
#endif
        std::cout << arch << " Architecture" << std::endl;
        // Compiler specific code
#ifdef _MSC_VER
        compiler = "MSVC";
        compiler += " " + std::to_string(_MSC_VER);
#elif defined(__clang__)
        compiler = "Clang";
        compiler += " " + std::to_string(__clang_major__) + "." + std::to_string(__clang_minor__) + "." + std::to_string(__clang_patchlevel__);
#elif defined(__GNUC__)
        compiler = "GCC";
        compiler += " " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__) + "." + std::to_string(__GNUC_PATCHLEVEL__);
#else
        std::cout << "Compiled with an unknown compiler" << std::endl;
        compiler = "Unknown";
#endif
        std::cout << "Compiled with " << compiler << std::endl;
    }
}