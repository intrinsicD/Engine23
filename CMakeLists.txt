cmake_minimum_required(VERSION 3.26)
project(Engine23)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#A custom target to build all the libraries and executables
add_custom_target(Engine23 ALL)

#The main library
add_library(Engine23Lib SHARED
        include/SafeNormalization.h
        include/Rotation/Rotation3D6dof.h
        include/Rendering/Material.h
        include/Camera/LookAtRH.h
        include/Camera/OrthoRH_NO.h
        include/Camera/PerspectiveRH_NO.h
        include/Visibility/VisibilityComponent.h)


add_subdirectory(app)
add_subdirectory(platform)
add_subdirectory(plugins)
add_subdirectory(backend)
add_subdirectory(ext)
add_subdirectory(include)
add_subdirectory(src)

export(TARGETS Engine23Lib glfw fmt glm TinyAD NAMESPACE FILE BcgEngine23LibTargets.cmake)


