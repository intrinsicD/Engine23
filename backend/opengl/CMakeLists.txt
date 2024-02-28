target_include_directories(Engine23Lib SYSTEM PUBLIC ext/glad/include)
find_package(OpenGL REQUIRED)
target_include_directories(Engine23Lib SYSTEM PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(Engine23Lib PUBLIC .)

target_sources(Engine23Lib PRIVATE
        OpenGLUtils.cpp
        SystemBuffers.cpp
        SystemVertexArrayObject.cpp
        SystemShaderPrograms.cpp
        SystemRendererOpenGL.cpp
        GLMeshRenderPass.cpp
)

target_link_libraries(Engine23Lib PRIVATE ${OPENGL_LIBRARIES})