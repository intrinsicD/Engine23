set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)

add_subdirectory(tinygltf)
target_link_libraries(bcg_plugin_tiny_gltf PUBLIC tinygltf)