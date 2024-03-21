target_include_directories(Engine23Lib PUBLIC .)
# Detect operating systems
if (WIN32)
    add_subdirectory(opengl)
    add_subdirectory(vulkan)
elseif (APPLE)
    add_subdirectory(metal)
elseif (UNIX AND NOT APPLE)
    add_subdirectory(opengl)
    add_subdirectory(vulkan)
endif ()

enable_language(CUDA)

if (CMAKE_CUDA_COMPILER)
    add_subdirectory(cuda)
else ()
    message(STATUS "CUDA compiler not found")
endif ()