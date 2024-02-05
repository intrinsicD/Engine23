add_library(bcg_plugin_geometry_processing SHARED
        include/SystemPointCloud.h
        src/SystemPointCloud.cpp)

add_subdirectory(include)
add_subdirectory(src)

target_link_libraries(bcg_plugin_geometry_processing PRIVATE Engine23Lib)

export(TARGETS bcg_plugin_geometry_processing NAMESPACE FILE BcgPluginGeometryProcessingTargets.cmake)

add_dependencies(Engine23 bcg_plugin_geometry_processing)