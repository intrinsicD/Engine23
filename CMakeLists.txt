cmake_minimum_required(VERSION 3.26)
project(Engine23)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#A custom target to build all the libraries and executables
add_custom_target(Engine23 ALL)

#The main library
add_library(Engine23Lib SHARED
        include/AABB/AABB.h
        include/AABB/AABBGui.h
        src/AABB/AABBGui.cpp
        include/Property/Property.h
        include/EigenUtils/IsEigenType.h
        include/Property/HandleBase.h
        include/Property/AbstractProperty.h
        include/Property/PropertyVector.h
        include/Property/PropertyUtils.h
        include/Property/PropertyContainer.h
        include/Property/PropertyIterator.h
        include/Property/VertexContainer.h
        src/Property/VertexContainer.cpp
        include/Property/HalfedgeContainer.h
        src/Property/HalfedgeContainer.cpp
        include/Property/EdgeContainer.h
        src/Property/EdgeContainer.cpp
        include/Property/FaceContainer.h
        src/Property/FaceContainer.cpp
        src/Property/AbstractProperty.cpp
        src/Property/PropertyContainer.cpp
        src/PointCloud/PointCloud.cpp
        include/Graph/Graph.h
        include/Mesh/MeshReader.h
        include/Mesh/MeshReaderOff.h
        src/Mesh/MeshReader.cpp
        src/Mesh/MeshReaderOff.cpp
        include/Mesh/MeshReaderObj.h
        src/Mesh/MeshReaderObj.cpp
        include/Mesh/MeshReaderStl.h
        src/Mesh/MeshReaderStl.cpp
        include/Mesh/MeshReaderPly.h
        src/Mesh/MeshReaderPly.cpp
        include/Mesh/MeshReaderPmp.h
        src/Mesh/MeshReaderPmp.cpp
        include/Mesh/MeshWriter.h
        include/Mesh/MeshWriterObj.h
        src/Mesh/MeshWriterObj.cpp
        include/Mesh/MeshWriterStl.h
        src/Mesh/MeshWriterStl.cpp
        include/Mesh/MeshWriterOff.h
        src/Mesh/MeshWriterOff.cpp
        include/Mesh/MeshWriterPly.h
        src/Mesh/MeshWriterPly.cpp
        include/Mesh/MeshWriterPmp.h
        src/Mesh/MeshWriterPmp.cpp
        src/Mesh/MeshWriter.cpp
)

add_subdirectory(app)
add_subdirectory(platform)
add_subdirectory(plugins)
add_subdirectory(backend)
add_subdirectory(ext)
add_subdirectory(include)
add_subdirectory(src)

export(TARGETS Engine23Lib glfw fmt glm NAMESPACE FILE BcgEngine23LibTargets.cmake)


