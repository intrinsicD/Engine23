target_sources(Engine23Lib PRIVATE
        Commands.cpp
        Components.cpp
        Engine.cpp
        FilePath.cpp
        StringUtils.cpp
        ImGuiUtils.cpp
        Plugins.cpp
        SafeAcos.cpp
)
add_subdirectory(AABB)
add_subdirectory(Graph)
add_subdirectory(Mesh)
add_subdirectory(Property)
add_subdirectory(PointCloud)
add_subdirectory(systems)
