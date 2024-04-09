add_executable(Engine23Test)

target_include_directories(Engine23Test PUBLIC .)
target_link_libraries(Engine23Test PUBLIC Engine23Lib gtest gtest_main)

target_sources(Engine23Test PRIVATE
        TestCommandGraph.cpp
        TestCommandGraphAdj.cpp
        TestEntt.cpp
        TestAABB.cpp
        TestLine.cpp
        TestPlane.cpp
        TestSphere.cpp
        TestTriangle.cpp
)