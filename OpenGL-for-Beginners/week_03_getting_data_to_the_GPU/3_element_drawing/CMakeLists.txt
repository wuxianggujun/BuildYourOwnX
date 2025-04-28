cmake_minimum_required(VERSION 3.10)
project(hello_window VERSION 1.0)


add_executable(${PROJECT_NAME} src/glad.c src/triangle_mesh.cpp src/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE src ${DEPENDENCIES_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIB_DIR}/glfw3dll.lib)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
endif()

# --- (重要！) 添加复制 DLL 的步骤 ---
# 检查你的 .dll 文件名是否确实是 glfw3.dll
set(GLFW_DLL_PATH "${GLFW_LIB_DIR}/glfw3.dll")
if(EXISTS ${GLFW_DLL_PATH})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${GLFW_DLL_PATH}"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "Copying glfw3.dll to output directory")
else()
    message(WARNING "GLFW DLL not found at ${GLFW_DLL_PATH}. Runtime execution might fail.")
endif()

# --- (重要！) 添加复制 shaders 的步骤 ---
file(COPY "${CMAKE_CURRENT_LIST_DIR}/shaders" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")