# CMake
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
endif()

# CXX
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Werror -Wall -Wpedantic -D_THREAD_SAFE")

# Project
project(FluidSims VERSION 1.0 LANGUAGES CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

if(APPLE)
    add_definitions(-Wno-deprecated-declarations)
endif()

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW
# PkgConfig is needed for the header, glfw3 for the library... probably
# something I should resolve.
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
find_package(glfw3 3.3 REQUIRED)

# Includes and libraries
set(INCLUDE_DIRS
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    "src"
    "ext"
)
set(LIBRARIES ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})

message(STATUS "OPENGL_INCLUDE_DIR = ${OPENGL_INCLUDE_DIR}")
message(STATUS "GLFW_INCLUDE_DIRS = ${GLFW_INCLUDE_DIRS}")

message(STATUS "OPENGL_LIBRARIES = ${OPENGL_LIBRARIES}")
message(STATUS "GLFW_LIBRARIES = ${GLFW_LIBRARIES}")

# Include search directories
include_directories(${INCLUDE_DIRS})

# Subdirectories
add_subdirectory(src)
add_subdirectory(apps)
