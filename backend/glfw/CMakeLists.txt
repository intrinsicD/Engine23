target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC .)
target_sources(${PROJECT_NAME} PRIVATE WindowManagerGLFW.cpp)

add_subdirectory(ext)