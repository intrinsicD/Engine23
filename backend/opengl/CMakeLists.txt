target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ext/glad/include)

target_sources(${PROJECT_NAME} PRIVATE SystemRendererOpenGL.cpp)