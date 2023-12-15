target_include_directories(Engine23Lib SYSTEM PUBLIC imgui)

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