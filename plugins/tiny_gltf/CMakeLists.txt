add_library(bcg_plugin_tiny_gltf SHARED)

add_subdirectory(include)
add_subdirectory(ext)
add_subdirectory(src)

target_link_libraries(bcg_plugin_tiny_gltf PRIVATE Engine23Lib)

export(TARGETS bcg_plugin_tiny_gltf tinygltf NAMESPACE FILE BcgPluginTinyGltfTargets.cmake)

add_dependencies(Engine23 bcg_plugin_tiny_gltf)