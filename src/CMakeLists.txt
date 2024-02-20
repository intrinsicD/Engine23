target_sources(Engine23Lib PRIVATE
        Commands.cpp
        Components.cpp
        Engine.cpp
        ImGuiUtils.cpp
        Plugins.cpp
)
add_subdirectory(AABB)
add_subdirectory(systems)
add_subdirectory(Property)
add_subdirectory(PointCloud)
