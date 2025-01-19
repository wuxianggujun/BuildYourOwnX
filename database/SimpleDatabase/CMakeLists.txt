
####
# 教程 https://github.com/cstack/db_tutorial
# 搞不懂，我究竟是哪里翻译错误导致无法正确运行。心累
###

cmake_minimum_required(VERSION 3.20)
# vcpkg configuration
set(CMAKE_TOOLCHAIN_FILE "D:/Programs/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(SimpleDatabase)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH "D:/Programs/vcpkg/installed/x64-windows")
# 设置编译器标志
if (MSVC)
    # 对于 MSVC（Visual Studio 编译器），使用 /fsanitize=address
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address")
else()
    # 对于其他编译器（如 GCC 或 Clang），使用 -fsanitize=address
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

find_package(GTest CONFIG REQUIRED)

set(SRC_FILES
        src/Common.hpp
        src/Row.hpp
        src/Statement.hpp
        src/NodeHeader.hpp
        src/InternalNodeHeader.hpp
        src/LeafNodeHeader.hpp
        src/Node.hpp
        src/InternalNode.hpp
        src/LeafNode.hpp
        src/Cursor.hpp
        src/Pager.hpp
        src/Pager.cpp
        src/Table.hpp
        src/Table.cpp
        src/InputBuffer.hpp
        src/InputBuffer.cpp
        src/main.cpp
        src/Cursor.cpp
        src/InternalNode.cpp
        src/LeafNode.cpp
)
add_executable(${PROJECT_NAME} ${SRC_FILES})

add_compile_options(-fsanitize=address)
add_compile_options(-fno-omit-frame-pointer)
add_link_options(-fsanitize=address)
add_link_options(-fno-omit-frame-pointer)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)
# 链接 Google Test
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_compile_definitions(${PROJECT_NAME} PRIVATE CMAKE_TOOLCHAIN_FILE="D:/Programs/vcpkg/scripts/buildsystems/vcpkg.cmake")


#add_subdirectory(test)