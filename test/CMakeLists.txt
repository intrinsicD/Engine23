add_executable(Engine23Test)

add_subdirectory(ext)

target_include_directories(Engine23Test PUBLIC .)
target_link_libraries(Engine23Test PUBLIC Engine23Lib gtest gtest_main)

target_sources(Engine23Test PRIVATE
        TestCommandGraph.cpp
)