cmake_minimum_required(VERSION 3.10)

# Set the project name
project(SdfTask CXX)

# Find the SDL2 library
find_package(SDL2 REQUIRED)

# Include SDL2 headers
include_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/lib/LiteMath/external)
include_directories(${SDL2_INCLUDE_DIRS})

add_compile_definitions(USE_STB_IMAGE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Add the executable
add_executable(render
    src/main.cpp
    src/mesh.cpp
    src/RayTracer.cpp
    src/Hittable.cpp
    lib/LiteMath/Image2d.cpp
        src/Utils.h)

# Link the SDL2 library to the executable
target_link_libraries(render ${SDL2_LIBRARIES})

# Set path to executable
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
