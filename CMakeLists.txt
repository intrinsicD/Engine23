cmake_minimum_required(VERSION 3.26)
project(Engine23)

set(CMAKE_CXX_STANDARD 23)

add_executable(Engine23)

add_subdirectory(platform)
add_subdirectory(backend)
add_subdirectory(ext)
add_subdirectory(include)
add_subdirectory(src)


