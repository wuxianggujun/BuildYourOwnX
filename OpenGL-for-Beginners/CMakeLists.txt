cmake_minimum_required(VERSION 3.20)
project(OpenGL-for-Beginners CXX)

set(GLFW_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib)
set(DEPENDENCIES_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)

add_subdirectory(week_01_hello_window)