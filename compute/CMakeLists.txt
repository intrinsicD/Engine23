target_include_directories(Engine23Lib PUBLIC .)
target_sources(Engine23Lib PRIVATE
        ComputeShader.cpp
)
add_subdirectory(FaceNormals)