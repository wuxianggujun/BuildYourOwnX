cmake_minimum_required(VERSION 3.10)
project(hello_window VERSION 1.0)

add_executable(${PROJECT_NAME}
        src/glad.c
        src/triangle_mesh.cpp
        src/main.cpp
        src/Config.cpp
        src/material.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE src ${DEPENDENCIES_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIB_DIR}/glfw3dll.lib)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
endif()

# 定义复制函数宏
macro(copy_to_output target name path)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove_directory
            "$<TARGET_FILE_DIR:${target}>/${name}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${path}"
            "$<TARGET_FILE_DIR:${target}>/${name}"
            COMMENT "Copying ${name} to output directory")
endmacro()

# DLL复制（特殊处理单个文件）
set(GLFW_DLL_PATH "${GLFW_LIB_DIR}/glfw3.dll")
if(EXISTS ${GLFW_DLL_PATH})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${GLFW_DLL_PATH}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMENT "Copying glfw3.dll to output directory")
else()
    message(WARNING "GLFW DLL not found at ${GLFW_DLL_PATH}. Runtime execution might fail.")
endif()

# 使用宏复制目录
copy_to_output(${PROJECT_NAME} "shaders" "${CMAKE_CURRENT_LIST_DIR}/shaders")
copy_to_output(${PROJECT_NAME} "img" "${CMAKE_CURRENT_LIST_DIR}/img")