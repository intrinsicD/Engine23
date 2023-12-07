target_include_directories(Engine23Lib SYSTEM PUBLIC ext/glad/include)
target_include_directories(Engine23Lib PUBLIC .)

target_sources(Engine23Lib PRIVATE
        OpenGLUtils.cpp
        SystemBuffers.cpp
        SystemVertexArrayObject.cpp
        SystemShaderPrograms.cpp
        SystemRendererOpenGL.cpp
)