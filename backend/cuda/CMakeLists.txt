message(STATUS "CUDA compiler detected: ${CMAKE_CUDA_COMPILER} ${CMAKE_CUDA_COMPILER_VERSION}")
add_definitions(-DUSE_CUDA)
#set(CUDA_NVCC_FLAGS ...)

target_include_directories(${PROJECT_NAME} PUBLIC .)

message(STATUS "Cuda backend not supported yet")