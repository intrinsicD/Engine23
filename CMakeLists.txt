cmake_minimum_required(VERSION 3.26)
project(Engine23)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#A custom target to build all the libraries and executables
add_custom_target(Engine23 ALL)

#The main library
add_library(Engine23Lib SHARED)

add_subdirectory(app)
add_subdirectory(platform)
add_subdirectory(plugins)
add_subdirectory(backend)
add_subdirectory(ext)
add_subdirectory(include)
add_subdirectory(src)

export(TARGETS Engine23Lib glfw fmt NAMESPACE FILE BcgEngine23LibTargets.cmake)


