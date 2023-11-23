# Detect operating systems
if(WIN32)
    message(STATUS "Building for Windows")
    add_definitions(-DWINDOWS)
    # Windows-specific flags go here
    add_subdirectory(windows)
elseif(APPLE)
    message(STATUS "Building for macOS")
    add_definitions(-DMACOS)
    # macOS-specific flags go here
    add_subdirectory(mac)
elseif(UNIX AND NOT APPLE)
    message(STATUS "Building for Linux")
    add_definitions(-DLINUX)
    # Linux-specific flags go here
    add_subdirectory(linux)
else()
    message(STATUS "Building for an unknown platform")
endif()

# Detect compilers and set flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Using Microsoft Visual C++ Compiler ${CMAKE_CXX_COMPILER_VERSION}")
    # MSVC-specific flags
    add_compile_options(/W4 /WX)

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "Using Clang Compiler ${CMAKE_CXX_COMPILER_VERSION}")
    # Clang-specific flags
    add_compile_options(-Wall -Wextra -pedantic -Werror)

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Using GNU Compiler Collection ${CMAKE_CXX_COMPILER_VERSION}")
    # GCC-specific flags
    add_compile_options(-Wall -Wextra -pedantic -Werror)

else()
    message(STATUS "Using an unknown compiler")
endif()

# Detect processor architecture and set flags
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "Building for a 64-bit architecture")
    add_definitions(-DARCH_64)
    # 64-bit specific flags go here

elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(STATUS "Building for a 32-bit architecture")
    add_definitions(-DARCH_32)
    # 32-bit specific flags go here

else()
    message(STATUS "Building for an unknown architecture")
endif()

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC .)
target_sources(${PROJECT_NAME} PRIVATE Platform.cpp)


