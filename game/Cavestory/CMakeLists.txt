cmake_minimum_required(VERSION 3.20)
# vcpkg configuration
set(CMAKE_TOOLCHAIN_FILE "D:/Programs/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(Cavestory)

set(CMAKE_PREFIX_PATH "D:/Programs/vcpkg/installed/x64-windows;${CMAKE_PREFIX_PATH}")

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE include)

#configure_file(D:/Programs/vcpkg/installed/x64-windows/bin/SDL2.dll ${CMAKE_CURRENT_BINARY_DIR}/SDL2.dll COPYONLY)
#configure_file(D:/Programs/vcpkg/installed/x64-windows/debug/bin/SDL2d.dll ${CMAKE_CURRENT_BINARY_DIR}/SDL2d.dll COPYONLY)
#configure_file(D:/Programs/vcpkg/installed/x64-windows/bin/SDL2_image.dll ${CMAKE_CURRENT_BINARY_DIR}/SDL2_image.dll COPYONLY)
#configure_file(D:/Programs/vcpkg/installed/x64-windows/debug/bin/SDL2_imaged.dll ${CMAKE_CURRENT_BINARY_DIR}/SDL2_imaged.dll COPYONLY)
#configure_file(D:/Programs/vcpkg/installed/x64-windows/bin/libpng16.dll ${CMAKE_CURRENT_BINARY_DIR}/libpng16.dll COPYONLY)
#configure_file(D:/Programs/vcpkg/installed/x64-windows/debug/bin/libpng16d.dll ${CMAKE_CURRENT_BINARY_DIR}/libpng16d.dll COPYONLY)

target_link_libraries(${PROJECT_NAME} PRIVATE
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_image::SDL2_image)

target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_TOOLCHAIN_FILE="D:/Programs/vcpkg/scripts/buildsystems/vcpkg.cmake")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/resource" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resource"
    COMMENT "Copying resource directory to output directory..."
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "D:/Programs/vcpkg/installed/x64-windows/bin" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "D:/Programs/vcpkg/installed/x64-windows/debug/bin" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMENT "Copying DLLs to output directory..."
)