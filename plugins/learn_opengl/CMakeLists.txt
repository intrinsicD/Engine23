add_library(bcg_plugin_learn_opengl SHARED)

add_subdirectory(include)
add_subdirectory(src)

target_link_libraries(bcg_plugin_learn_opengl PRIVATE Engine23Lib)
set_target_properties(bcg_plugin_learn_opengl PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/plugins
)

export(TARGETS bcg_plugin_learn_opengl NAMESPACE FILE BcgPluginLearnOpenGLTargets.cmake)

add_dependencies(Engine23 bcg_plugin_learn_opengl)