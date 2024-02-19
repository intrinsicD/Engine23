message(STATUS "CUDA compiler detected: ${CMAKE_CUDA_COMPILER} ${CMAKE_CUDA_COMPILER_VERSION}")

add_definitions(-DUSE_CUDA)
set(CMAKE_CUDA_ARCHITECTURES native)
message(STATUS "CMAKE_CUDA_ARCHITECTURES: ${CMAKE_CUDA_ARCHITECTURES}")

target_include_directories(Engine23Lib PUBLIC .)

message(STATUS "Cuda backend not supported yet")