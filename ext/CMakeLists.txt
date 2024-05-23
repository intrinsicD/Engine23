target_include_directories(Engine23Lib SYSTEM PUBLIC imgui)
target_include_directories(Engine23Lib SYSTEM PUBLIC ImGuizmo)
target_include_directories(Engine23Lib SYSTEM PUBLIC soil/include)
target_include_directories(Engine23Lib SYSTEM PUBLIC happly)
target_include_directories(Engine23Lib SYSTEM PUBLIC nanoflann/include)

target_compile_definitions(Engine23Lib PRIVATE -DIMGUI_IMPL_OPENGL_LOADER_GLAD)

add_subdirectory(glfw)

target_sources(Engine23Lib PRIVATE
        imgui/imgui.cpp
        imgui/misc/cpp/imgui_stdlib.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
        ImGuizmo/ImGuizmo.cpp
        soil/src/soil.c
        soil/src/image_helper.c
        soil/src/stb_image_aug.c
        soil/src/image_dxt.c
#[[        implot/implot.cpp
        implot/implot_demo.cpp
        implot/implot_items.cpp]]
)
add_subdirectory(glm)
target_link_libraries(Engine23Lib PUBLIC glm::glm)
add_subdirectory(entt)
target_link_libraries(Engine23Lib PUBLIC EnTT::EnTT)
add_subdirectory(fmt)
target_link_libraries(Engine23Lib PUBLIC fmt::fmt)
add_subdirectory(eigen)
target_link_libraries(Engine23Lib PUBLIC Eigen3::Eigen)
add_subdirectory(TinyAD)
target_link_libraries(Engine23Lib PUBLIC TinyAD)
add_subdirectory(googletest)