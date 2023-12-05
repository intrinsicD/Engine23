#[[target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC entt/src)]]
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC imgui)

target_compile_definitions(${PROJECT_NAME} PRIVATE -DIMGUI_IMPL_OPENGL_LOADER_GLAD)

add_subdirectory(glfw)

target_sources(${PROJECT_NAME} PRIVATE
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

add_subdirectory(entt)
target_link_libraries(${PROJECT_NAME} PUBLIC EnTT::EnTT)
add_subdirectory(fmt)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)