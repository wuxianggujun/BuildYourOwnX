cmake_minimum_required(VERSION 3.20)
project(OpenGL-for-Beginners CXX)

set(GLFW_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib)
set(DEPENDENCIES_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)

#add_subdirectory(week_01_hello_window)
#add_subdirectory(week_02_what_are_shaders)
#add_subdirectory(week_03_getting_data_to_the_GPU/1_screen_coordinates)
#add_subdirectory(week_03_getting_data_to_the_GPU/2_multiple_buffers)
#add_subdirectory(week_03_getting_data_to_the_GPU/3_element_drawing)
#add_subdirectory(week_04_textures/1_drawing_the_texture)
add_subdirectory(week_04_textures/2_texture_units)

