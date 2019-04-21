cmake_minimum_required(VERSION 3.0.0)

project(RhythmMilk)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set C++ Compile Version
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# Exclude OpenGL documents, tests and examples.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Sub-directories
add_subdirectory(Dependencies/glfw)
add_subdirectory(RhythmEngine)
add_subdirectory(Client)
add_subdirectory(Metronome)

set_property(TARGET glfw PROPERTY FOLDER "Dependencies")