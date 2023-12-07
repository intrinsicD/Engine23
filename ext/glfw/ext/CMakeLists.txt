set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

target_include_directories(Engine23Lib SYSTEM PUBLIC glfw/include)
target_link_libraries(Engine23Lib PUBLIC glfw)