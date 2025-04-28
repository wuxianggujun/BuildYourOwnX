cmake_minimum_required(VERSION 3.10)
project(hello_window VERSION 1.0)

# Macro for forced file/directory copy (executes on every run)
macro(force_copy_to_output target name path)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Force copying ${name}..."
            COMMAND ${CMAKE_COMMAND} -E remove -f "$<TARGET_FILE_DIR:${target}>/${name}"
            COMMAND ${CMAKE_COMMAND} -E copy "${path}" "$<TARGET_FILE_DIR:${target}>/${name}"
            COMMENT "Force copying ${name} to output directory"
    )
endmacro()

# Main executable
add_executable(${PROJECT_NAME}
        src/glad.c
        src/triangle_mesh.cpp
        src/main.cpp
        src/Config.cpp
        src/material.cpp)

# --- Forced copy operations ---
# DLL file (special handling)
set(GLFW_DLL_PATH "${GLFW_LIB_DIR}/glfw3.dll")
if(EXISTS ${GLFW_DLL_PATH})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Force copying DLL..."
            COMMAND ${CMAKE_COMMAND} -E remove -f "$<TARGET_FILE_DIR:${PROJECT_NAME}>/glfw3.dll"
            COMMAND ${CMAKE_COMMAND} -E copy "${GLFW_DLL_PATH}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMENT "Force copying glfw3.dll"
    )
else()
    message(WARNING "GLFW DLL not found: ${GLFW_DLL_PATH}")
endif()

# Directory copies (using macro)
force_copy_to_output(${PROJECT_NAME} "shaders" "${CMAKE_CURRENT_LIST_DIR}/shaders")
force_copy_to_output(${PROJECT_NAME} "img" "${CMAKE_CURRENT_LIST_DIR}/img")

# Other configurations...
target_include_directories(${PROJECT_NAME} PRIVATE src ${DEPENDENCIES_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIB_DIR}/glfw3dll.lib)
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
endif()